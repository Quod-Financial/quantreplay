#include "ih/orders/actions/cancellation.hpp"

#include "ih/common/events/client_notification.hpp"
#include "ih/orders/book/order_algorithms.hpp"
#include "ih/orders/replies/cancellation_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "ih/orders/tools/order_lookup.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

Cancellation::Cancellation(EventListener& event_listener, OrderBook& order_book)
    : EventReporter(event_listener), order_book_(order_book) {}

auto Cancellation::operator()(const OrderCancel& cancel) -> void {
  log::debug("running order cancellation operation");

  const Side side = cancel.order_side;
  cancel_order(cancel, order_book_.take_page(side));
}

auto Cancellation::cancel_order(const OrderCancel& cancel, OrderPage& page)
    -> void {
  const auto order_it = find_target_limit_order(page, cancel);
  if (order_it == limit_orders_end(page)) {
    emit(ClientNotification(prepare_cancellation_reject(cancel)
                                .with_reason(RejectText{"order not found"})
                                .build()));
    return;
  }

  auto order = *order_it;
  page.limit_orders().erase(order_it);
  emit(order::make_making_order_removed_from_book_notification(order));

  order.cancel();

  emit(ClientNotification(
      prepare_cancellation_confirmation(order)
          .with_execution_id(order.make_execution_id())
          .with_client_order_id(cancel.client_order_id)
          .with_orig_client_order_id(cancel.orig_client_order_id)
          .build()));
}

}  // namespace simulator::trading_system::matching_engine