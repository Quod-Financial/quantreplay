#include "ih/orders/tools/exec_id_generator.hpp"

#include <functional>
#include <memory>
#include <stdexcept>

#include "idgen/execution_id.hpp"

namespace simulator::trading_system::matching_engine::order {

namespace {

class IdgenExecIdImplementation : public ExecIdGenerator {
 public:
  IdgenExecIdImplementation() = delete;

  explicit IdgenExecIdImplementation(OrderId target_order_id)
      : context_(idgen::make_execution_id_generation_ctx(target_order_id)) {}

  auto operator()() -> ExecutionId override {
    if (auto identifier = idgen::generate_new_id(context_)) [[likely]] {
      return *identifier;
    }

    // ExecutionId generation may fail only when all possible identifiers are
    // generated already for a target order identifier.
    // Did we generate more than a billion execution identifiers for an order
    // somehow?
    throw std::runtime_error("failed to generate a new execution identifier");
  }

 private:
  idgen::ExecutionIdContext context_;
};

}  // namespace

auto ExecIdGenerator::create(simulator::trading_system::OrderId target_order_id)
    -> std::unique_ptr<ExecIdGenerator> {
  return std::make_unique<IdgenExecIdImplementation>(target_order_id);
}

auto generate_execution_id(ExecIdGenerator& generator) -> ExecutionId {
  return std::invoke(generator);
}

auto generate_aux_execution_id(OrderId target_order_id) -> ExecutionId {
  return generate_execution_id(*ExecIdGenerator::create(target_order_id));
}

}  // namespace simulator::trading_system::matching_engine::order