#ifndef SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_COUNTERPARTY_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_COUNTERPARTY_GENERATOR_HPP_

#include <fmt/format.h>

#include <memory>

#include "ih/constants.hpp"
#include "ih/random/generators/value_generator.hpp"
#include "ih/tracing/null_tracer.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::random {

class CounterpartyGenerator {
 public:
  virtual ~CounterpartyGenerator() = default;

  template <typename GenerationTracer = trace::NullTracer>
  auto generate_counterparty(GenerationTracer&& tracer = GenerationTracer{})
      -> PartyId;

 private:
  virtual auto generate_counterparty_number() -> unsigned int = 0;
};

template <typename GenerationTracer>
inline auto CounterpartyGenerator::generate_counterparty(
    GenerationTracer&& tracer) -> PartyId {
  using namespace trace;
  auto step = make_step(tracer, "generating order counterparty");

  const auto counterparty_number = generate_counterparty_number();
  trace_input(step,
              make_value("randomCounterpartyNumber", counterparty_number));

  std::string counterparty_identifier =
      fmt::format("{}{}", constant::CounterpartyIDPrefix, counterparty_number);
  trace_output(step, make_value("partyId", counterparty_identifier));

  trace_step(std::move(step), tracer);
  return PartyId{counterparty_identifier};
}

class CounterpartyGeneratorImpl final : public random::CounterpartyGenerator {
 public:
  explicit CounterpartyGeneratorImpl(
      unsigned int parties_count,
      std::shared_ptr<random::ValueGenerator> random_int_generator) noexcept;

  static auto create(
      unsigned int parties_count,
      std::shared_ptr<random::ValueGenerator> random_int_generator)
      -> std::unique_ptr<CounterpartyGeneratorImpl>;

 private:
  auto generate_counterparty_number() -> std::uint32_t override;

  std::shared_ptr<random::ValueGenerator> random_int_generator_;
  std::uint32_t max_party_number_{0};
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_COUNTERPARTY_GENERATOR_HPP_
