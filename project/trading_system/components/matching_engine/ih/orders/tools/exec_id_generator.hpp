#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_EXEC_ID_GENERATOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_EXEC_ID_GENERATOR_HPP_

#include <memory>

#include "common/attributes.hpp"

namespace simulator::trading_system::matching_engine::order {

class ExecIdGenerator {
 public:
  ExecIdGenerator() = default;
  ExecIdGenerator(const ExecIdGenerator&) = default;
  ExecIdGenerator(ExecIdGenerator&&) = default;
  virtual ~ExecIdGenerator() = default;

  auto operator=(const ExecIdGenerator&) -> ExecIdGenerator& = default;
  auto operator=(ExecIdGenerator&&) -> ExecIdGenerator& = default;

  [[nodiscard]]
  virtual auto operator()() -> ExecutionId = 0;

  [[nodiscard]]
  static auto create(OrderId target_order_id)
      -> std::unique_ptr<ExecIdGenerator>;
};

[[nodiscard]]
auto generate_execution_id(ExecIdGenerator& generator) -> ExecutionId;

[[nodiscard]]
auto generate_aux_execution_id(OrderId target_order_id) -> ExecutionId;

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_EXEC_ID_GENERATOR_HPP_
