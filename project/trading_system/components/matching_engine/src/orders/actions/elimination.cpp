#include "ih/orders/actions/elimination.hpp"

#include "ih/orders/replies/cancellation_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine::order {

namespace {

auto get_expire_time(const LimitOrder& order) -> std::optional<core::sys_us> {
  std::optional<core::sys_us> expire_time;
  if (const auto ord_expire_time = order.expire_time()) {
    expire_time = static_cast<core::sys_us>(*ord_expire_time);
  }
  return expire_time;
}

auto get_expire_date(const LimitOrder& order)
    -> std::optional<core::local_days> {
  std::optional<core::local_days> expire_date;
  if (const auto ord_expire_date = order.expire_date()) {
    expire_date = static_cast<core::local_days>(*ord_expire_date);
  }
  return expire_date;
}

}  // namespace

SystemElimination::SystemElimination(EventListener& event_listener,
                                     event::Tick system_tick)
    : EventReporter(event_listener),
      current_expire_time_(system_tick.sys_tick_time),
      current_expire_date_(
          core::as_local_time(core::to_date(system_tick.tz_tick_time))),
      is_new_day_(system_tick.is_new_tz_day) {}

auto SystemElimination::operator()(OrderBook& book) const -> void {
  log::trace("eliminating buy orders");
  eliminate_expired(book.buy_page().limit_orders());

  log::trace("eliminating sell orders");
  eliminate_expired(book.sell_page().limit_orders());

  log::trace("finished checking for orders eliminated");
}

auto SystemElimination::eliminate_expired(LimitOrdersContainer& orders) const
    -> void {
  for (auto iter = orders.begin(); iter != orders.end();) {
    if (is_expired(*iter)) {
      eliminate(*iter);
      iter = orders.erase(iter);
    } else {
      ++iter;
    }
  }
}

auto SystemElimination::is_expired(const LimitOrder& order) const -> bool {
  const auto time_in_force = order.time_in_force();
  bool expired = false;
  if (time_in_force == TimeInForce::Option::Day) {
    // all DAY orders become expired once new day starts
    expired = is_new_day_;
  } else if (time_in_force == TimeInForce::Option::GoodTillDate) {
    if (const auto expire_time = get_expire_time(order)) {
      // GTD orders with expire time become expired at expire time
      expired = current_expire_time_ >= *expire_time;
    } else if (const auto expire_date = get_expire_date(order)) {
      // GTD orders with expire date become expired at the end of the day
      expired = is_new_day_ && current_expire_date_ > *expire_date;
    }
  }
  return expired;
}

auto SystemElimination::eliminate(LimitOrder& order) const -> void {
  log::trace("eliminating expired order: {}", order);
  order.cancel();
  emit(make_making_order_removed_from_book_notification(order));
  emit(ClientNotification(prepare_cancellation_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .with_client_order_id(order.client_order_id())
                              .build()));
  log::debug("cancelled eliminated order {}", order);
}

AllOrdersElimination::AllOrdersElimination(EventListener& event_listener)
    : EventReporter{event_listener} {}

auto AllOrdersElimination::operator()(OrderBook& book) const -> void {
  log::trace("eliminating all buy orders");
  eliminate(book.buy_page().limit_orders());

  log::trace("eliminating all sell orders");
  eliminate(book.sell_page().limit_orders());

  log::trace("finished eliminating all orders");
}

auto AllOrdersElimination::eliminate(LimitOrdersContainer& orders) const
    -> void {
  for (auto iter = orders.begin(); iter != orders.end();) {
    eliminate(*iter);
    iter = orders.erase(iter);
  }
}

auto AllOrdersElimination::eliminate(LimitOrder& order) const -> void {
  log::trace("eliminating order: {}", order);
  order.cancel();
  emit(make_making_order_removed_from_book_notification(order));
  log::debug("eliminated the order {}", order.id());
}

ClosedPhaseElimination::ClosedPhaseElimination(EventListener& event_listener,
                                               core::tz_us phase_tz_start_time)
    : EventReporter(event_listener),
      phase_start_date_(
          core::as_local_time(core::to_date(phase_tz_start_time))) {}

auto ClosedPhaseElimination::operator()(OrderBook& book) const -> void {
  log::trace("eliminating buy orders");
  eliminate_expired(book.buy_page().limit_orders());

  log::trace("eliminating sell orders");
  eliminate_expired(book.sell_page().limit_orders());

  log::trace("finished checking for orders eliminated");
}

auto ClosedPhaseElimination::eliminate_expired(
    LimitOrdersContainer& orders) const -> void {
  for (auto iter = orders.begin(); iter != orders.end();) {
    if (is_expired(*iter)) {
      eliminate(*iter);
      iter = orders.erase(iter);
    } else {
      ++iter;
    }
  }
}

auto ClosedPhaseElimination::is_expired(const LimitOrder& order) const -> bool {
  const auto time_in_force = order.time_in_force();
  bool expired = false;

  if (time_in_force == TimeInForce::Option::Day) {
    // all DAY orders become expired at CLO phase starts
    expired = true;
  } else if (time_in_force == TimeInForce::Option::GoodTillDate) {
    if (const auto expire_date = get_expire_date(order)) {
      // GTD orders with expire date become expired once CLO phase starts
      expired = phase_start_date_ >= *expire_date;
    }
  }

  return expired;
}

auto ClosedPhaseElimination::eliminate(LimitOrder& order) const -> void {
  log::trace("client disconnected, eliminating order: {}", order);
  order.cancel();
  emit(make_making_order_removed_from_book_notification(order));
  emit(ClientNotification(prepare_cancellation_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .with_client_order_id(order.client_order_id())
                              .build()));
  log::debug("eliminated the order {} due to client disconnect", order.id());
}

OnDisconnectElimination::OnDisconnectElimination(
    EventListener& event_listener,
    const protocol::Session& disconnected_session)
    : EventReporter(event_listener),
      disconnected_session_(&disconnected_session) {}

auto OnDisconnectElimination::operator()(OrderBook& book) const -> void {
  log::trace("eliminating buy orders due to user disconnect");
  handle_eliminated_orders(book.buy_page().limit_orders());

  log::trace("eliminating sell orders due to user disconnect");
  handle_eliminated_orders(book.sell_page().limit_orders());

  log::trace("finished checking for orders eliminated due to user disconnect");
}

auto OnDisconnectElimination::handle_eliminated_orders(
    LimitOrdersContainer& orders) const -> void {
  for (auto iter = orders.begin(); iter != orders.end();) {
    if (should_be_eliminated(*iter)) {
      eliminate(*iter);
      iter = orders.erase(iter);
    } else {
      ++iter;
    }
  }
}

auto OnDisconnectElimination::should_be_eliminated(
    const LimitOrder& order) const -> bool {
  return order.time_in_force() == TimeInForce::Option::Day &&
         order.client_session() == *disconnected_session_;
}

auto OnDisconnectElimination::eliminate(LimitOrder& order) const -> void {
  log::trace("client disconnected, eliminating order: {}", order);
  order.cancel();
  emit(make_making_order_removed_from_book_notification(order));
  emit(ClientNotification(prepare_cancellation_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .with_client_order_id(order.client_order_id())
                              .build()));

  log::debug("eliminated the order {} due to client disconnect", order.id());
}

}  // namespace simulator::trading_system::matching_engine::order