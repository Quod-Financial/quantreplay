#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_UPDATES_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_UPDATES_HPP_

#include <optional>

#include "common/attributes.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine {

struct LimitUpdate {
  LimitUpdate(protocol::Session session, Side side, LimitOrder::Update diff);

  protocol::Session client_session;
  LimitOrder::Update order_diff;
  std::optional<ClientOrderId> client_order_id;
  std::optional<OrigClientOrderId> orig_client_order_id;
  std::optional<OrderId> order_id;
  Side order_side;
};

struct OrderCancel {
  explicit OrderCancel(protocol::Session session, Side order_side);

  protocol::Session client_session;
  std::optional<ClientOrderId> client_order_id;
  std::optional<OrigClientOrderId> orig_client_order_id;
  std::optional<OrderId> order_id;
  Side order_side;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_UPDATES_HPP_