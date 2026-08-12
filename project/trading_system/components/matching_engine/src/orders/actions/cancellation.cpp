#include "ih/orders/actions/cancellation.hpp"

#include <type_traits>

#include "ih/common/events/client_notification.hpp"
#include "ih/orders/replies/cancellation_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "ih/orders/tools/order_lookup.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

namespace {

template <typename Container>
constexpr bool holds_limit_orders =
    std::is_same_v<Container, LimitOrdersContainer>;

template <typename Container>
auto find_target(Container& orders, const OrderCancel& cancel) {
  if constexpr (holds_limit_orders<Container>) {
    return find_target_limit_order(orders, cancel);
  } else {
    return find_target_market_order(orders, cancel);
  }
}

}  // namespace

Cancellation::Cancellation(EventListener& event_listener,
                           OrderBook& order_book,
                           std::optional<PriceTick> price_tick,
                           LimitOrderQueue queue)
    : EventReporter{event_listener},
      order_book_{order_book},
      price_tick_{price_tick},
      queue_{queue} {}

auto Cancellation::operator()(const OrderCancel& cancel) -> OrderBookUpdates {
  log::debug("running order cancellation operation");

  const Side side = cancel.order_side;
  return cancel_order(cancel, order_book_.take_page(side));
}

template <typename Container>
auto Cancellation::try_cancel(const OrderCancel& cancel, Container& orders)
    -> std::optional<OrderBookUpdate> {
  const auto order_it = find_target(orders, cancel);
  if (order_it == orders.end()) {
    return std::nullopt;
  }

  typename Container::value_type order = *order_it;
  orders.erase(order_it);

  std::optional<OrderPrice> price;
  if constexpr (holds_limit_orders<Container>) {
    price = order.price();
  }
  const OrderBookUpdate removal{.side = order.side(),
                                .action = OrderBookUpdate::Action::Remove,
                                .price = price,
                                .quantity = order.leaves_quantity()};

  emit(order::make_making_order_removed_from_book_notification(order));
  order.cancel();
  emit(ClientNotification(
      prepare_cancellation_confirmation(order, price_tick_)
          .with_leaving_quantity(LeavesQuantity{0})
          .with_execution_id(order.make_execution_id())
          .with_client_order_id(cancel.client_order_id)
          .with_orig_client_order_id(cancel.orig_client_order_id)
          .build()));

  return removal;
}

auto Cancellation::cancel_order(const OrderCancel& cancel, OrderPage& page)
    -> OrderBookUpdates {
  if (const auto removal =
          try_cancel(cancel, select_limit_orders(page, queue_))) {
    return {*removal};
  }
  if (const auto removal = try_cancel(cancel, page.market_orders())) {
    return {*removal};
  }

  emit(ClientNotification(prepare_cancellation_reject(cancel)
                              .with_reason(RejectText{"order not found"})
                              .build()));
  return {};
}

}  // namespace simulator::trading_system::matching_engine
