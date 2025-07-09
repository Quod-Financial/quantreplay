#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_REGULAR_ORDER_ACTION_PROCESSOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_REGULAR_ORDER_ACTION_PROCESSOR_HPP_

#include <memory>

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/orders/actions/order_action_handler.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_book.hpp"

namespace simulator::trading_system::matching_engine {

class RegularOrderActionProcessor : public OrderActionHandler {
 public:
  explicit RegularOrderActionProcessor(EventListener& event_listener,
                                       OrderBook& order_book);

  auto place_limit_order(LimitOrder order) -> void override;

  auto place_market_order(MarketOrder order) -> void override;

  auto amend_limit_order(LimitUpdate update) -> void override;

  auto cancel_order(const OrderCancel& cancel) -> void override;

  auto recover_order(market_state::LimitOrder order_state) -> void override;

 private:
  EventListener& event_listener_;
  OrderBook& order_book_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_REGULAR_ORDER_ACTION_PROCESSOR_HPP_