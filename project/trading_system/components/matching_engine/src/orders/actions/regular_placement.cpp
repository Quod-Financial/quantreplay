#include "ih/orders/actions/regular_placement.hpp"

#include "ih/orders/replies/cancellation_reply_builders.hpp"
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

auto RegularPlacement::operator()(LimitOrder order) -> OrderBookUpdates {
  log::debug("running regular limit order placement operation");

  if (order.time_in_force() == TimeInForce::Option::ImmediateOrCancel) {
    return match_ioc_order(std::move(order));
  }
  if (order.time_in_force() == TimeInForce::Option::FillOrKill) {
    return match_fok_order(std::move(order));
  }
  return place_order(std::move(order));
}

auto RegularPlacement::operator()(MarketOrder order) -> OrderBookUpdates {
  log::debug("placing/matching market order: {}", order);

  if (!matcher_.has_facing_orders(order)) {
    order.cancel();
    emit(ClientNotification(
        prepare_cancellation_confirmation(order, std::nullopt)
            .with_leaving_quantity(LeavesQuantity{0})
            .with_execution_id(order.make_execution_id())
            .with_client_order_id(order.client_order_id())
            .with_cancellation_text(CancellationText{"no facing orders found"})
            .build()));
    return {};
  }

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));
  matcher_.match(order);

  // Market orders never rest; any resting-order updates from matching are not
  // captured here, since no consumer observes updates from regular-mode
  // matching yet.
  return {};
}

auto RegularPlacement::place_order(LimitOrder order) -> OrderBookUpdates {
  log::debug("placing limit order {}", order);

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));

  matcher_.match(order);

  if (order.executed()) {
    return {};
  }

  order_book_.take_page(order.side()).limit_orders().emplace(order);
  emit(order::make_making_order_added_to_book_notification(order));

  return {OrderBookUpdate{.side = order.side(),
                          .action = OrderBookUpdate::Action::Add,
                          .price = order.price(),
                          .quantity = order.leaves_quantity()}};
}

auto RegularPlacement::match_ioc_order(LimitOrder order) -> OrderBookUpdates {
  log::debug("matching IoC order {}", order);
  if (!matcher_.has_facing_orders(order)) {
    order.cancel();
    emit(ClientNotification(
        prepare_cancellation_confirmation(order, std::nullopt)
            .with_leaving_quantity(LeavesQuantity{0})
            .with_execution_id(order.make_execution_id())
            .with_client_order_id(order.client_order_id())
            .with_cancellation_text(CancellationText{"no facing orders found"})
            .build()));
    return {};
  }

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));
  matcher_.match(order);
  return {};
}

auto RegularPlacement::match_fok_order(LimitOrder order) -> OrderBookUpdates {
  log::debug("matching FoK order {}", order);

  if (!matcher_.has_facing_orders(order)) {
    order.cancel();
    emit(ClientNotification(
        prepare_cancellation_confirmation(order, std::nullopt)
            .with_leaving_quantity(LeavesQuantity{0})
            .with_execution_id(order.make_execution_id())
            .with_client_order_id(order.client_order_id())
            .with_cancellation_text(CancellationText{"no facing orders found"})
            .build()));
    return {};
  }

  if (!matcher_.can_fully_trade(order)) {
    order.cancel();
    emit(ClientNotification(
        prepare_cancellation_confirmation(order, std::nullopt)
            .with_leaving_quantity(LeavesQuantity{0})
            .with_execution_id(order.make_execution_id())
            .with_client_order_id(order.client_order_id())
            .with_cancellation_text(
                CancellationText{"not enough liquidity to fill FoK order"})
            .build()));
    return {};
  }

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));

  matcher_.match(order);
  return {};
}

}  // namespace simulator::trading_system::matching_engine