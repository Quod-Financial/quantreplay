#ifndef SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_PRICE_WALK_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_PRICE_WALK_HPP_

#include <cstdint>

#include "core/tools/price_tick_ranges.hpp"

namespace simulator::generator::random {

enum class PriceDirection : std::uint8_t { Up, Down };

struct PriceWalk {
  double price{0.};
  // The sum of the moves: within one tick size, exactly ticks x tick size.
  double distance{0.};
};

[[nodiscard]]
auto tick_size_at(double price, const core::PriceTickRanges& tick_ranges)
    -> double;

[[nodiscard]]
auto shift_price(double price,
                 double offset,
                 PriceDirection direction,
                 const core::PriceTickRanges& tick_ranges) -> double;

[[nodiscard]]
auto walk_price(double start_price,
                PriceDirection direction,
                std::uint32_t ticks,
                const core::PriceTickRanges& tick_ranges) -> PriceWalk;

[[nodiscard]]
auto distance_price(double distance,
                    PriceDirection direction,
                    const core::PriceTickRanges& tick_ranges) -> double;

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_PRICE_WALK_HPP_
