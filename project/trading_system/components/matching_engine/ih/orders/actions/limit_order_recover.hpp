#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_LIMIT_ORDER_RECOVER_HPP
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_LIMIT_ORDER_RECOVER_HPP

#include "common/market_state/snapshot.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/order_book.hpp"

namespace simulator::trading_system::matching_engine {

class LimitOrderRecover : private EventReporter {
 public:
  LimitOrderRecover(EventListener& event_listener, OrderBook& order_book);

  LimitOrderRecover(const LimitOrderRecover&) = delete;
  ~LimitOrderRecover() override = default;

  auto operator=(const LimitOrderRecover&) -> LimitOrderRecover& = delete;

  auto operator()(market_state::LimitOrder order_state) -> void;

 private:
  OrderBook& order_book_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_LIMIT_ORDER_RECOVER_HPP
