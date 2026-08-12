#include "ih/orders/actions/regular_amendment.hpp"

#include "ih/common/events/client_notification.hpp"
#include "ih/orders/replies/modification_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "ih/orders/tools/order_lookup.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

RegularAmendment::RegularAmendment(EventListener& event_listener,
                                   OrderBook& order_book,
                                   RegularMatcher& matcher,
                                   std::optional<PriceTick> price_tick,
                                   LimitOrderQueue queue)
    : EventReporter{event_listener},
      order_book_{order_book},
      matcher_{matcher},
      price_tick_{price_tick},
      queue_{queue} {}

auto RegularAmendment::operator()(LimitUpdate update) -> OrderBookUpdates {
  log::debug("running regular limit order amendment operation");

  const Side side = update.order_side;
  return amend_order(std::move(update),
                     select_limit_orders(order_book_.take_page(side), queue_));
}

auto RegularAmendment::amend_order(LimitUpdate update,
                                   LimitOrdersContainer& orders)
    -> OrderBookUpdates {
  const auto order_it = find_target_limit_order(orders, update);
  if (order_it == orders.end()) {
    emit(ClientNotification(prepare_modification_reject(update)
                                .with_reason(RejectText{"order not found"})
                                .build()));
    return {};
  }

  if (static_cast<double>(update.order_diff.quantity) <=
      static_cast<double>(order_it->cum_executed_quantity())) {
    emit(ClientNotification(prepare_modification_reject(update)
                                .with_order_status(order_it->status())
                                .with_reason(RejectText{"invalid quantity"})
                                .build()));
    return {};
  }

  if (order_it->time_in_force() !=
      update.order_diff.attributes.time_in_force()) {
    emit(ClientNotification(
        prepare_modification_reject(update)
            .with_order_status(order_it->status())
            .with_reason(RejectText{"time in force can not be changed"})
            .build()));
    return {};
  }

  const OrderBookUpdate removal{.side = order_it->side(),
                                .action = OrderBookUpdate::Action::Remove,
                                .price = order_it->price(),
                                .quantity = order_it->leaves_quantity()};

  LimitOrder order = *order_it;
  orders.erase(order_it);
  emit(order::make_making_order_removed_from_book_notification(order));

  order.amend(std::move(update.order_diff));
  emit(ClientNotification(
      prepare_modification_confirmation(order, price_tick_)
          .with_execution_id(order.make_execution_id())
          .with_orig_client_order_id(update.orig_client_order_id)
          .build()));

  matcher_.match(order);

  if (order.executed()) {
    return {removal};
  }

  orders.emplace(order);
  emit(order::make_making_order_added_to_book_notification(order));

  return {removal,
          OrderBookUpdate{.side = order.side(),
                          .action = OrderBookUpdate::Action::Add,
                          .price = order.price(),
                          .quantity = order.leaves_quantity()}};
}

}  // namespace simulator::trading_system::matching_engine
