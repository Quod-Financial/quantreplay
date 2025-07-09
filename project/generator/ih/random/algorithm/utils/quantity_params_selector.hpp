#ifndef SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_QUANTITY_PARAMS_SELECTOR_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_QUANTITY_PARAMS_SELECTOR_HPP_

#include <string_view>

#include "data_layer/api/models/listing.hpp"
#include "ih/random/values/event.hpp"
#include "ih/random/values/quantity_generation_params.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::random {

class QuantityParamsSelector {
 public:
  explicit QuantityParamsSelector(
      const data_layer::Listing& instrument) noexcept;

  QuantityParamsSelector(const QuantityParamsSelector&) = delete;
  auto operator=(const QuantityParamsSelector&)
      -> QuantityParamsSelector& = delete;
  QuantityParamsSelector(QuantityParamsSelector&&) = delete;
  auto operator=(QuantityParamsSelector&&) -> QuantityParamsSelector& = delete;

  template <typename Tracer = trace::NullTracer>
  auto select(double price, Event event, Tracer&& tracer = Tracer{})
      -> QuantityGenerationParams;

 private:
  struct Quantity {
    enum class Source : std::uint8_t { Instrument_Qty, Random_Qty };

    double value;
    Source source;
  };

  auto init_order_qty_min(Quantity& order_qty_min,
                          const std::optional<double>& random_qty_min) -> void;
  auto init_order_qty_max(Quantity& order_qty_max,
                          const std::optional<double>& random_qty_max) -> void;

  template <typename StepTracer>
  auto select_qty_multiplier(StepTracer&& step_tracer) -> double;

  template <typename StepTracer>
  auto select_min_qty(double price, Event event, StepTracer&& step_tracer)
      -> double;

  template <typename StepTracer>
  auto select_resting_min_qty(double price, StepTracer&& step_tracer) -> double;

  template <typename StepTracer>
  auto select_aggressive_min_qty(double price, StepTracer&& step_tracer)
      -> double;

  template <typename StepTracer>
  auto select_max_qty(double price, Event event, StepTracer&& step_tracer)
      -> double;

  template <typename StepTracer>
  auto select_resting_max_qty(double price, StepTracer&& step_tracer) -> double;

  template <typename StepTracer>
  auto select_aggressive_max_qty(double price, StepTracer&& step_tracer)
      -> double;

  static auto is_zero(double value) noexcept -> bool;

  std::string symbol_;
  const double qty_multiplier_;
  const double instrument_qty_min_;
  const double instrument_qty_max_;

  bool at_least_one_random_aggressive_min_parameter_present_;
  bool at_least_one_random_aggressive_max_parameter_present_;
  Quantity aggressive_qty_min_;
  Quantity aggressive_qty_max_;
  const std::optional<double> aggressive_amt_min_;
  const std::optional<double> aggressive_amt_max_;

  Quantity resting_qty_min_;
  Quantity resting_qty_max_;
  const std::optional<double> resting_amt_minimum_;
  const std::optional<double> resting_amt_maximum_;
};

template <typename Tracer>
auto QuantityParamsSelector::select(double price, Event event, Tracer&& tracer)
    -> QuantityGenerationParams {
  using namespace trace;

  QuantityGenerationParams::Builder params_builder;
  auto step = make_step(tracer, "selecting params for quantity generation");
  trace_input(step, make_value("instrument_symbol", symbol_));

  params_builder.set_quantity_multiplier(select_qty_multiplier(step));
  params_builder.set_minimal_quantity(select_min_qty(price, event, step));
  params_builder.set_maximal_quantity(select_max_qty(price, event, step));

  trace_step(std::move(step), tracer);
  return QuantityGenerationParams{params_builder};
}

template <typename StepTracer>
auto QuantityParamsSelector::select_qty_multiplier(StepTracer&& step_tracer)
    -> double {
  trace::trace_output(
      step_tracer,
      trace::make_commented_value(
          "qty_multiple",
          qty_multiplier_,
          "quantity has been selected as instrument minimal quantity"));
  return qty_multiplier_;
}

template <typename StepTracer>
auto QuantityParamsSelector::select_min_qty(double price,
                                            Event event,
                                            StepTracer&& step_tracer)
    -> double {
  if (event.is_aggressive_order_event() &&
      at_least_one_random_aggressive_min_parameter_present_) {
    return select_aggressive_min_qty(price, step_tracer);
  }
  return select_resting_min_qty(price, step_tracer);
}

template <typename StepTracer>
auto QuantityParamsSelector::select_aggressive_min_qty(double price,
                                                       StepTracer&& step_tracer)
    -> double {
  using namespace trace;

  const bool selected_random_qty =
      aggressive_qty_min_.source == Quantity::Source::Random_Qty;
  double min_qty = aggressive_qty_min_.value;

  bool selected_random_amount = false;
  if (aggressive_amt_min_.has_value() && !is_zero(price)) {
    const double min_amt_qty = *aggressive_amt_min_ / price;
    selected_random_amount = min_amt_qty >= min_qty;
    min_qty = selected_random_amount ? min_amt_qty : min_qty;
  }

  std::string_view trace_comment{
      "quantity has been selected as instrument minimal quantity"};
  if (selected_random_amount) {
    trace_comment = std::string_view{
        "quantity has been selected "
        "based on RandomAggressiveAmountMinimum value"};
  } else if (selected_random_qty) {
    trace_comment = std::string_view{
        "quantity has been selected "
        "based on RandomAggressiveQtyMinimum value"};
  }

  trace_output(step_tracer,
               make_commented_value("min_qty", min_qty, trace_comment));

  return min_qty;
}

template <typename StepTracer>
auto QuantityParamsSelector::select_resting_min_qty(double price,
                                                    StepTracer&& step_tracer)
    -> double {
  using namespace trace;

  trace_input(step_tracer,
              make_value("instrument_qty_min", instrument_qty_min_));

  double min_qty = resting_qty_min_.value;

  bool selected_random_amount = false;
  if (resting_amt_minimum_ && !is_zero(price)) {
    const double min_amt_qty = *resting_amt_minimum_ / price;
    selected_random_amount = min_amt_qty >= min_qty;
    min_qty = selected_random_amount ? min_amt_qty : min_qty;
  }

  std::string_view trace_comment{
      "quantity has been selected as instrument minimal quantity"};
  if (selected_random_amount) {
    // This has to be checked firstly as
    // min amount qty can override min random qty
    trace_comment = std::string_view{
        "quantity has been selected "
        "based on RandomOrdersAmountMinimum value"};
  } else if (resting_qty_min_.source == Quantity::Source::Random_Qty) {
    trace_comment = std::string_view{
        "quantity has been selected "
        "based on RandomQtyMinimum value"};
  }

  trace_output(step_tracer,
               make_commented_value("min_qty", min_qty, trace_comment));
  return min_qty;
}

template <typename StepTracer>
auto QuantityParamsSelector::select_max_qty(double price,
                                            Event event,
                                            StepTracer&& step_tracer)
    -> double {
  if (event.is_aggressive_order_event() &&
      at_least_one_random_aggressive_max_parameter_present_) {
    return select_aggressive_max_qty(price, step_tracer);
  }
  return select_resting_max_qty(price, step_tracer);
}

template <typename StepTracer>
auto QuantityParamsSelector::select_aggressive_max_qty(double price,
                                                       StepTracer&& step_tracer)
    -> double {
  using namespace trace;

  const bool selected_random_qty =
      aggressive_qty_max_.source == Quantity::Source::Random_Qty;
  double max_qty = aggressive_qty_max_.value;

  bool selected_random_amount = false;
  if (aggressive_amt_max_.has_value() && !is_zero(price)) {
    const double max_amt_qty = *aggressive_amt_max_ / price;
    selected_random_amount = max_amt_qty <= max_qty;
    max_qty = selected_random_amount ? max_amt_qty : max_qty;
  }

  std::string_view trace_comment{
      "quantity has been selected as instrument maximal quantity"};
  if (selected_random_amount) {
    trace_comment = std::string_view{
        "quantity has been selected "
        "based on RandomAggressiveAmountMaximum value"};
  } else if (selected_random_qty) {
    trace_comment = std::string_view{
        "quantity has been selected "
        "based on RandomAggressiveQtyMaximum value"};
  }

  trace_output(step_tracer,
               make_commented_value("max_qty", max_qty, trace_comment));

  return max_qty;
}

template <typename StepTracer>
auto QuantityParamsSelector::select_resting_max_qty(double price,
                                                    StepTracer&& step_tracer)
    -> double {
  using namespace trace;

  trace_input(step_tracer,
              make_value("instrument_qty_max", instrument_qty_max_));

  double max_qty = resting_qty_max_.value;

  bool selected_random_amount = false;
  if (resting_amt_maximum_ && !is_zero(price)) {
    const double max_amt_qty = *resting_amt_maximum_ / price;
    selected_random_amount = max_amt_qty <= max_qty;
    max_qty = selected_random_amount ? max_amt_qty : max_qty;
  }

  std::string_view trace_comment{
      "quantity has been selected as instrument maximal quantity"};

  if (selected_random_amount) {
    // This has to be checked firstly as max amount qty
    // can override max random qty
    trace_comment = std::string_view{
        "quantity has been selected "
        "based on RandomOrdersAmountMaximum value"};
  } else if (resting_qty_max_.source == Quantity::Source::Random_Qty) {
    trace_comment = std::string_view{
        "quantity has been selected "
        "based on RandomQtyMaximum value"};
  }

  trace_output(step_tracer,
               make_commented_value("max_qty", max_qty, trace_comment));
  return max_qty;
}

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_UTILS_QUANTITY_PARAMS_SELECTOR_HPP_
