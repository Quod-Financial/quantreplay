#ifndef SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_MAX_MKTDEPTH_SELECTOR_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_MAX_MKTDEPTH_SELECTOR_HPP_

#include <cstdint>
#include <optional>

#include "data_layer/api/models/listing.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::random {

class MaxMktDepthSelector {
 public:
  template <typename Tracer = trace::NullTracer>
  static auto select(const data_layer::Listing& instrument,
                     std::uint32_t parties_count,
                     Tracer&& tracer = Tracer{})
      -> std::optional<std::uint32_t>;
};

template <typename Tracer>
auto MaxMktDepthSelector::select(const data_layer::Listing& instrument,
                                 std::uint32_t parties_count,
                                 Tracer&& tracer)
    -> std::optional<std::uint32_t> {
  using namespace trace;
  auto step = make_step(tracer, "selecting maximal instrument depth value");

  const auto& instrument_symbol = instrument.symbol().value_or(std::string{});
  trace_input(step, make_value("instrumentSymbol", instrument_symbol));
  trace_input(step, make_value("maxPartiesCount", parties_count));

  std::optional<std::uint32_t> opt_max_levels;
  if (const auto instrument_levels = instrument.random_depth_levels()) {
    trace_input(step, make_value("randomDepthLevels", *instrument_levels));

    if (*instrument_levels <= parties_count) {
      opt_max_levels = *instrument_levels;
      trace_output(
          step,
          make_commented_value(
              "maxDepthLevels",
              *opt_max_levels,
              "randomDepthLevels is used as maximal market depth value"));
    } else {
      opt_max_levels = parties_count;
      trace_output(step,
                   make_commented_value(
                       "maxDepthLevels",
                       *opt_max_levels,
                       "a random parties count is used as max depth value "
                       "(maxInstrumentDepth > randomPartiesCount)"));
    }
  } else {
    trace_input(step,
                make_value("randomDepthLevels", std::string_view{"none"}));

    trace_output(
        step,
        make_commented_value(
            "maxDepthLevels",
            std::string_view{"none"},
            "this instrument does not have max depth levels configured"));
  }

  trace_step(std::move(step), tracer);
  return opt_max_levels;
}

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_MAX_MKTDEPTH_SELECTOR_HPP_
