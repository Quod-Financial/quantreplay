#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_CANCELLATION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_CANCELLATION_HPP_

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_updates.hpp"

namespace simulator::trading_system::matching_engine {

class Cancellation : private EventReporter {
 public:
  Cancellation(EventListener& event_listener, OrderBook& order_book);

  Cancellation(const Cancellation&) = default;
  Cancellation(Cancellation&&) = default;
  ~Cancellation() override = default;

  auto operator=(const Cancellation&) -> Cancellation& = delete;
  auto operator=(Cancellation&&) -> Cancellation& = delete;

  auto operator()(const OrderCancel& cancel) -> void;

 private:
  auto cancel_order(const OrderCancel& cancel, OrderPage& page) -> void;

  OrderBook& order_book_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_CANCELLATION_HPP_
