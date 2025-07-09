#ifndef SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_RANDOM_EVENT_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_RANDOM_EVENT_GENERATOR_HPP_

#include <memory>
#include <string_view>
#include <utility>

#include "ih/random/generators/value_generator.hpp"
#include "ih/random/values/event.hpp"
#include "ih/tracing/null_tracer.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::random {

class EventGenerator {
 public:
  virtual ~EventGenerator() = default;

  template <typename GenerationTracer = trace::NullTracer>
  auto generate_event(GenerationTracer&& tracer = GenerationTracer{}) -> Event;

 private:
  virtual auto generate_choice_integer() -> Event::RandomInteger = 0;
};

template <typename GenerationTracer>
auto EventGenerator::generate_event(GenerationTracer&& tracer) -> Event {
  using namespace trace;

  auto step_tracer = make_step(tracer, "generating random event");

  const auto random_int = generate_choice_integer();
  trace_input(step_tracer, make_value("randomValue", random_int));

  Event event{random_int};
  trace_output(step_tracer, make_value("generatedEvent", event.to_string()));

  trace_step(std::move(step_tracer), tracer);
  return event;
}

class EventGeneratorImpl final : public random::EventGenerator {
 public:
  explicit EventGeneratorImpl(
      std::shared_ptr<random::ValueGenerator> random_int_generator) noexcept;

  static auto create(
      std::shared_ptr<random::ValueGenerator> random_int_generator)
      -> std::unique_ptr<EventGeneratorImpl>;

 private:
  auto generate_choice_integer() -> Event::RandomInteger override;

  std::shared_ptr<random::ValueGenerator> random_int_generator_;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_RANDOM_EVENT_GENERATOR_HPP_
