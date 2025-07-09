#include "ih/orders/actions/regular_amendment.hpp"

#include "ih/common/events/client_notification.hpp"
#include "ih/orders/book/order_algorithms.hpp"
#include "ih/orders/replies/modification_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "ih/orders/tools/order_lookup.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

RegularAmendment::RegularAmendment(EventListener &event_listener,
                                   OrderBook &order_book,
                                   RegularMatcher &matcher)
    : EventReporter(event_listener),
      order_book_(order_book),
      matcher_(matcher) {}

auto RegularAmendment::operator()(LimitUpdate update) -> void {
  log::debug("running regular limit order amendment operation");

  const Side side = update.order_side;
  amend_order(std::move(update), order_book_.take_page(side));
}

auto RegularAmendment::amend_order(LimitUpdate update, OrderPage &page)
    -> void {
  const auto order_it = find_target_limit_order(page, update);
  if (order_it == limit_orders_end(page)) {
    emit(ClientNotification(prepare_modification_reject(update)
                                .with_reason(RejectText{"order not found"})
                                .build()));
    return;
  }

  if (static_cast<double>(update.order_diff.quantity) <=
      static_cast<double>(order_it->cum_executed_quantity())) {
    emit(ClientNotification(prepare_modification_reject(update)
                                .with_order_status(order_it->status())
                                .with_reason(RejectText{"invalid quantity"})
                                .build()));
    return;
  }

  if (order_it->time_in_force() !=
      update.order_diff.attributes.time_in_force()) {
    emit(ClientNotification(
        prepare_modification_reject(update)
            .with_order_status(order_it->status())
            .with_reason(RejectText{"time in force can not be changed"})
            .build()));
    return;
  }

  LimitOrder order = *order_it;
  page.limit_orders().erase(order_it);
  emit(order::make_making_order_removed_from_book_notification(order));

  order.amend(std::move(update.order_diff));
  emit(ClientNotification(
      prepare_modification_confirmation(order)
          .with_execution_id(order.make_execution_id())
          .with_orig_client_order_id(update.orig_client_order_id)
          .build()));

  matcher_.match(order);

  if (!order.executed()) {
    page.limit_orders().emplace(order);
    emit(order::make_making_order_added_to_book_notification(order));
  }
}

}  // namespace simulator::trading_system::matching_engine