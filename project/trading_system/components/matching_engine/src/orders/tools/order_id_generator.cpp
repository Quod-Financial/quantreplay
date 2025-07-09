#include "ih/orders/tools/order_id_generator.hpp"

#include <functional>
#include <memory>

#include "idgen/order_id.hpp"

namespace simulator::trading_system::matching_engine::order {

namespace {

class IdgenOrderIdImplementation : public OrderIdGenerator {
 public:
  IdgenOrderIdImplementation()
      : gen_context_(idgen::make_order_id_generation_ctx()) {}

  auto operator()() -> OrderId override {
    return idgen::generate_new_id(gen_context_);
  }

 private:
  idgen::OrderIdContext gen_context_;
};

}  // namespace

auto OrderIdGenerator::create() -> std::unique_ptr<OrderIdGenerator> {
  return std::make_unique<IdgenOrderIdImplementation>();
}

auto generate_order_id(OrderIdGenerator& generator) -> OrderId {
  return std::invoke(generator);
}

}  // namespace simulator::trading_system::matching_engine::order