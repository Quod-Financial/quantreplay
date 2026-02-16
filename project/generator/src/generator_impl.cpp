#include "ih/generator_impl.hpp"

#include <memory>

#include "data_layer/api/data_access_layer.hpp"
#include "ih/adaptation/protocol_conversion.hpp"
#include "ih/context/generation_manager.hpp"
#include "ih/context/order_generation_context_impl.hpp"
#include "ih/factory/executable_factory_impl.hpp"
#include "ih/registry/registry_updater.hpp"
#include "ih/utils/executor.hpp"
#include "ih/utils/validator.hpp"
#include "log/logging.hpp"

namespace simulator::generator {

GeneratorImpl::GeneratorImpl(const data_layer::Venue& target_venue,
                             data_layer::database::Context database_context)
    : database_context_(std::move(database_context)),
      generation_manager_{GenerationManager::create(target_venue)},
      rnd_executor_factory_{InstrumentRandomGeneratorFactoryImpl::create()},
      hist_executor_factory_{std::make_unique<HistoricalReplierFactoryImpl>()} {
  assert(generation_manager_);

  initialize_instruments();
  initialize_random_generation_executors();
  initialize_historical_executor();

  log::info("generator initialized successfully");
}

GeneratorImpl::~GeneratorImpl() noexcept { terminate_generator(); }

auto GeneratorImpl::status() -> bool {
  return generation_manager_->is_component_running();
}

auto GeneratorImpl::suspend() -> void { generation_manager_->suspend(); }

auto GeneratorImpl::resume() -> void { generation_manager_->launch(); }

auto GeneratorImpl::start() -> void {
  for (const auto& random_orders_executable : listings_random_generators_) {
    random_orders_executable->launch();
  }

  if (historical_replier_) {
    historical_replier_->launch();
  }
}

auto GeneratorImpl::initialize_instruments() -> void {
  for (const auto& listing :
       data_layer::select_all_listings(database_context_)) {
    initialize_instrument(listing);
  }
}

auto GeneratorImpl::initialize_instrument(const data_layer::Listing& listing)
    -> void {
  const auto& current_venue = generation_manager_->get_venue();
  if (listing.venue_id() != current_venue.venue_id()) {
    log::debug(
        "generator ignores listing `{}' (id: {}), "
        "which does not belong to current venue `{}'",
        listing.symbol(),
        listing.listing_id(),
        current_venue.venue_id());
    return;
  }

  if (!Validator::is_acceptable(listing)) {
    return;
  }

  const auto internal_instrument_id = listing.listing_id();
  InstrumentDescriptor descriptor = convert_to_instrument_descriptor(listing);
  descriptor.requester_instrument_id =
      RequesterInstrumentId{internal_instrument_id};

  const auto context = OrderInstrumentContextImpl::create(
      listing, descriptor, generation_manager_);

  order_listings_contexts_.emplace_back(context);
  context_lookup_.emplace(internal_instrument_id, context);

  log::debug("configured order generation context for the `{}'",
             listing.symbol());
}

auto GeneratorImpl::initialize_random_generation_executors() -> void {
  for (const auto& instrument_ctx : order_listings_contexts_) {
    const auto& instrument = instrument_ctx->get_instrument();

    if (!instrument.instr_symbol().has_value()) {
      log::info(
          "can not initialize random orders generation executable for "
          "instrument id `{}' - the instrument does not have an instr_symbol",
          instrument.listing_id());
      continue;
    }

    std::unique_ptr<Executable> executable;
    try {
      const auto px_seed = data_layer::select_one_price_seed(
          database_context_,
          data_layer::predicate::eq<data_layer::PriceSeed>(
              data_layer::PriceSeed::Attribute::InstrumentSymbol,
              *instrument.instr_symbol()));

      executable = rnd_executor_factory_->create_orders_executable(
          instrument_ctx, px_seed);
    } catch (std::logic_error& e) {
      log::info(
          "can not initialize random orders generation executable for "
          "instrument with the instr_symbol `{}' - no price seed entry has "
          "been found for the instrument",
          instrument.instr_symbol());
      continue;
    }

    if (executable) {
      listings_random_generators_.emplace_back(
          Executor::create(std::move(executable), generation_manager_));
    }
  }
}

auto GeneratorImpl::initialize_historical_executor() -> void {
  if (order_listings_contexts_.empty()) {
    historical_replier_ = nullptr;
    return;
  }

  using data_layer::Datasource;
  using data_layer::DatasourceCmp;
  using Attribute = data_layer::Datasource::Attribute;
  const auto& operating_venue_id = generation_manager_->get_venue().venue_id();
  auto pred = DatasourceCmp::eq(Attribute::VenueId, operating_venue_id) &&
              DatasourceCmp::eq(Attribute::Enabled, true);

  const std::vector<Datasource> datasources =
      data_layer::select_all_datasources(database_context_, std::move(pred));

  auto executable = hist_executor_factory_->create_orders_executable(
      datasources, order_listings_contexts_);

  historical_replier_ =
      Executor::create(std::move(executable), generation_manager_);
}

auto GeneratorImpl::terminate() noexcept -> void { terminate_generator(); }

auto GeneratorImpl::process_reply(std::uint64_t instrument_id,
                                  const GeneratedMessage& reply) -> void {
  const auto context_iter = context_lookup_.find(instrument_id);
  if (context_iter == context_lookup_.end()) {
    log::warn(
        "can not process reply message for instrument {}, "
        "no context has been found",
        instrument_id);
    return;
  }

  OrderRegistryUpdater::update(context_iter->second->take_registry(), reply);

  log::debug("reply message for instrument {} processed", instrument_id);
}

auto GeneratorImpl::terminate_generator() noexcept -> void {
  if (was_terminated_) {
    return;
  }

  was_terminated_ = true;
  for (const auto& random_orders_executable : listings_random_generators_) {
    random_orders_executable->terminate();
  }
  listings_random_generators_.clear();

  if (historical_replier_) {
    historical_replier_->terminate();
  }

  generation_manager_->terminate();
}

}  // namespace simulator::generator
