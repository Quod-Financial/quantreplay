#ifndef SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_PRICE_PARAMS_SELECTOR_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_PRICE_PARAMS_SELECTOR_HPP_

#include <string>
#include <utility>

#include "data_layer/api/models/listing.hpp"
#include "ih/random/values/price_generation_params.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::random {

class PriceParamsSelector {
 public:
  explicit PriceParamsSelector(const data_layer::Listing& instrument);

  template <typename Tracer = trace::NullTracer>
  [[nodiscard]]
  auto select(Tracer&& tracer = Tracer{}) const -> const PriceGenerationParams&;

 private:
  static auto trace_selection(trace::NullTracer::Step /*step*/) noexcept
      -> void {}

  auto trace_selection(trace::JsonTracer::Step& step) const -> void;

  std::string symbol_;
  PriceGenerationParams params_;
  bool uses_default_tick_range_;
};

template <typename Tracer>
auto PriceParamsSelector::select(Tracer&& tracer) const
    -> const PriceGenerationParams& {
  auto step = trace::make_step(tracer, "selecting params for price generation");
  trace_selection(step);
  trace_step(std::move(step), std::forward<Tracer>(tracer));
  return params_;
}

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_PRICE_PARAMS_SELECTOR_HPP_
