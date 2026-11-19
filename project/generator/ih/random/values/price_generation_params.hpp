#ifndef SIMULATOR_GENERATOR_IH_RANDOM_VALUES_PRICE_GENERATION_PARAMS_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_VALUES_PRICE_GENERATION_PARAMS_HPP_

#include <cstdint>
#include <optional>

#include "core/tools/price_tick_ranges.hpp"

namespace simulator::generator::random {

class PriceGenerationParams {
  struct Params {
    core::PriceTickRanges price_tick_ranges;
    std::optional<double> price_spread;
    std::uint32_t price_tick_range{0};
  };

 public:
  class Builder {
    friend class PriceGenerationParams;

   public:
    auto set_price_tick_range(std::uint32_t px_tick_range) noexcept -> void;

    auto set_price_tick_ranges(core::PriceTickRanges px_tick_ranges) noexcept
        -> void;

    auto set_price_spread(double px_spread) noexcept -> void;

   private:
    Params specified_params_;
  };

  PriceGenerationParams() = delete;

  explicit PriceGenerationParams(Builder builder) noexcept;

  [[nodiscard]]
  auto get_price_tick_range() const noexcept -> std::uint32_t;

  [[nodiscard]]
  auto get_price_tick_ranges() const noexcept -> const core::PriceTickRanges&;

  [[nodiscard]]
  auto get_price_spread() const noexcept -> std::optional<double>;

 private:
  Params params_;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_VALUES_PRICE_GENERATION_PARAMS_HPP_
