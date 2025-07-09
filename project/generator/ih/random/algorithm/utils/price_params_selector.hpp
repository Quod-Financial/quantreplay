#ifndef SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_PRICE_PARAMS_SELECTOR_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_PRICE_PARAMS_SELECTOR_HPP_

#include "data_layer/api/models/listing.hpp"
#include "ih/constants.hpp"
#include "ih/random/values/price_generation_params.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::random {

class PriceParamsSelector {
 public:
  template <typename Tracer = trace::NullTracer>
  static auto select(const data_layer::Listing& instrument,
                     Tracer&& tracer = Tracer{}) -> PriceGenerationParams;
};

template <typename Tracer>
auto PriceParamsSelector::select(const data_layer::Listing& instrument,
                                 Tracer&& tracer) -> PriceGenerationParams {
  using namespace trace;

  PriceGenerationParams::Builder params_builder{};
  auto step = make_step(tracer, "selecting params for price generation");

  const auto& instrument_symbol = instrument.symbol();
  trace_input(
      step, make_value("instrumentSymbol", instrument_symbol.value_or("none")));

  const auto px_tick_range = instrument.random_tick_range().value_or(
      constant::DefaultListingTickRange);
  params_builder.set_price_tick_range(px_tick_range);
  trace_output(step, make_value("randomTickRange", px_tick_range));

  const auto px_tick_size = instrument.price_tick_size().value_or(
      constant::DefaultListingPriceTickSize);
  params_builder.set_price_tick_size(px_tick_size);
  trace_output(step, make_value("priceTickSize", px_tick_size));

  const auto px_spread =
      instrument.random_orders_spread().value_or(px_tick_size);
  params_builder.set_price_spread(px_spread);

  trace_output(step, make_value("priceSpread", px_spread));
  return PriceGenerationParams{params_builder};
}

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_PRICE_PARAMS_SELECTOR_HPP_
