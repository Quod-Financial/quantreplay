#include "ih/orders/actions/time_reporter.hpp"

#include "ih/common/data/market_data_updates.hpp"
#include "ih/common/events/order_book_notification.hpp"

namespace simulator::trading_system::matching_engine::order {

TimeReporter::TimeReporter(EventListener& event_listener)
    : EventReporter{event_listener} {}

auto TimeReporter::operator()(const event::Tick& tick) const -> void {
  if (tick.is_new_tz_day) {
    emit(OrderBookNotification{
        TzDayPassed{.sys_tick_time = tick.sys_tick_time}});
  }
}

}  // namespace simulator::trading_system::matching_engine::order
