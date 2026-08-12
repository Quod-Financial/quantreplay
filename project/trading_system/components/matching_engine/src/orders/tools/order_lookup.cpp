#include "ih/orders/tools/order_lookup.hpp"

#include <type_traits>

#include "ih/orders/book/order_algorithms.hpp"

namespace simulator::trading_system::matching_engine {
namespace {

template <typename Container>
constexpr bool holds_limit_orders =
    std::is_same_v<Container, LimitOrdersContainer>;

template <typename Container>
using StoredOrder = typename Container::value_type;

template <typename Container>
auto find_order_by_order_id(Container& orders, OrderId order_id) {
  const auto pred = [order_id](const StoredOrder<Container>& order) {
    return order.id() == order_id;
  };

  if constexpr (holds_limit_orders<Container>) {
    return find_limit_order(orders, pred);
  } else {
    return find_market_order(orders, pred);
  }
}

template <typename Container>
auto find_order_by_client_order_id(Container& orders,
                                   const ClientOrderId& order_id,
                                   const protocol::Session& client_session) {
  const auto pred = [&order_id,
                     &client_session](const StoredOrder<Container>& order) {
    return order.client_order_id() == order_id &&
           order.client_session() == client_session;
  };

  if constexpr (holds_limit_orders<Container>) {
    return find_unique_limit_order(orders, pred);
  } else {
    return find_unique_market_order(orders, pred);
  }
}

template <typename Container>
auto find_order_by_orig_client_order_id(
    Container& orders,
    const OrigClientOrderId& order_id,
    const protocol::Session& client_session) {
  const auto pred = [&order_id,
                     &client_session](const StoredOrder<Container>& order) {
    if (const auto& client_order_id = order.client_order_id()) {
      return client_order_id->value() == order_id.value() &&
             order.client_session() == client_session;
    }
    return false;
  };

  if constexpr (holds_limit_orders<Container>) {
    return find_unique_limit_order(orders, pred);
  } else {
    return find_unique_market_order(orders, pred);
  }
}

// Resolves a cancel/amend target using the same identifier precedence for both
// the limit and the market container: venue order id, then orig client order
// id, then client order id.
template <typename Container, typename Request>
auto find_target_order(Container& orders, const Request& request) {
  if (request.order_id.has_value()) {
    return find_order_by_order_id(orders, *request.order_id);
  }
  if (request.orig_client_order_id.has_value()) {
    return find_order_by_orig_client_order_id(
        orders, *request.orig_client_order_id, request.client_session);
  }
  if (request.client_order_id.has_value()) {
    return find_order_by_client_order_id(
        orders, *request.client_order_id, request.client_session);
  }
  return orders.end();
}

}  // namespace

auto find_target_limit_order(LimitOrdersContainer& orders,
                             const LimitUpdate& update)
    -> LimitOrdersContainer::iterator {
  return find_target_order(orders, update);
}

auto find_target_market_order(MarketOrdersContainer& orders,
                              const MarketUpdate& update)
    -> MarketOrdersContainer::iterator {
  return find_target_order(orders, update);
}

auto find_target_limit_order(LimitOrdersContainer& orders,
                             const OrderCancel& cancel)
    -> LimitOrdersContainer::iterator {
  return find_target_order(orders, cancel);
}

auto find_target_market_order(MarketOrdersContainer& orders,
                              const OrderCancel& cancel)
    -> MarketOrdersContainer::iterator {
  return find_target_order(orders, cancel);
}

}  // namespace simulator::trading_system::matching_engine
