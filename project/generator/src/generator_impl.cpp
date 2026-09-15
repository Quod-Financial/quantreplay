#include "ih/generator_impl.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "data_layer/api/data_access_layer.hpp"
#include "ih/adaptation/protocol_conversion.hpp"
#include "ih/context/generation_manager.hpp"
#include "ih/context/order_generation_context_impl.hpp"
#include "ih/factory/executable_factory_impl.hpp"
#include "ih/historical/processor.hpp"
#include "ih/market_data/market_data_provider.hpp"
#include "ih/market_data/utils.hpp"
#include "ih/random/seed_hasher.hpp"
#include "ih/registry/registry_updater.hpp"
#include "ih/utils/executor.hpp"
#include "ih/utils/validator.hpp"
#include "log/logging.hpp"

namespace simulator::generator {

GeneratorImpl::GeneratorImpl(const data_layer::Venue& target_venue,
                             data_layer::database::Context database_context)
    : database_context_(std::move(database_context)),
      generation_manager_{GenerationManager::create(target_venue)},
      price_cache_{std::make_shared<mdata::PriceCache>()},
      local_subscription_manager_{price_cache_, generation_manager_},
      subscription_manager_{price_cache_, generation_manager_},
      rnd_executor_factory_{InstrumentRandomGeneratorFactoryImpl::create()},
      hist_executor_factory_{std::make_unique<HistoricalReplierFactoryImpl>()} {
  assert(generation_manager_);

  const std::vector<data_layer::Listing> listings = select_venue_listings();
  const std::vector<data_layer::Datasource> datasources =
      select_venue_datasources();

  initialize_instruments(listings);

  const std::vector<RandomGenerationCandidate> candidates =
      select_random_generation_candidates();
  std::unordered_set<mdata::ListingId> random_generation_listings;
  for (const RandomGenerationCandidate& candidate : candidates) {
    random_generation_listings.emplace(
        candidate.context->get_instrument().listing_id());
  }

  initialize_market_data_subscriptions(
      datasources, listings, random_generation_listings);
  initialize_order_book_derivation(datasources);
  initialize_random_generation_executors(candidates);
  initialize_historical_executor(datasources);

  log::info("generator initialized successfully");
}

GeneratorImpl::~GeneratorImpl() noexcept { terminate_generator(); }

auto GeneratorImpl::status() -> bool {
  return generation_manager_->is_component_running();
}

auto GeneratorImpl::suspend() -> void {
  generation_manager_->suspend();

  unsubscribe_from_market_data();
}

auto GeneratorImpl::resume(const std::optional<std::string>& user_seed)
    -> void {
  //  We assume that there are only two threads changing PRNG state:
  //  1. Order generation state, managed by executor
  //  2. "Admin" thread invoking GeneratorImpl::resume
  //  The behaviour is undefined when:
  //  1. Reseed is called when order generation thread is running
  //  2. More than one thread is executing GeneratorImpl::resume at the same
  //  time
  //  3. PRNG is used by more than one thread (OrderGenerator thread)
  if (generation_manager_->is_component_running()) {
    log::info(
        "start generation request ignored - generation is already running "
        "(any user-provided seed is ignored)");
    return;
  }

  if (user_seed.has_value()) {
    log::info(
        "re-seeding {} per-listing random order generators "
        "before resume (user seed provided)",
        listings_random_generators_.size());

    for (auto& random_generator : listings_random_generators_) {
      const auto listing_seed =
          random_generator.seed_hasher.with_seed(*user_seed);
      random_generator.random_root->reseed(listing_seed);
    }
  }

  subscribe_to_market_data();

  generation_manager_->launch();
}

auto GeneratorImpl::start() -> void {
  if (generation_manager_->is_component_running()) {
    subscribe_to_market_data();
  }

  for (const auto& entry : listings_random_generators_) {
    entry.executor->launch();
  }

  if (historical_replier_) {
    historical_replier_->launch();
  }
}

auto GeneratorImpl::initialize_instruments(
    const std::vector<data_layer::Listing>& listings) -> void {
  for (const auto& listing : listings) {
    initialize_instrument(listing);
  }
}

auto GeneratorImpl::initialize_instrument(const data_layer::Listing& listing)
    -> void {
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

auto GeneratorImpl::select_random_generation_candidates() const
    -> std::vector<RandomGenerationCandidate> {
  std::vector<RandomGenerationCandidate> candidates;

  for (const auto& instrument_ctx : order_listings_contexts_) {
    const auto& instrument = instrument_ctx->get_instrument();

    if (!instrument.instr_symbol().has_value()) {
      log::info(
          "can not initialize random orders generation executable for "
          "instrument id `{}' - the instrument does not have an instr_symbol",
          instrument.listing_id());
      continue;
    }

    if (!Validator::is_acceptable_for_random_generation(instrument)) {
      continue;
    }

    std::optional<data_layer::PriceSeed> px_seed;
    try {
      px_seed = data_layer::select_one_price_seed(
          database_context_,
          data_layer::predicate::eq<data_layer::PriceSeed>(
              data_layer::PriceSeed::Attribute::InstrumentSymbol,
              *instrument.instr_symbol()));
    } catch (const std::logic_error&) {
      log::info(
          "can not initialize random orders generation executable for "
          "instrument with the instr_symbol `{}' - no price seed entry has "
          "been found for the instrument",
          instrument.instr_symbol());
      continue;
    }

    if (!Validator::is_acceptable(*px_seed)) {
      continue;
    }

    candidates.push_back(RandomGenerationCandidate{
        .context = instrument_ctx, .price_seed = *std::move(px_seed)});
  }

  return candidates;
}

auto GeneratorImpl::initialize_random_generation_executors(
    std::span<const RandomGenerationCandidate> candidates) -> void {
  for (const RandomGenerationCandidate& candidate : candidates) {
    const auto& instrument_ctx = candidate.context;

    auto order_generator = rnd_executor_factory_->create_orders_executable(
        instrument_ctx,
        candidate.price_seed,
        make_market_data_provider(*instrument_ctx));

    auto* random_root = order_generator.get();
    listings_random_generators_.push_back(RandomGenerationEntry{
        Executor::create(std::move(order_generator), generation_manager_),
        random_root,
        random::ListingSeedHasher{instrument_ctx->get_instrument()}});
  }
}

auto GeneratorImpl::make_market_data_provider(
    const OrderInstrumentContext& instrument_ctx)
    -> std::unique_ptr<mdata::MarketDataProvider> {
  const MdRequestId local_request_id = local_subscription_manager_.add(
      instrument_ctx.get_instrument_descriptor());

  const auto& instrument = instrument_ctx.get_instrument();
  std::vector<MdRequestId> external_request_ids;
  if (const auto request_ids =
          random_price_request_ids_.find(instrument.listing_id());
      request_ids != random_price_request_ids_.end()) {
    external_request_ids = request_ids->second;
    log::info(
        "random prices of the `{}' listing (id: {}) are based on the top of "
        "the book aggregated from its own order book and {} market data "
        "subscription(s)",
        instrument.symbol(),
        instrument.listing_id(),
        external_request_ids.size());
  } else {
    log::info(
        "random prices of the `{}' listing (id: {}) are based on the top of "
        "the book of its own order book",
        instrument.symbol(),
        instrument.listing_id());
  }

  return std::make_unique<mdata::CachedMarketDataProvider>(
      price_cache_, local_request_id, std::move(external_request_ids));
}

auto GeneratorImpl::select_venue_listings() const
    -> std::vector<data_layer::Listing> {
  using data_layer::ListingCmp;
  using Attribute = data_layer::Listing::Attribute;
  const auto& operating_venue_id = generation_manager_->get_venue().venue_id();

  return data_layer::select_all_listings(
      database_context_,
      ListingCmp::eq(Attribute::VenueId, operating_venue_id) &&
          ListingCmp::eq(Attribute::Enabled, true));
}

auto GeneratorImpl::select_venue_datasources() const
    -> std::vector<data_layer::Datasource> {
  using data_layer::DatasourceCmp;
  using Attribute = data_layer::Datasource::Attribute;
  const auto& operating_venue_id = generation_manager_->get_venue().venue_id();
  auto pred = DatasourceCmp::eq(Attribute::VenueId, operating_venue_id) &&
              DatasourceCmp::eq(Attribute::Enabled, true);

  return data_layer::select_all_datasources(database_context_, std::move(pred));
}

auto GeneratorImpl::initialize_historical_executor(
    const std::vector<data_layer::Datasource>& datasources) -> void {
  if (order_listings_contexts_.empty()) {
    historical_replier_ = nullptr;
    return;
  }

  std::vector<data_layer::Datasource> non_fix_datasources;
  std::ranges::copy_if(datasources,
                       std::back_inserter(non_fix_datasources),
                       [](const data_layer::Datasource& datasource) {
                         return datasource.format() !=
                                data_layer::Datasource::Format::Fix;
                       });

  auto executable = hist_executor_factory_->create_orders_executable(
      non_fix_datasources, order_listings_contexts_);

  historical_replier_ =
      Executor::create(std::move(executable), generation_manager_);
}

auto GeneratorImpl::initialize_market_data_subscriptions(
    const std::vector<data_layer::Datasource>& datasources,
    const std::vector<data_layer::Listing>& listings,
    const std::unordered_set<mdata::ListingId>& random_generation_listings)
    -> void {
  const auto datasource_requirements =
      mdata::make_datasource_subscription_requirements(datasources);
  for (const auto& requirement : datasource_requirements) {
    subscription_manager_.add_datasource_subscription(requirement);
  }

  const auto listing_price_sources = mdata::make_listing_random_price_sources(
      datasources, listings, random_generation_listings);
  for (const auto& [listing_id, price_sources] : listing_price_sources) {
    std::vector<MdRequestId> request_ids;
    request_ids.reserve(price_sources.size());

    for (const mdata::SubscriptionKey& price_source : price_sources) {
      const auto request_id = subscription_manager_.find_request_id(
          price_source.session, price_source.symbol);
      if (!request_id.has_value()) {
        log::warn(
            "the random prices of the listing (id: {}) are based on the `{}' "
            "symbol, which is not subscribed to, ignoring the random price "
            "source",
            listing_id,
            price_source.symbol);
        continue;
      }

      request_ids.push_back(*request_id);
      price_cache_->add(*request_id);
    }

    if (!request_ids.empty()) {
      random_price_request_ids_.emplace(listing_id, std::move(request_ids));
    }
  }
}

auto GeneratorImpl::initialize_order_book_derivation(
    const std::vector<data_layer::Datasource>& datasources) -> void {
  order_book_derivation_ = std::make_unique<mdata::OrderBookDerivation>(
      std::make_shared<historical::ActionProcessor>(order_listings_contexts_),
      generation_manager_);

  std::unordered_set<std::string_view> listing_symbols;
  for (const auto& context : order_listings_contexts_) {
    const auto& symbol = context->get_instrument().symbol();
    if (symbol.has_value()) {
      listing_symbols.emplace(*symbol);
    }
  }

  for (mdata::OrderDerivationRequirement& requirement :
       mdata::make_order_derivation_requirements(datasources)) {
    if (!listing_symbols.contains(requirement.symbol.value())) {
      log::warn(
          "the `{}' symbol subscribed to on an external feed does not match "
          "any order generation listing of the venue, no orders are derived "
          "from its market data",
          requirement.symbol);
      continue;
    }

    const auto request_id = subscription_manager_.find_request_id(
        requirement.session, requirement.symbol);
    if (!request_id.has_value()) {
      log::warn(
          "the `{}' symbol is not subscribed to on the external feed, no "
          "orders are derived from its market data",
          requirement.symbol);
      continue;
    }

    log::info(
        "orders for the `{}' listing are derived from the `{}' market data "
        "subscription",
        requirement.symbol,
        *request_id);
    order_book_derivation_->add(*request_id, std::move(requirement));
  }
}

auto GeneratorImpl::subscribe_to_market_data() -> void {
  local_subscription_manager_.subscribe();
  subscription_manager_.subscribe();
}

auto GeneratorImpl::unsubscribe_from_market_data() -> void {
  local_subscription_manager_.unsubscribe();
  subscription_manager_.unsubscribe();
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

auto GeneratorImpl::process_market_data(
    const protocol::MarketDataSnapshot& snapshot) -> void {
  price_cache_->process(snapshot);
  order_book_derivation_->process(snapshot);
}

auto GeneratorImpl::process_market_data(
    const protocol::MarketDataReject& reject) -> void {
  subscription_manager_.handle_reject(reject);
}

auto GeneratorImpl::process_session_connection(
    const protocol::SessionConnectedEvent& event) -> void {
  subscription_manager_.handle_session_connected(event.session);
}

auto GeneratorImpl::process_session_disconnection(
    const protocol::SessionTerminatedEvent& event) -> void {
  subscription_manager_.handle_session_disconnected(event.session);
}

auto GeneratorImpl::terminate_generator() noexcept -> void {
  if (was_terminated_) {
    return;
  }

  was_terminated_ = true;
  unsubscribe_from_market_data();

  for (auto& entry : listings_random_generators_) {
    entry.executor->terminate();
  }
  listings_random_generators_.clear();

  if (historical_replier_) {
    historical_replier_->terminate();
  }

  generation_manager_->terminate();
}

}  // namespace simulator::generator
