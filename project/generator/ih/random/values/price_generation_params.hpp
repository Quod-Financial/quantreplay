#ifndef SIMULATOR_GENERATOR_IH_RANDOM_VALUES_PRICE_GENERATION_PARAMS_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_VALUES_PRICE_GENERATION_PARAMS_HPP_

#include <cstdint>

namespace simulator::generator::random {

class PriceGenerationParams {
  struct Params {
    double price_tick_size{0.};
    double price_spread{0.};
    std::uint32_t price_tick_range{0};
  };

 public:
  class Builder {
    friend class PriceGenerationParams;

   public:
    auto set_price_tick_range(std::uint32_t px_tick_range) noexcept -> void;

    auto set_price_tick_size(double px_tick_size) noexcept -> void;

    auto set_price_spread(double px_spread) noexcept -> void;

   private:
    Params specified_params_;
  };

  PriceGenerationParams() = delete;

  explicit PriceGenerationParams(Builder builder) noexcept;

  [[nodiscard]]
  auto get_price_tick_range() const noexcept -> std::uint32_t;

  [[nodiscard]]
  auto get_price_tick_size() const noexcept -> double;

  [[nodiscard]]
  auto get_price_spread() const noexcept -> double;

 private:
  Params params_;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_VALUES_PRICE_GENERATION_PARAMS_HPP_
