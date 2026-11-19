#include "ih/random/values/price_generation_params.hpp"

#include <optional>
#include <utility>

#include "core/tools/price_tick_ranges.hpp"

namespace simulator::generator::random {

auto PriceGenerationParams::Builder::set_price_tick_range(
    std::uint32_t px_tick_range) noexcept -> void {
  specified_params_.price_tick_range = px_tick_range;
}

auto PriceGenerationParams::Builder::set_price_tick_ranges(
    core::PriceTickRanges px_tick_ranges) noexcept -> void {
  specified_params_.price_tick_ranges = std::move(px_tick_ranges);
}

auto PriceGenerationParams::Builder::set_price_spread(double px_spread) noexcept
    -> void {
  specified_params_.price_spread = px_spread;
}

PriceGenerationParams::PriceGenerationParams(Builder builder) noexcept
    : params_{std::move(builder.specified_params_)} {}

auto PriceGenerationParams::get_price_tick_range() const noexcept
    -> std::uint32_t {
  return params_.price_tick_range;
}

auto PriceGenerationParams::get_price_tick_ranges() const noexcept
    -> const core::PriceTickRanges& {
  return params_.price_tick_ranges;
}

auto PriceGenerationParams::get_price_spread() const noexcept
    -> std::optional<double> {
  return params_.price_spread;
}

}  // namespace simulator::generator::random
