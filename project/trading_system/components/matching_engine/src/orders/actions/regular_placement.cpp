#include "ih/orders/actions/regular_placement.hpp"

#include "ih/orders/replies/placement_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

RegularPlacement::RegularPlacement(EventListener& event_listener,
                                   OrderBook& order_book,
                                   RegularMatcher& matcher)
    : EventReporter(event_listener),
      order_book_(order_book),
      matcher_(matcher) {}

auto RegularPlacement::operator()(LimitOrder order) -> void {
  log::debug("running regular limit order placement operation");

  if (order.time_in_force() == TimeInForce::Option::ImmediateOrCancel) {
    match_ioc_order(std::move(order));
  } else if (order.time_in_force() == TimeInForce::Option::FillOrKill) {
    match_fok_order(std::move(order));
  } else {
    place_order(std::move(order));
  }
}

auto RegularPlacement::operator()(MarketOrder order) -> void {
  log::debug("placing/matching market order: {}", order);

  if (!matcher_.has_facing_orders(order)) {
    emit(ClientNotification(
        prepare_placement_reject(order)
            .with_reason(RejectText{"no facing orders found"})
            .build()));
    return;
  }

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));
  matcher_.match(order);
}

auto RegularPlacement::place_order(LimitOrder order) -> void {
  log::debug("placing limit order {}", order);

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));

  matcher_.match(order);

  if (!order.executed()) {
    order_book_.take_page(order.side()).limit_orders().emplace(order);
    emit(order::make_making_order_added_to_book_notification(order));
  }
}

auto RegularPlacement::match_ioc_order(LimitOrder order) -> void {
  log::debug("matching IoC order {}", order);
  if (!matcher_.has_facing_orders(order)) {
    emit(ClientNotification(
        prepare_placement_reject(order)
            .with_reason(RejectText{"no facing orders found"})
            .build()));
    return;
  }

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));
  matcher_.match(order);
}

auto RegularPlacement::match_fok_order(LimitOrder order) -> void {
  log::debug("matching FoK order {}", order);

  if (!matcher_.has_facing_orders(order)) {
    emit(ClientNotification(
        prepare_placement_reject(order)
            .with_reason(RejectText{"no facing orders found"})
            .build()));
    return;
  }

  if (!matcher_.can_fully_trade(order)) {
    emit(ClientNotification(
        prepare_placement_reject(order)
            .with_reason(RejectText("not enough liquidity to fill FoK order"))
            .build()));
    return;
  }

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));

  matcher_.match(order);
}

}  // namespace simulator::trading_system::matching_engine