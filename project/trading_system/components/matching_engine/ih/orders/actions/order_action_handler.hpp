#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_ORDER_ACTION_LISTENER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_ORDER_ACTION_LISTENER_HPP_

#include <vector>

#include "common/attributes.hpp"
#include "common/market_state/snapshot.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_updates.hpp"

namespace simulator::trading_system::matching_engine {

class OrderActionHandler {
 public:
  OrderActionHandler() = default;
  OrderActionHandler(const OrderActionHandler&) = default;
  OrderActionHandler(OrderActionHandler&&) = default;
  virtual ~OrderActionHandler() = default;

  auto operator=(const OrderActionHandler&) -> OrderActionHandler& = default;
  auto operator=(OrderActionHandler&&) -> OrderActionHandler& = default;

  virtual auto place_limit_order(LimitOrder order) -> void = 0;

  virtual auto place_market_order(MarketOrder order) -> void = 0;

  virtual auto amend_limit_order(LimitUpdate update) -> void = 0;

  virtual auto cancel_order(const OrderCancel& cancel) -> void = 0;

  virtual auto recover_order(market_state::LimitOrder order_state) -> void = 0;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_ORDER_ACTION_LISTENER_HPP_