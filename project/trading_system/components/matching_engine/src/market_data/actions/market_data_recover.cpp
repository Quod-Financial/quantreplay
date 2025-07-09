#include "ih/market_data/actions/market_data_recover.hpp"

namespace simulator::trading_system::matching_engine::mdata {

MarketDataRecover::MarketDataRecover(EventListener& event_listener)
    : EventReporter{event_listener} {}

auto MarketDataRecover::operator()(std::optional<Trade> last_trade) -> void {
  emit(OrderBookNotification{LastTradeRecover{std::move(last_trade)}});
}

auto MarketDataRecover::operator()(
    std::optional<market_state::InstrumentInfo> info) -> void {
  emit(OrderBookNotification{InstrumentInfoRecover{std::move(info)}});
}

}  // namespace simulator::trading_system::matching_engine::mdata
