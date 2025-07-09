#ifndef SIMULATOR_GENERATOR_IH_TRACING_NULL_TRACER_HPP_
#define SIMULATOR_GENERATOR_IH_TRACING_NULL_TRACER_HPP_

namespace simulator::generator::trace {

class NullTracer {
 public:
  class Step {};

  template <typename... Ts>
  static auto make_step(Ts&&... /*args*/) noexcept -> NullTracer::Step {
    return NullTracer::Step{};
  }
};

}  // namespace simulator::generator::trace

#endif  // SIMULATOR_GENERATOR_IH_TRACING_NULL_TRACER_HPP_
