#include "ih/orders/actions/early_price_reporter.hpp"

#include <chrono>

#include "ih/common/data/market_data_updates.hpp"
#include "ih/common/events/order_book_notification.hpp"

namespace simulator::trading_system::matching_engine::order {

EarlyPriceReporter::EarlyPriceReporter(EventListener& event_listener,
                                       const PhaseHandler& phase_handler)
    : EventReporter{event_listener}, phase_handler_{&phase_handler} {}

auto EarlyPriceReporter::operator()(
    const event::Tick& tick, const std::optional<AuctionResult>& auction_result)
    -> void {
  if (!phase_handler_->in_auction_call()) {
    baseline_.reset();
    return;
  }

  if (!baseline_.has_value()) {
    baseline_ = tick.sys_tick_time;
    return;
  }

  constexpr auto interval = std::chrono::seconds{30};
  if (tick.sys_tick_time - *baseline_ < interval) {
    return;
  }
  baseline_ = tick.sys_tick_time;

  if (auction_result.has_value()) {
    emit(OrderBookNotification{EarlyPriceUpdate{
        .early_value = TradeResult{.price = auction_result->price,
                                   .quantity = auction_result->quantity}}});
  }
}

auto EarlyPriceReporter::report_cleared() -> void {
  emit(OrderBookNotification{EarlyPriceUpdate{}});
  baseline_.reset();
}

}  // namespace simulator::trading_system::matching_engine::order
