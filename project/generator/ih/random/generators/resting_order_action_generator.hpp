#ifndef SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_RESTING_ORDER_ACTION_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_RESTING_ORDER_ACTION_GENERATOR_HPP_

#include <utility>

#include "ih/random/generators/value_generator.hpp"
#include "ih/random/values/resting_order_action.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::random {

class RestingOrderActionGenerator {
 public:
  virtual ~RestingOrderActionGenerator() = default;

  template <typename Tracer = trace::NullTracer>
  auto generate_action(Tracer&& tracer = Tracer{}) -> RestingOrderAction;

 private:
  virtual auto generate_integer() -> RestingOrderAction::RandomInteger = 0;
};

template <typename Tracer>
auto RestingOrderActionGenerator::generate_action(Tracer&& tracer)
    -> RestingOrderAction {
  using namespace trace;
  auto step = make_step(tracer, "generating random resting order action");

  const auto random_integer = generate_integer();
  trace_input(step, make_value("randomValue", random_integer));

  RestingOrderAction action{random_integer};
  trace_output(step, make_value("restingOrderAction", action.to_string()));

  trace_step(std::move(step), tracer);
  return action;
}

class RestingOrderActionGeneratorImpl final
    : public random::RestingOrderActionGenerator {
 public:
  explicit RestingOrderActionGeneratorImpl(
      std::shared_ptr<random::ValueGenerator> random_int_generator) noexcept;

  static auto create(
      std::shared_ptr<random::ValueGenerator> random_int_generator)
      -> std::unique_ptr<RestingOrderActionGeneratorImpl>;

 private:
  auto generate_integer() -> RestingOrderAction::RandomInteger override;

  std::shared_ptr<random::ValueGenerator> random_int_generator_;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_RESTING_ORDER_ACTION_GENERATOR_HPP_
