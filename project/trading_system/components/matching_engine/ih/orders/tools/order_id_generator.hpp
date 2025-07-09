#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_ORDER_ID_GENERATOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_ORDER_ID_GENERATOR_HPP_

#include <memory>

#include "common/attributes.hpp"

namespace simulator::trading_system::matching_engine::order {

class OrderIdGenerator {
 public:
  OrderIdGenerator() = default;
  OrderIdGenerator(const OrderIdGenerator&) = default;
  OrderIdGenerator(OrderIdGenerator&&) = default;
  virtual ~OrderIdGenerator() = default;

  auto operator=(const OrderIdGenerator&) -> OrderIdGenerator& = default;
  auto operator=(OrderIdGenerator&&) -> OrderIdGenerator& = default;

  [[nodiscard]]
  virtual auto operator()() -> OrderId = 0;

  [[nodiscard]]
  static auto create() -> std::unique_ptr<OrderIdGenerator>;
};

[[nodiscard]]
auto generate_order_id(OrderIdGenerator& generator) -> OrderId;

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_ORDER_ID_GENERATOR_HPP_
