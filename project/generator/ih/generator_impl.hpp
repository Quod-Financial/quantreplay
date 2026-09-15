#ifndef SIMULATOR_GENERATOR_IH_GENERATOR_IMPL_HPP_
#define SIMULATOR_GENERATOR_IH_GENERATOR_IMPL_HPP_

#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core/domain/attributes.hpp"
#include "data_layer/api/database/context.hpp"
#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/price_seed.hpp"
#include "ih/context/generation_manager.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/factory/executable_factory.hpp"
#include "ih/market_data/local_subscription_manager.hpp"
#include "ih/market_data/market_data_provider.hpp"
#include "ih/market_data/order_book_derivation.hpp"
#include "ih/market_data/price_cache.hpp"
#include "ih/market_data/subscription_manager.hpp"
#include "ih/market_data/utils.hpp"
#include "ih/random/instrument_generator.hpp"
#include "ih/random/seed_hasher.hpp"
#include "ih/utils/executor.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"
#include "protocol/app/session_connected_event.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::generator {

class GeneratorImpl final {
 public:
  GeneratorImpl() = delete;

  GeneratorImpl(const data_layer::Venue& target_venue,
                data_layer::database::Context database_context);

  GeneratorImpl(const GeneratorImpl&) = delete;
  auto operator=(const GeneratorImpl&) -> GeneratorImpl& = delete;

  GeneratorImpl(GeneratorImpl&&) = delete;
  auto operator=(GeneratorImpl&&) -> GeneratorImpl& = delete;

  ~GeneratorImpl() noexcept;

  auto status() -> bool;

  auto suspend() -> void;

  auto resume(const std::optional<std::string>& user_seed) -> void;

  auto start() -> void;

  auto terminate() noexcept -> void;

  auto process_reply(std::uint64_t instrument_id, const GeneratedMessage& reply)
      -> void;

  auto process_market_data(const protocol::MarketDataSnapshot& snapshot)
      -> void;

  auto process_market_data(const protocol::MarketDataReject& reject) -> void;

  auto process_session_connection(const protocol::SessionConnectedEvent& event)
      -> void;

  auto process_session_disconnection(
      const protocol::SessionTerminatedEvent& event) -> void;

 private:
  struct RandomGenerationEntry {
    std::unique_ptr<Executor> executor;
    random::OrderGenerator* random_root;
    random::ListingSeedHasher seed_hasher;
  };

  struct RandomGenerationCandidate {
    std::shared_ptr<OrderInstrumentContext> context;
    data_layer::PriceSeed price_seed;
  };

  auto initialize_instruments(const std::vector<data_layer::Listing>& listings)
      -> void;

  auto initialize_instrument(const data_layer::Listing& listing) -> void;

  [[nodiscard]]
  auto select_random_generation_candidates() const
      -> std::vector<RandomGenerationCandidate>;

  auto initialize_random_generation_executors(
      std::span<const RandomGenerationCandidate> candidates) -> void;

  [[nodiscard]]
  auto make_market_data_provider(const OrderInstrumentContext& instrument_ctx)
      -> std::unique_ptr<mdata::MarketDataProvider>;

  [[nodiscard]]
  auto select_venue_listings() const -> std::vector<data_layer::Listing>;

  [[nodiscard]]
  auto select_venue_datasources() const -> std::vector<data_layer::Datasource>;

  auto initialize_historical_executor(
      const std::vector<data_layer::Datasource>& datasources) -> void;

  auto initialize_market_data_subscriptions(
      const std::vector<data_layer::Datasource>& datasources,
      const std::vector<data_layer::Listing>& listings,
      const std::unordered_set<mdata::ListingId>& random_generation_listings)
      -> void;

  auto initialize_order_book_derivation(
      const std::vector<data_layer::Datasource>& datasources) -> void;

  auto subscribe_to_market_data() -> void;

  auto unsubscribe_from_market_data() -> void;

  auto terminate_generator() noexcept -> void;

  std::vector<RandomGenerationEntry> listings_random_generators_;
  std::vector<std::shared_ptr<OrderInstrumentContext>> order_listings_contexts_;
  std::unordered_map<std::uint64_t, std::shared_ptr<OrderInstrumentContext>>
      context_lookup_;

  std::unordered_map<mdata::ListingId, std::vector<MdRequestId>>
      random_price_request_ids_;

  data_layer::database::Context database_context_;

  std::shared_ptr<GenerationManager> generation_manager_;

  std::shared_ptr<mdata::PriceCache> price_cache_;
  mdata::LocalSubscriptionManager local_subscription_manager_;
  mdata::SubscriptionManager subscription_manager_;
  std::unique_ptr<mdata::OrderBookDerivation> order_book_derivation_;

  std::unique_ptr<Executor> historical_replier_;
  std::unique_ptr<InstrumentRandomGeneratorFactory> rnd_executor_factory_;

  std::unique_ptr<InstrumentHistoricalReplierFactory> hist_executor_factory_;

  bool was_terminated_{false};
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_GENERATOR_IMPL_HPP_
