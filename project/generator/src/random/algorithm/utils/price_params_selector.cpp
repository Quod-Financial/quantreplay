#include "ih/random/algorithm/utils/price_params_selector.hpp"

#include <fmt/format.h>

#include <string_view>
#include <utility>

#include "core/tools/price_tick_ranges.hpp"
#include "data_layer/api/converters/price_tick_ranges.hpp"
#include "ih/constants.hpp"

namespace simulator::generator::random {
namespace {

[[nodiscard]]
auto select_params(const data_layer::Listing& instrument)
    -> PriceGenerationParams {
  PriceGenerationParams::Builder builder;
  builder.set_price_tick_range(instrument.random_tick_range().value_or(
      constant::DefaultListingTickRange));
  builder.set_price_tick_ranges(data_layer::converter::to_price_tick_ranges(
      instrument.price_tick_ranges()));
  if (const auto price_spread = instrument.random_price_spread()) {
    builder.set_price_spread(*price_spread);
  }
  return PriceGenerationParams{std::move(builder)};
}

}  // namespace

PriceParamsSelector::PriceParamsSelector(const data_layer::Listing& instrument)
    : symbol_{instrument.symbol().value_or("none")},
      params_{select_params(instrument)},
      uses_default_tick_range_{!instrument.random_tick_range().has_value()} {}

auto PriceParamsSelector::trace_selection(trace::JsonTracer::Step& step) const
    -> void {
  trace_input(step, trace::make_value("instrumentSymbol", symbol_));

  const auto tick_range = params_.get_price_tick_range();
  if (uses_default_tick_range_) {
    trace_output(
        step,
        trace::make_commented_value(
            "randomTickRange",
            tick_range,
            "instrument's randomTickRange is null, set to the default value"));
  } else {
    trace_output(step, trace::make_value("randomTickRange", tick_range));
  }

  const auto& tick_ranges = params_.get_price_tick_ranges();
  const auto traced_ranges = fmt::format("{}", tick_ranges);
  if (tick_ranges.ranges().empty()) {
    trace_output(step,
                 trace::make_commented_value(
                     "priceTickRanges",
                     traced_ranges,
                     "instrument has no priceTickRanges, the default price "
                     "tick size is used"));
  } else {
    trace_output(step, trace::make_value("priceTickRanges", traced_ranges));
  }

  if (const auto price_spread = params_.get_price_spread()) {
    trace_output(step, trace::make_value("randomPriceSpread", *price_spread));
  } else {
    trace_output(step,
                 trace::make_commented_value(
                     "randomPriceSpread",
                     std::string_view{"none"},
                     "instrument's randomPriceSpread is null, the price tick "
                     "size is the minimal spread"));
  }
}

}  // namespace simulator::generator::random
