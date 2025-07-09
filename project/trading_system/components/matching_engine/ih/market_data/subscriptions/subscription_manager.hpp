#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_SUBSCRIPTIONS_SUBSCRIPTION_MANAGER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_SUBSCRIPTIONS_SUBSCRIPTION_MANAGER_HPP_

#include <memory>

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/market_data/cache/market_data_provider.hpp"
#include "ih/market_data/subscriptions/subscription.hpp"
#include "matching_engine/configuration.hpp"
#include "protocol/app/market_data_request.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class SubscriptionManager : EventReporter {
  class Index;

 public:
  SubscriptionManager(Configuration configuration,
                      EventListener& listener,
                      MarketDataProvider& market_data_provider);

  SubscriptionManager() = delete;
  SubscriptionManager(const SubscriptionManager&) = delete;
  SubscriptionManager(SubscriptionManager&&) = delete;
  ~SubscriptionManager() noexcept override;

  auto operator=(const SubscriptionManager&) -> SubscriptionManager& = delete;
  auto operator=(SubscriptionManager&&) -> SubscriptionManager& = delete;

  auto process(protocol::MarketDataRequest request) -> void;

  auto unsubscribe(const protocol::Session& client_session) -> void;

  auto publish() -> void;

 private:
  auto validate(const protocol::MarketDataRequest& request) -> bool;

  auto snapshot(protocol::MarketDataRequest request) const -> void;

  auto subscribe(protocol::MarketDataRequest request) -> void;

  auto unsubscribe(protocol::MarketDataRequest request) -> void;

  auto create_subscription(const protocol::MarketDataRequest& request) const
      -> Subscription;

  auto trade_streaming_enabled() const -> bool;

  Configuration configuration_;
  std::unique_ptr<Index> index_;
  MarketDataProvider& data_provider_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_SUBSCRIPTIONS_SUBSCRIPTION_MANAGER_HPP_
