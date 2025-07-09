#include "ih/orders/tools/order_lookup.hpp"

#include "ih/orders/actions/order_action_handler.hpp"
#include "ih/orders/book/order_algorithms.hpp"

namespace simulator::trading_system::matching_engine {
namespace {

auto find_limit_order_by_order_id(OrderPage& page, OrderId order_id)
    -> LimitOrdersContainer::iterator {
  const auto pred = [order_id](const LimitOrder& order) {
    return order.id() == order_id;
  };

  return find_limit_order(page, pred);
}

auto find_limit_order_by_client_order_id(
    OrderPage& page,
    const ClientOrderId& order_id,
    const protocol::Session& client_session) -> LimitOrdersContainer::iterator {
  const auto pred = [&order_id, &client_session](const LimitOrder& order) {
    return order.client_order_id() == order_id &&
           order.client_session() == client_session;
  };

  return find_unique_limit_order(page, pred);
}

auto find_limit_order_by_orig_client_order_id(
    OrderPage& page,
    const OrigClientOrderId& order_id,
    const protocol::Session& client_session) -> LimitOrdersContainer::iterator {
  const auto pred = [&order_id, &client_session](const LimitOrder& order) {
    if (const auto& client_order_id = order.client_order_id()) {
      return client_order_id->value() == order_id.value() &&
             order.client_session() == client_session;
    }
    return false;
  };

  return find_unique_limit_order(page, pred);
}

}  // namespace

auto find_target_limit_order(OrderPage& page, const LimitUpdate& update)
    -> LimitOrdersContainer::iterator {
  if (update.order_id.has_value()) {
    return find_limit_order_by_order_id(page, *update.order_id);
  }
  if (update.orig_client_order_id.has_value()) {
    return find_limit_order_by_orig_client_order_id(
        page, *update.orig_client_order_id, update.client_session);
  }
  if (update.client_order_id.has_value()) {
    return find_limit_order_by_client_order_id(
        page, *update.client_order_id, update.client_session);
  }
  return limit_orders_end(page);
}

auto find_target_limit_order(OrderPage& page, const OrderCancel& cancel)
    -> LimitOrdersContainer::iterator {
  if (cancel.order_id.has_value()) {
    return find_limit_order_by_order_id(page, *cancel.order_id);
  }
  if (cancel.orig_client_order_id.has_value()) {
    return find_limit_order_by_orig_client_order_id(
        page, *cancel.orig_client_order_id, cancel.client_session);
  }
  if (cancel.client_order_id.has_value()) {
    return find_limit_order_by_client_order_id(
        page, *cancel.client_order_id, cancel.client_session);
  }
  return limit_orders_end(page);
}

}  // namespace simulator::trading_system::matching_engine