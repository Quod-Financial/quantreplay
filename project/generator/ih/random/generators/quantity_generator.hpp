#ifndef SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_QUANTITY_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_QUANTITY_GENERATOR_HPP_

#include <memory>

#include "ih/random/generators/value_generator.hpp"
#include "ih/random/values/quantity_generation_params.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::random {

class QuantityGenerator {
 protected:
  struct GenerationOutput {
    double generated_quantity{0.};
    int random_number{0};
  };

 public:
  virtual ~QuantityGenerator() = default;

  template <typename Tracer = trace::NullTracer>
  auto generate_qty(const QuantityGenerationParams& params,
                    Tracer&& tracer = Tracer{}) -> double;

 private:
  virtual auto generate_random_qty(const QuantityGenerationParams& params)
      -> GenerationOutput = 0;
};

template <typename Tracer>
auto QuantityGenerator::generate_qty(const QuantityGenerationParams& params,
                                     Tracer&& tracer) -> double {
  using namespace trace;

  auto step = make_step(tracer, "generating random quantity");

  trace_input(step, make_value("quantityMultiplier", params.get_multiplier()));
  trace_input(step,
              make_value("minimalQuantity", params.get_minimal_quantity()));
  trace_input(step,
              make_value("maximalQuantity", params.get_maximal_quantity()));

  auto generation_out = generate_random_qty(params);
  trace_output(step, make_value("randomValue", generation_out.random_number));
  trace_output(step,
               make_value("generatedQty", generation_out.generated_quantity));

  trace_step(std::move(step), tracer);
  return generation_out.generated_quantity;
}

class QuantityGeneratorImpl final : public random::QuantityGenerator {
 public:
  QuantityGeneratorImpl() = delete;

  explicit QuantityGeneratorImpl(
      std::shared_ptr<random::ValueGenerator> random_int_generator) noexcept;

  static auto create(
      std::shared_ptr<random::ValueGenerator> random_int_generator)
      -> std::unique_ptr<QuantityGeneratorImpl>;

 private:
  auto generate_random_qty(const QuantityGenerationParams& params)
      -> GenerationOutput override;

  std::shared_ptr<random::ValueGenerator> random_int_generator_;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_QUANTITY_GENERATOR_HPP_
