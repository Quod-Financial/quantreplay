#ifndef SIMULATOR_GENERATOR_IH_TRACING_TRACE_LOGGER_HPP_
#define SIMULATOR_GENERATOR_IH_TRACING_TRACE_LOGGER_HPP_

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <functional>
#include <type_traits>

#include "ih/tracing/json_tracer.hpp"
#include "ih/tracing/null_tracer.hpp"
#include "log/logging.hpp"

namespace simulator::generator::trace {

template <typename Tracer>
class FormattingHelper {
  using TracerType = std::decay_t<Tracer>;

 public:
  explicit FormattingHelper(Tracer& tracer) : tracer_reference_{tracer} {}

  [[nodiscard]] auto get_trace_string() const -> std::string {
    return tracer_reference_.get().compose_trace();
  }

 private:
  mutable std::reference_wrapper<TracerType> tracer_reference_;
};

}  // namespace simulator::generator::trace

namespace fmt {

template <typename Tracer>
struct [[maybe_unused]] formatter<
    simulator::generator::trace::FormattingHelper<Tracer>> {
  using Type = simulator::generator::trace::FormattingHelper<Tracer>;

  static constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormattingContext>
  auto format(const Type& formatting_helper,
              FormattingContext&& context) const {
    return format_to(context.out(), "{}", formatting_helper.get_trace_string());
  }
};

}  // namespace fmt

namespace simulator::generator::trace {

class [[maybe_unused]] TraceLogger {
 public:
  static auto log(NullTracer&& /*tracer*/) -> void {}

  static auto log(JsonTracer&& tracer) -> void {
    log::debug("{}", FormattingHelper<JsonTracer>{tracer});
  }
};

class [[maybe_unused]] TraceFormatter {
 public:
  static auto format(NullTracer&& /*tracer*/) -> std::string { return ""; }

  static auto format(JsonTracer&& tracer) -> std::string {
    return fmt::format("{}", FormattingHelper<JsonTracer>{tracer});
  }
};

}  // namespace simulator::generator::trace

#endif  // SIMULATOR_GENERATOR_IH_TRACING_TRACE_LOGGER_HPP_
