#ifndef SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_ATTRIBUTES_SETTER_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_ATTRIBUTES_SETTER_HPP_

#include <fmt/format.h>

#include "ih/adaptation/generated_message.hpp"
#include "ih/tracing/null_tracer.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::random {

class AttributesSetter {
 public:
  template <typename Tracer = trace::NullTracer>
  static auto set(GeneratedMessage& message,
                  MessageType message_type,
                  Tracer&& tracer = Tracer{}) -> void;

  template <typename Tracer = trace::NullTracer>
  static auto set(GeneratedMessage& message,
                  OrderType order_type,
                  Tracer&& tracer = Tracer{}) -> void;

  template <typename Tracer = trace::NullTracer>
  static auto set(GeneratedMessage& message,
                  Side order_side,
                  Tracer&& tracer = Tracer{}) -> void;

  template <typename Tracer = trace::NullTracer>
  static auto set(GeneratedMessage& message,
                  TimeInForce time_in_force,
                  Tracer&& tracer = Tracer{}) -> void;
};

template <typename Tracer>
inline auto AttributesSetter::set(GeneratedMessage& message,
                                  MessageType message_type,
                                  Tracer&& tracer) -> void {
  using namespace trace;
  message.message_type = message_type;

  auto step = make_step(tracer, "specifying MessageType");
  trace_output(
      step,
      make_value("message_type", fmt::format("{}", message.message_type)));
  trace_step(std::move(step), tracer);
}

template <typename Tracer>
inline auto AttributesSetter::set(GeneratedMessage& message,
                                  OrderType order_type,
                                  Tracer&& tracer) -> void {
  using namespace trace;
  message.order_type = order_type;

  auto step = make_step(tracer, "specifying OrderType");
  trace_output(step,
               make_value("order_type", fmt::format("{}", message.order_type)));
  trace_step(std::move(step), tracer);
}

template <typename Tracer>
auto AttributesSetter::set(GeneratedMessage& message,
                           Side order_side,
                           Tracer&& tracer) -> void {
  using namespace trace;
  message.side = order_side;

  auto step = make_step(tracer, "specifying Side");
  trace_output(step, make_value("side", fmt::format("{}", message.side)));
  trace_step(std::move(step), tracer);
}

template <typename Tracer>
inline auto AttributesSetter::set(GeneratedMessage& message,
                                  TimeInForce time_in_force,
                                  Tracer&& tracer) -> void {
  using namespace trace;
  message.time_in_force = time_in_force;

  auto step = make_step(tracer, "specifying TimeInForce");
  trace_output(
      step,
      make_value("time_in_force", fmt::format("{}", message.time_in_force)));
  trace_step(std::move(step), tracer);
}

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_ATTRIBUTES_SETTER_HPP_
