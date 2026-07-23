#include "ih/orders/actions/cancellation.hpp"

#include <type_traits>

#include "ih/common/events/client_notification.hpp"
#include "ih/orders/replies/cancellation_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "ih/orders/tools/order_lookup.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

namespace {

template <typename Order>
auto& page_orders(OrderPage& page) {
  if constexpr (std::is_same_v<Order, LimitOrder>) {
    return page.limit_orders();
  } else {
    return page.market_orders();
  }
}

template <typename Order>
auto find_target(OrderPage& page, const OrderCancel& cancel) {
  if constexpr (std::is_same_v<Order, LimitOrder>) {
    return find_target_limit_order(page, cancel);
  } else {
    return find_target_market_order(page, cancel);
  }
}

}  // namespace

Cancellation::Cancellation(EventListener& event_listener,
                           OrderBook& order_book,
                           std::optional<PriceTick> price_tick)
    : EventReporter{event_listener},
      order_book_{order_book},
      price_tick_{price_tick} {}

auto Cancellation::operator()(const OrderCancel& cancel) -> void {
  log::debug("running order cancellation operation");

  const Side side = cancel.order_side;
  cancel_order(cancel, order_book_.take_page(side));
}

template <typename Order>
auto Cancellation::try_cancel(const OrderCancel& cancel,
                              OrderPage& page) -> bool {
  auto& orders = page_orders<Order>(page);

  const auto order_it = find_target<Order>(page, cancel);
  if (order_it == orders.end()) {
    return false;
  }

  Order order = *order_it;
  orders.erase(order_it);

  emit(order::make_making_order_removed_from_book_notification(order));
  order.cancel();
  emit(ClientNotification(
      prepare_cancellation_confirmation(order, price_tick_)
          .with_leaving_quantity(LeavesQuantity{0})
          .with_execution_id(order.make_execution_id())
          .with_client_order_id(cancel.client_order_id)
          .with_orig_client_order_id(cancel.orig_client_order_id)
          .build()));
  return true;
}

auto Cancellation::cancel_order(const OrderCancel& cancel,
                                OrderPage& page) -> void {
  if (try_cancel<LimitOrder>(cancel, page)) {
    return;
  }
  if (try_cancel<MarketOrder>(cancel, page)) {
    return;
  }

  emit(ClientNotification(prepare_cancellation_reject(cancel)
                              .with_reason(RejectText{"order not found"})
                              .build()));
}

}  // namespace simulator::trading_system::matching_engine
