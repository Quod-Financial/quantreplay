#include "ih/random/values/price_generation_params.hpp"

namespace simulator::generator::random {

auto PriceGenerationParams::Builder::set_price_tick_range(
    std::uint32_t px_tick_range) noexcept -> void {
  specified_params_.price_tick_range = px_tick_range;
}

auto PriceGenerationParams::Builder::set_price_tick_size(
    double px_tick_size) noexcept -> void {
  specified_params_.price_tick_size = px_tick_size;
}

auto PriceGenerationParams::Builder::set_price_spread(double px_spread) noexcept
    -> void {
  specified_params_.price_spread = px_spread;
}

PriceGenerationParams::PriceGenerationParams(Builder builder) noexcept
    : params_{builder.specified_params_} {}

auto PriceGenerationParams::get_price_tick_range() const noexcept
    -> std::uint32_t {
  return params_.price_tick_range;
}

auto PriceGenerationParams::get_price_tick_size() const noexcept -> double {
  return params_.price_tick_size;
}

auto PriceGenerationParams::get_price_spread() const noexcept -> double {
  return params_.price_spread;
}

}  // namespace simulator::generator::random
