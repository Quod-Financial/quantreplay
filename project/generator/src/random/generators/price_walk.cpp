#include "ih/random/generators/price_walk.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>

#include "core/tools/numeric.hpp"
#include "core/tools/price_tick_ranges.hpp"
#include "ih/constants.hpp"

namespace simulator::generator::random {
namespace {

[[nodiscard]]
auto move_price(double price, double offset, PriceDirection direction)
    -> double {
  return direction == PriceDirection::Up ? price + offset : price - offset;
}

[[nodiscard]]
auto distance_between(double from_price,
                      double to_price,
                      PriceDirection direction) -> double {
  return direction == PriceDirection::Up ? to_price - from_price
                                         : from_price - to_price;
}

[[nodiscard]]
auto round_to_tick_size(double price,
                        double tick_size,
                        PriceDirection direction) -> double {
  if (core::is_fully_divisible(price, tick_size)) {
    return price;
  }

  const double ticks = price / tick_size;
  return tick_size * (direction == PriceDirection::Up ? std::ceil(ticks)
                                                      : std::floor(ticks));
}

[[nodiscard]]
auto round_to_range_tick_size(double price,
                              PriceDirection direction,
                              const core::PriceTickRanges& tick_ranges)
    -> double {
  double rounded_price = price;
  // A rounding that needs another has entered a further range, so at most one
  // rounding per range is needed.
  for (std::size_t rounding = 0; rounding < tick_ranges.ranges().size();
       ++rounding) {
    const double tick_size = tick_size_at(rounded_price, tick_ranges);
    // a price is stepped by a zero or negative tick size, but not rounded to it
    if (tick_size <= 0.) {
      break;
    }
    rounded_price = round_to_tick_size(rounded_price, tick_size, direction);
    if (tick_size_at(rounded_price, tick_ranges) == tick_size) {
      break;
    }
  }
  return rounded_price;
}

[[nodiscard]]
auto count_steps_of_tick_size(double price,
                              double tick_size,
                              std::uint32_t max_steps,
                              PriceDirection direction,
                              const core::PriceTickRanges& tick_ranges)
    -> std::uint32_t {
  std::uint32_t steps = 1;
  while (steps < max_steps &&
         tick_size_at(move_price(price, steps * tick_size, direction),
                      tick_ranges) == tick_size) {
    ++steps;
  }
  return steps;
}

}  // namespace

auto tick_size_at(double price, const core::PriceTickRanges& tick_ranges)
    -> double {
  return tick_ranges.tick_of(price).value_or(
      constant::DefaultListingPriceTickSize);
}

auto shift_price(double price,
                 double offset,
                 PriceDirection direction,
                 const core::PriceTickRanges& tick_ranges) -> double {
  const double shifted_price = move_price(price, offset, direction);
  const bool keeps_tick_size = tick_size_at(shifted_price, tick_ranges) ==
                               tick_size_at(price, tick_ranges);
  return keeps_tick_size
             ? shifted_price
             : round_to_range_tick_size(shifted_price, direction, tick_ranges);
}

auto walk_price(double start_price,
                PriceDirection direction,
                std::uint32_t ticks,
                const core::PriceTickRanges& tick_ranges) -> PriceWalk {
  PriceWalk walk{.price = start_price, .distance = 0.};
  std::uint32_t remaining_ticks = ticks;
  while (remaining_ticks > 0) {
    const double tick_size = tick_size_at(walk.price, tick_ranges);
    const std::uint32_t steps = count_steps_of_tick_size(
        walk.price, tick_size, remaining_ticks, direction, tick_ranges);

    const double offset = steps * tick_size;
    const double unrounded_price = move_price(walk.price, offset, direction);
    walk.price = shift_price(walk.price, offset, direction, tick_ranges);
    walk.distance +=
        offset + distance_between(unrounded_price, walk.price, direction);
    remaining_ticks -= steps;
  }
  return walk;
}

auto distance_price(double distance,
                    PriceDirection direction,
                    const core::PriceTickRanges& tick_ranges) -> double {
  if (distance <= 0.) {
    return distance;
  }
  const double price =
      round_to_range_tick_size(distance, direction, tick_ranges);
  if (price > 0.) {
    return price;
  }
  return round_to_range_tick_size(
      tick_size_at(0., tick_ranges), PriceDirection::Up, tick_ranges);
}

}  // namespace simulator::generator::random
