#include "ih/random/generators/price_generator.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <optional>
#include <utility>

#include "core/domain/attributes.hpp"
#include "core/tools/numeric.hpp"
#include "ih/constants.hpp"
#include "ih/market_data/market_state.hpp"
#include "ih/random/generators/price_walk.hpp"
#include "ih/random/generators/value_generator.hpp"
#include "ih/random/utils.hpp"
#include "ih/random/values/event.hpp"
#include "ih/random/values/price_generation_params.hpp"

namespace simulator::generator::random {
namespace {

auto divide_logs(double base, double val) -> double {
  return std::log(val) / std::log(base);
}

auto geometric_series_sum(std::uint32_t count, double koef, double first_value)
    -> double {
  return (first_value * (1 - std::pow(koef, count))) / (1 - koef);
}

[[nodiscard]]
auto best_price(const MarketState& actual_prices, Side side)
    -> std::optional<double> {
  const auto price = Utils::select_price(actual_prices, side);
  return core::equal(price.value_or(0.), 0.) ? std::nullopt : price;
}

[[nodiscard]]
auto minimal_spread(std::optional<double> configured_spread, double tick_size)
    -> double {
  return std::max(configured_spread.value_or(tick_size), tick_size);
}

[[nodiscard]]
auto walk_direction(Event event) -> PriceDirection {
  return event.is_buy_event() != event.is_resting_order_event()
             ? PriceDirection::Up
             : PriceDirection::Down;
}

}  // namespace

PriceGeneratorImpl::PriceGeneratorImpl(
    std::shared_ptr<random::ValueGenerator> random_int_generator) noexcept
    : random_int_generator_{std::move(random_int_generator)} {
  assert(random_int_generator_);
}

auto PriceGeneratorImpl::create(
    std::shared_ptr<random::ValueGenerator> random_int_generator)
    -> std::unique_ptr<PriceGeneratorImpl> {
  return std::make_unique<PriceGeneratorImpl>(std::move(random_int_generator));
}

auto PriceGeneratorImpl::generate_px(
    const PriceGenerationParams& params,
    const MarketState& actual_prices,
    const data_layer::PriceSeed& configured_prices,
    random::Event event)
    -> std::pair<double, PriceGenerator::GenerationDetails> {
  assert(event.is_buy_event() || event.is_sell_event());
  assert(event.is_resting_order_event() || event.is_aggressive_order_event());

  GenerationDetails details{};
  const auto& tick_ranges = params.get_price_tick_ranges();
  const auto price_side = event.target_side();

  const auto opposite_px =
      best_price(actual_prices, Utils::opposite(price_side));
  const auto market_px = opposite_px.has_value()
                             ? opposite_px
                             : best_price(actual_prices, price_side);
  const double reference_px =
      market_px.has_value()
          ? *market_px
          : resolve_configured_price(configured_prices, event);

  details.price_tick_size = tick_size_at(reference_px, tick_ranges);
  details.price_spread =
      minimal_spread(params.get_price_spread(), details.price_tick_size);

  if (!market_px.has_value()) {
    details.was_configured_px_used = true;
    return std::make_pair(reference_px, details);
  }

  const auto direction = walk_direction(event);
  details.was_base_px_increment_added = direction == PriceDirection::Up;

  const bool keeps_spread =
      opposite_px.has_value() && event.is_resting_order_event();
  const double base_px =
      keeps_spread
          ? shift_price(
                reference_px, details.price_spread, direction, tick_ranges)
          : reference_px;
  details.base_px = base_px;

  const auto tick_count = generate_tick_count(params, details);
  const auto walk = walk_price(base_px, direction, tick_count, tick_ranges);
  details.random_tick = walk.distance;
  details.base_px_increment = walk.distance;

  const double generated_px =
      walk.distance >= base_px
          ? distance_price(walk.distance, direction, tick_ranges)
          : walk.price;
  return std::make_pair(generated_px, details);
}

auto PriceGeneratorImpl::generate_tick_count(
    const PriceGenerationParams& params, GenerationDetails& details)
    -> std::uint32_t {
  constexpr int random_min = 0;
  const auto tick_range = params.get_price_tick_range();

  const auto geometric_sum = static_cast<int>(
      geometric_series_sum(tick_range,
                           constant::PriceGenerationCoefficient,
                           constant::PriceGenerationCoefficient));
  details.geometric_sum = geometric_sum;

  const int rand_value = random_int_generator_->generate_uniform_value(
      random_min, geometric_sum - 1);
  details.random_number = rand_value;

  constexpr auto log_base = constant::PriceGenerationCoefficient;
  const auto log_num = (rand_value * (log_base - 1)) / log_base + 1;
  const double px_deviation = std::ceil(divide_logs(log_base, log_num));

  const auto tick_count = static_cast<std::uint32_t>(tick_range - px_deviation);
  details.random_tick_count = tick_count;
  return tick_count;
}

auto PriceGeneratorImpl::resolve_configured_price(
    const data_layer::PriceSeed& configured_prices, random::Event event)
    -> double {
  assert(event.is_buy_event() || event.is_sell_event());
  const auto price_side = event.target_side();

  if (price_side == Side::Option::Buy) {
    if (configured_prices.bid_price().has_value()) {
      return *configured_prices.bid_price();
    }
  } else {
    if (configured_prices.offer_price().has_value()) {
      return *configured_prices.offer_price();
    }
  }

  return *configured_prices.mid_price();
}

}  // namespace simulator::generator::random
