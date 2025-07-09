#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_ACTIONS_MARKET_DATA_RECOVER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_ACTIONS_MARKET_DATA_RECOVER_HPP_

#include "common/market_state/snapshot.hpp"
#include "common/trade.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class MarketDataRecover : public EventReporter {
 public:
  explicit MarketDataRecover(EventListener& event_listener);

  auto operator()(std::optional<Trade> last_trade) -> void;

  auto operator()(std::optional<market_state::InstrumentInfo> info) -> void;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_ACTIONS_MARKET_DATA_RECOVER_HPP_
