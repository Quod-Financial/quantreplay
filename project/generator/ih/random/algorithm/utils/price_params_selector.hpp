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

 private:
  template <typename StepTracer>
  static auto select_tick_range(const data_layer::Listing& instrument,
                                StepTracer& step) -> std::uint32_t;

  template <typename StepTracer>
  static auto select_tick_size(const data_layer::Listing& instrument,
                               StepTracer& step) -> double;

  template <typename StepTracer>
  static auto select_spread(const data_layer::Listing& instrument,
                            double px_tick_size,
                            StepTracer& step) -> double;
};

template <typename Tracer>
auto PriceParamsSelector::select(const data_layer::Listing& instrument,
                                 Tracer&& tracer) -> PriceGenerationParams {
  PriceGenerationParams::Builder params_builder;
  auto step = trace::make_step(tracer, "selecting params for price generation");

  const auto& instrument_symbol = instrument.symbol();
  trace_input(step,
              trace::make_value("instrumentSymbol",
                                instrument_symbol.value_or("none")));

  params_builder.set_price_tick_range(select_tick_range(instrument, step));

  const auto px_tick_size = select_tick_size(instrument, step);
  params_builder.set_price_tick_size(px_tick_size);

  params_builder.set_price_spread(
      select_spread(instrument, px_tick_size, step));

  trace_step(std::move(step), tracer);
  return PriceGenerationParams{params_builder};
}

template <typename StepTracer>
auto PriceParamsSelector::select_tick_range(
    const data_layer::Listing& instrument, StepTracer& step) -> std::uint32_t {
  std::uint32_t px_tick_range = 0;
  if (instrument.random_tick_range()) {
    px_tick_range = *instrument.random_tick_range();
    trace_output(step, trace::make_value("randomTickRange", px_tick_range));
  } else {
    px_tick_range = constant::DefaultListingTickRange;
    trace_output(
        step,
        trace::make_commented_value(
            "randomTickRange",
            px_tick_range,
            "instrument's randomTickRange is null, set to the default value"));
  }

  return px_tick_range;
}

template <typename StepTracer>
auto PriceParamsSelector::select_tick_size(
    const data_layer::Listing& instrument, StepTracer& step) -> double {
  double px_tick_size = 0;
  if (instrument.price_tick_size()) {
    px_tick_size = *instrument.price_tick_size();
    trace_output(step, trace::make_value("priceTickSize", px_tick_size));
  } else {
    px_tick_size = constant::DefaultListingPriceTickSize;
    trace_output(
        step,
        trace::make_commented_value(
            "priceTickSize",
            px_tick_size,
            "instrument's priceTickSize is null, set to the default value"));
  }

  return px_tick_size;
}

template <typename StepTracer>
auto PriceParamsSelector::select_spread(const data_layer::Listing& instrument,
                                        double px_tick_size,
                                        StepTracer& step) -> double {
  double px_spread = 0;
  if (instrument.random_orders_spread()) {
    px_spread = *instrument.random_orders_spread();
    if (px_spread < px_tick_size) {
      px_spread = px_tick_size;
      trace_output(step,
                   trace::make_commented_value(
                       "randomOrdersSpread",
                       px_spread,
                       "instrument's randomOrdersSpread is less than "
                       "priceTickSize, set to priceTickSize"));
    } else {
      trace_output(step, trace::make_value("randomOrdersSpread", px_spread));
    }
  } else {
    px_spread = px_tick_size;
    trace_output(
        step,
        trace::make_commented_value(
            "randomOrdersSpread",
            px_spread,
            "instrument's randomOrdersSpread is null, set to priceTickSize"));
  }

  return px_spread;
}

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_PRICE_PARAMS_SELECTOR_HPP_
