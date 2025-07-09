#include "ih/random/generators/price_generator.hpp"

#include <cassert>
#include <cmath>
#include <optional>
#include <utility>

#include "core/tools/numeric.hpp"
#include "ih/constants.hpp"
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

  const auto opt_base_price = resolve_base_price(actual_prices, params, event);
  if (!opt_base_price.has_value()) {
    const double configured_px =
        resolve_configured_price(configured_prices, event);
    details.was_configured_px_used = true;
    return std::make_pair(configured_px, details);
  }

  const auto base_px = *opt_base_price;
  details.base_px = base_px;

  // Add a random price increment to a base price for
  // aggressive-buy and resting-sell orders
  //
  // Subtract a random price increment from a base price for
  // aggressive-sell and resting-buy orders
  const bool increment_px =
      event.is_buy_event() ^ event.is_resting_order_event();
  details.was_base_px_increment_added = increment_px;

  const double tick = generate_tick(params, details);
  details.base_px_increment = tick;

  if (tick >= base_px) {
    return std::make_pair(tick, details);
  }

  const double price = increment_px ? base_px + tick : base_px - tick;

  return std::make_pair(price, details);
}

auto PriceGeneratorImpl::generate_tick(const PriceGenerationParams& params,
                                       GenerationDetails& details) -> double {
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

  const double random_tick = tick_range - px_deviation;
  const double scaled_random_tick = random_tick * params.get_price_tick_size();

  details.random_tick = scaled_random_tick;
  return scaled_random_tick;
}

auto PriceGeneratorImpl::resolve_base_price(
    const MarketState& actual_prices,
    const PriceGenerationParams& generation_params,
    random::Event event) -> std::optional<double> {
  assert(event.is_aggressive_order_event() || event.is_resting_order_event());
  assert(event.is_buy_event() || event.is_sell_event());
  const auto price_side = event.target_side();

  // Try resolve a base price as the best price for an opposite side
  auto resolved_price =
      Utils::select_price(actual_prices, Utils::opposite(price_side));
  if (!resolved_price || core::equal(*resolved_price, 0.)) {
    // In case an opposite side is empty (does not have the best price)
    // try to resolve base price as the best price for a current side
    // and return it immediately.
    resolved_price = Utils::select_price(actual_prices, price_side);
    return core::equal(*resolved_price, 0.) ? std::nullopt : resolved_price;
  }

  if (event.is_resting_order_event()) {
    // Subtract spread from the resolved price if generating a buy price OR
    // add spread to the resolved price if generating a sell price
    resolved_price =
        event.is_buy_event()
            ? *resolved_price - generation_params.get_price_spread()
            : *resolved_price + generation_params.get_price_spread();
  }

  return resolved_price;
}

auto PriceGeneratorImpl::resolve_configured_price(
    const data_layer::PriceSeed& configured_prices, random::Event event)
    -> double {
  assert(event.is_buy_event() || event.is_sell_event());
  const auto price_side = event.target_side();

  return price_side == Side::Option::Buy ? *configured_prices.bid_price()
                                         : *configured_prices.offer_price();
}

}  // namespace simulator::generator::random
