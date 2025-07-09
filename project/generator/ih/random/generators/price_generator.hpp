#ifndef SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_PRICE_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_PRICE_GENERATOR_HPP_

#include <cassert>
#include <memory>
#include <optional>
#include <utility>

#include "data_layer/api/models/price_seed.hpp"
#include "ih/context/order_market_data_provider.hpp"
#include "ih/random/generators/value_generator.hpp"
#include "ih/random/values/event.hpp"
#include "ih/random/values/price_generation_params.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::random {

class PriceGenerator {
 protected:
  struct GenerationDetails {
    double base_px{0.};
    double base_px_increment{0.};
    double random_tick{0.};

    int geometric_sum{0};
    int random_number{0};

    bool was_base_px_increment_added{false};
    bool was_configured_px_used{false};
  };

 public:
  virtual ~PriceGenerator() = default;

  template <typename Tracer = trace::NullTracer>
  auto generate_price(const PriceGenerationParams& params,
                      const MarketState& actual_prices,
                      const data_layer::PriceSeed& configured_prices,
                      random::Event event,
                      Tracer&& tracer = Tracer{}) -> double;

 private:
  virtual auto generate_px(const PriceGenerationParams& params,
                           const MarketState& actual_prices,
                           const data_layer::PriceSeed& configured_prices,
                           random::Event event)
      -> std::pair<double, GenerationDetails> = 0;
};

template <typename Tracer>
auto PriceGenerator::generate_price(
    const PriceGenerationParams& params,
    const MarketState& actual_prices,
    const data_layer::PriceSeed& configured_prices,
    random::Event event,
    Tracer&& tracer) -> double {
  using namespace trace;

  assert(event.is_buy_event() || event.is_sell_event());
  assert(event.is_resting_order_event() || event.is_aggressive_order_event());

  auto step = make_step(tracer, "generating order price");
  trace_input(step,
              make_value("priceTickRange", params.get_price_tick_range()));
  trace_input(step, make_value("priceTickSize", params.get_price_tick_size()));
  trace_input(step, make_value("priceSpread", params.get_price_spread()));
  trace_input(
      step, make_value("bestBuyPrice", actual_prices.best_bid_price.value_or(0)));
  trace_input(
      step,
      make_value("bestSellPrice", actual_prices.best_offer_price.value_or(0)));

  const auto mid_price = configured_prices.mid_price().value_or(0);
  trace_input(step,
              make_value("priceSeedBuy",
                         configured_prices.bid_price().value_or(mid_price)));
  trace_input(step,
              make_value("priceSeedSell",
                         configured_prices.offer_price().value_or(mid_price)));
  trace_input(step, make_value("priceSeedMid", mid_price));
  trace_input(
      step, make_value("currentSide", fmt::format("{}", event.target_side())));

  const auto [generated_px, details] =
      generate_px(params, actual_prices, configured_prices, event);

  if (details.was_configured_px_used) {
    trace_output(
        step,
        make_commented_value("generatedPrice",
                             generated_px,
                             "price was generated from seed configuration"));
  } else {
    trace_output(step, make_value("randomValue", details.random_number));
    trace_output(step, make_value("geometricSum", details.geometric_sum));
    trace_output(step, make_value("randomTick", details.random_tick));

    trace_output(step,
                 make_commented_value(
                     "basePx",
                     details.base_px,
                     "The current best price "
                     "with price spread conditionally added/subtracted"));

    trace_output(step,
                 make_commented_value(
                     "basePxIncrement",
                     details.base_px_increment,
                     "Random price deviation added/subtracted to/from basePx"));

    trace_output(
        step,
        make_commented_value("wasBasePxIncrementAdded",
                             details.was_base_px_increment_added,
                             "indicates if a basePxIncrement was added to "
                             "or subtracted from basePx"));

    trace_output(
        step,
        make_commented_value(
            "generatedPrice",
            generated_px,
            "price was generated from a base price and random price tick"));
  }

  trace_step(std::move(step), tracer);
  return generated_px;
}

class PriceGeneratorImpl final : public random::PriceGenerator {
 public:
  explicit PriceGeneratorImpl(
      std::shared_ptr<random::ValueGenerator> random_int_generator) noexcept;

  static auto create(
      std::shared_ptr<random::ValueGenerator> random_int_generator)
      -> std::unique_ptr<PriceGeneratorImpl>;

 private:
  auto generate_px(const PriceGenerationParams& params,
                   const MarketState& actual_prices,
                   const data_layer::PriceSeed& configured_prices,
                   random::Event event)
      -> std::pair<double, GenerationDetails> override;

  auto generate_tick(const PriceGenerationParams& params,
                     GenerationDetails& details) -> double;

  static auto resolve_base_price(const MarketState& actual_prices,
                                 const PriceGenerationParams& generation_params,
                                 random::Event event) -> std::optional<double>;

  static auto resolve_configured_price(
      const data_layer::PriceSeed& configured_prices, random::Event event)
      -> double;

  std::shared_ptr<random::ValueGenerator> random_int_generator_;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_PRICE_GENERATOR_HPP_
