#include "ih/random/algorithm/utils/quantity_params_selector.hpp"

#include "core/tools/numeric.hpp"
#include "ih/constants.hpp"

namespace simulator::generator::random {

QuantityParamsSelector::QuantityParamsSelector(
    const data_layer::Listing& instrument) noexcept
    : symbol_{instrument.symbol().value_or("none")},
      qty_multiplier_{instrument.qty_multiple().value_or(
          constant::DefaultListingQtyMultiple)},
      instrument_qty_min_{instrument.qty_minimum().value_or(
          constant::DefaultListingQtyMinimum)},
      instrument_qty_max_{instrument.qty_maximum().value_or(
          constant::DefaultListingQtyMaximum)},
      at_least_one_random_aggressive_min_parameter_present_{
          instrument.random_aggressive_qty_minimum().has_value() ||
          instrument.random_aggressive_amt_minimum().has_value()},
      at_least_one_random_aggressive_max_parameter_present_{
          instrument.random_aggressive_qty_maximum().has_value() ||
          instrument.random_aggressive_amt_maximum().has_value()},
      aggressive_amt_min_{instrument.random_aggressive_amt_minimum()},
      aggressive_amt_max_{instrument.random_aggressive_amt_maximum()},
      resting_amt_minimum_{instrument.random_amt_minimum()},
      resting_amt_maximum_{instrument.random_amt_maximum()} {
  init_order_qty_min(resting_qty_min_, instrument.random_qty_minimum());
  init_order_qty_min(aggressive_qty_min_,
                     instrument.random_aggressive_qty_minimum());
  init_order_qty_max(resting_qty_max_, instrument.random_qty_maximum());
  init_order_qty_max(aggressive_qty_max_,
                     instrument.random_aggressive_qty_maximum());
}

auto QuantityParamsSelector::init_order_qty_min(
    Quantity& order_qty_min, const std::optional<double>& random_qty_min)
    -> void {
  if (random_qty_min.has_value() && *random_qty_min >= instrument_qty_min_) {
    order_qty_min = Quantity{.value = *random_qty_min,
                             .source = Quantity::Source::Random_Qty};
  } else {
    order_qty_min = Quantity{.value = instrument_qty_min_,
                             .source = Quantity::Source::Instrument_Qty};
  }
}

auto QuantityParamsSelector::init_order_qty_max(
    Quantity& order_qty_max, const std::optional<double>& random_qty_max)
    -> void {
  if (random_qty_max.has_value() && *random_qty_max <= instrument_qty_max_) {
    order_qty_max = Quantity{.value = *random_qty_max,
                             .source = Quantity::Source::Random_Qty};
  } else {
    order_qty_max = Quantity{.value = instrument_qty_max_,
                             .source = Quantity::Source::Instrument_Qty};
  }
}

auto QuantityParamsSelector::is_zero(double value) noexcept -> bool {
  return core::equal(value, 0.);
}

}  // namespace simulator::generator::random