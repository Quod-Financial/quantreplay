#include "ih/orders/tools/order_lookup.hpp"

#include <type_traits>

#include "ih/orders/book/order_algorithms.hpp"

namespace simulator::trading_system::matching_engine {
namespace {

template <typename Order>
auto find_order_by_order_id(OrderPage& page, OrderId order_id) {
  const auto pred = [order_id](const Order& order) {
    return order.id() == order_id;
  };

  if constexpr (std::is_same_v<Order, LimitOrder>) {
    return find_limit_order(page, pred);
  } else {
    return find_market_order(page, pred);
  }
}

template <typename Order>
auto find_order_by_client_order_id(OrderPage& page,
                                   const ClientOrderId& order_id,
                                   const protocol::Session& client_session) {
  const auto pred = [&order_id, &client_session](const Order& order) {
    return order.client_order_id() == order_id &&
           order.client_session() == client_session;
  };

  if constexpr (std::is_same_v<Order, LimitOrder>) {
    return find_unique_limit_order(page, pred);
  } else {
    return find_unique_market_order(page, pred);
  }
}

template <typename Order>
auto find_order_by_orig_client_order_id(
    OrderPage& page,
    const OrigClientOrderId& order_id,
    const protocol::Session& client_session) {
  const auto pred = [&order_id, &client_session](const Order& order) {
    if (const auto& client_order_id = order.client_order_id()) {
      return client_order_id->value() == order_id.value() &&
             order.client_session() == client_session;
    }
    return false;
  };

  if constexpr (std::is_same_v<Order, LimitOrder>) {
    return find_unique_limit_order(page, pred);
  } else {
    return find_unique_market_order(page, pred);
  }
}

// Resolves a cancel/amend target using the same identifier precedence for both
// the limit and the market container: venue order id, then orig client order
// id, then client order id.
template <typename Order, typename Request>
auto find_target_order(OrderPage& page, const Request& request) {
  if (request.order_id.has_value()) {
    return find_order_by_order_id<Order>(page, *request.order_id);
  }
  if (request.orig_client_order_id.has_value()) {
    return find_order_by_orig_client_order_id<Order>(
        page, *request.orig_client_order_id, request.client_session);
  }
  if (request.client_order_id.has_value()) {
    return find_order_by_client_order_id<Order>(
        page, *request.client_order_id, request.client_session);
  }
  if constexpr (std::is_same_v<Order, LimitOrder>) {
    return limit_orders_end(page);
  } else {
    return market_orders_end(page);
  }
}

}  // namespace

auto find_target_limit_order(OrderPage& page, const LimitUpdate& update)
    -> LimitOrdersContainer::iterator {
  return find_target_order<LimitOrder>(page, update);
}

auto find_target_market_order(OrderPage& page, const MarketUpdate& update)
    -> MarketOrdersContainer::iterator {
  return find_target_order<MarketOrder>(page, update);
}

auto find_target_limit_order(OrderPage& page, const OrderCancel& cancel)
    -> LimitOrdersContainer::iterator {
  return find_target_order<LimitOrder>(page, cancel);
}

auto find_target_market_order(OrderPage& page, const OrderCancel& cancel)
    -> MarketOrdersContainer::iterator {
  return find_target_order<MarketOrder>(page, cancel);
}

}  // namespace simulator::trading_system::matching_engine