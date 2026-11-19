#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_ORDER_MODIFYING_ENGINE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_ORDER_MODIFYING_ENGINE_HPP_

#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"

namespace simulator::trading_system {

class OrderModifyingEngine {
 public:
  OrderModifyingEngine() = default;
  OrderModifyingEngine(const OrderModifyingEngine&) = delete;
  OrderModifyingEngine(OrderModifyingEngine&&) = delete;
  virtual ~OrderModifyingEngine() = default;

  auto operator=(const OrderModifyingEngine&) -> OrderModifyingEngine& = delete;
  auto operator=(OrderModifyingEngine&&) -> OrderModifyingEngine& = delete;

  virtual auto execute(protocol::OrderModificationRequest request) -> void = 0;

  virtual auto execute(protocol::OrderCancellationRequest request) -> void = 0;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_ORDER_MODIFYING_ENGINE_HPP_
