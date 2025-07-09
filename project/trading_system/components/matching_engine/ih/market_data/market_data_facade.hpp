#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_MARKET_DATA_FACADE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_MARKET_DATA_FACADE_HPP_

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/abstractions/market_data_publisher.hpp"
#include "ih/common/abstractions/market_data_request_processor.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/market_data/actions/market_data_recover.hpp"
#include "ih/market_data/cache/cache_manager.hpp"
#include "ih/market_data/subscriptions/subscription_manager.hpp"
#include "ih/market_data/validation/validator.hpp"
#include "matching_engine/configuration.hpp"

namespace simulator::trading_system::matching_engine {

class MarketDataFacade : public MarketDataRequestProcessor,
                         public MarketDataPublisher {
 public:
  auto handle(OrderBookNotification notification) -> void;

  auto publish() -> void override;

  auto process(const protocol::MarketDataRequest& request) -> void override;

  auto capture(protocol::InstrumentState& state) -> void override;

  auto store_state(std::optional<Trade>& last_trade,
                   std::optional<market_state::InstrumentInfo>& info)
      -> void override;

  auto recover_state(std::optional<Trade> last_trade,
                     std::optional<market_state::InstrumentInfo> info)
      -> void override;

  auto stop_streaming(const protocol::Session& client_session) -> void override;

  static auto setup(const Configuration& configuration, EventListener& listener)
      -> MarketDataFacade;

 private:
  explicit MarketDataFacade(const Configuration& configuration,
                            EventListener& listener,
                            std::unique_ptr<mdata::Validator> validator);

  auto validate(const std::optional<Trade>& last_trade) const -> bool;

  auto validate(const std::optional<market_state::InstrumentInfo>& info) const
      -> bool;

  std::unique_ptr<mdata::Validator> validator_;
  mdata::MarketDataRecover recover_;

  mdata::CacheManager cache_manager_;
  mdata::SubscriptionManager subscription_manager_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_PROJECT_V2_TRADING_SYSTEM_COMPONENTS_MATCHING_ENGINE_IH_MARKET_DATA_MARKET_DATA_FACADE_HPP_
