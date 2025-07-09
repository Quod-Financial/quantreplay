#ifndef SIMULATOR_GENERATOR_IH_TRACING_TRACING_HPP_
#define SIMULATOR_GENERATOR_IH_TRACING_TRACING_HPP_

#include <type_traits>
#include <utility>
#include <variant>

#include "cfg/api/cfg.hpp"
#include "ih/tracing/json_tracer.hpp"
#include "ih/tracing/null_tracer.hpp"
#include "ih/tracing/trace_logger.hpp"
#include "ih/tracing/trace_value.hpp"

namespace simulator::generator::trace {
namespace detail {

template <typename Algorithm>
class TracingWrapperBase {
 public:
  virtual ~TracingWrapperBase() = default;

  TracingWrapperBase() = delete;

  explicit TracingWrapperBase(Algorithm&& algorithm)
      : wrapped_algorithm_{std::forward<Algorithm>(algorithm)} {}

  Algorithm& algorithm() { return wrapped_algorithm_; }

 private:
  Algorithm wrapped_algorithm_;
};

template <typename Algorithm, bool AlgorithmReturnsValue>
class TracingWrapper;

template <typename Algorithm>
class TracingWrapper<Algorithm, true> : TracingWrapperBase<Algorithm> {
  using Base = TracingWrapperBase<Algorithm>;

 public:
  TracingWrapper() = delete;

  explicit TracingWrapper(Algorithm&& algorithm)
      : Base(std::forward<Algorithm>(algorithm)) {}

  template <typename Tracer>
  auto operator()(Tracer& tracer) {
    auto result = Base::algorithm()(tracer);
    trace::TraceLogger::log(std::move(tracer));

    return result;
  }
};

template <typename Algorithm>
class TracingWrapper<Algorithm, false> : TracingWrapperBase<Algorithm> {
  using Base = TracingWrapperBase<Algorithm>;

 public:
  TracingWrapper() = delete;

  explicit TracingWrapper(Algorithm&& algorithm)
      : Base(std::forward<Algorithm>(algorithm)) {}

  template <typename Tracer>
  auto operator()(Tracer& tracer) -> void {
    Base::algorithm()(tracer);
    trace::TraceLogger::log(std::move(tracer));
  }
};

}  // namespace detail

template <typename Algorithm>
[[maybe_unused]]
auto trace(Algorithm&& algorithm) {
  using DefaultTracer = NullTracer;
  using ReturnType =
      std::invoke_result_t<Algorithm,
                           std::add_lvalue_reference_t<DefaultTracer>>;

  constexpr bool is_noreturn = std::is_void_v<ReturnType>;
  constexpr bool is_returning =
      !is_noreturn && !std::is_reference_v<ReturnType>;
  static_assert(
      is_noreturn ^ is_returning,
      "Unsupported 'Algorithm' type. An instance of an 'Algorithm' type "
      "has to have an 'operator()' defined with an single 'Tracer &' "
      "argument (where 'Tracer' is a template parameter) and must return "
      "a non-reference type or must not return anything.");

  std::variant<DefaultTracer, JsonTracer> tracer{DefaultTracer{}};
  if (simulator::cfg::generator().enable_tracing) {
    tracer = JsonTracer{};
  }

  using TracingWrapper = detail::TracingWrapper<Algorithm, is_returning>;
  return std::visit(TracingWrapper{std::forward<Algorithm>(algorithm)}, tracer);
}

[[maybe_unused]]
inline auto make_step(NullTracer /*tracer*/,
                      std::string_view /*action*/) noexcept {
  return NullTracer::make_step();
}

[[maybe_unused]] inline auto make_step(JsonTracer& tracer,
                                       std::string_view action)
    -> JsonTracer::Step {
  return tracer.make_step(action);
}

[[maybe_unused]] inline auto trace_step(NullTracer::Step /*step*/,
                                        NullTracer /*tracer*/) noexcept
    -> void {}

[[maybe_unused]] inline auto trace_step(JsonTracer::Step&& step,
                                        JsonTracer& tracer) -> void {
  tracer.trace(std::move(step));
}

template <typename T>
[[maybe_unused]] inline auto trace_input(NullTracer::Step /*step*/,
                                         const TraceValue<T>& /*value*/)
    -> void {}

template <typename T>
[[maybe_unused]] inline auto trace_input(JsonTracer::Step& step,
                                         const TraceValue<T>& value) -> void {
  step.trace_input(value);
}

template <typename T>
[[maybe_unused]] inline auto trace_output(NullTracer::Step /*step*/,
                                          const TraceValue<T>& /*value*/)
    -> void {}

template <typename T>
[[maybe_unused]] inline auto trace_output(JsonTracer::Step& step,
                                          const TraceValue<T>& value) -> void {
  step.trace_output(value);
}

template <typename T>
[[maybe_unused]] auto make_value(std::string_view value_name, T&& value) {
  return TraceValue<T>{value_name, std::forward<T>(value)};
}

template <typename T>
[[maybe_unused]]
auto make_commented_value(std::string_view value_name,
                          T&& value,
                          std::string_view comment) {
  auto val = make_value(value_name, std::forward<T>(value));
  val.set_comment(comment);
  return val;
}

}  // namespace simulator::generator::trace

#endif  // SIMULATOR_GENERATOR_IH_TRACING_TRACING_HPP_
