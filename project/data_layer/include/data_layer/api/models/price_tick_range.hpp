#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_PRICE_TICK_RANGE_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_PRICE_TICK_RANGE_HPP_

#include <cstdint>
#include <optional>

namespace simulator::data_layer {

class PriceTickRange {
 public:
  class Patch;

  enum class Attribute : std::uint8_t {
    ListingId,
    TickSize,
    UpperLimit,
    RowPosition
  };

  [[nodiscard]]
  static auto create(const Patch& snapshot) -> PriceTickRange;

  [[nodiscard]]
  auto tick_size() const noexcept -> double;

  [[nodiscard]]
  auto upper_limit() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto operator==(const PriceTickRange&) const -> bool = default;

 private:
  PriceTickRange() = default;

  double tick_size_{0.0};
  std::optional<double> upper_limit_;
};

class PriceTickRange::Patch {
  friend class PriceTickRange;

 public:
  using Attribute = PriceTickRange::Attribute;

  [[nodiscard]]
  auto tick_size() const noexcept -> std::optional<double>;
  auto with_tick_size(double tick_size) noexcept -> Patch&;

  [[nodiscard]]
  auto upper_limit() const noexcept -> std::optional<double>;
  auto with_upper_limit(std::optional<double> upper_limit) noexcept -> Patch&;

  [[nodiscard]]
  auto operator==(const Patch&) const -> bool = default;

 private:
  std::optional<double> tick_size_;
  std::optional<double> upper_limit_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_PRICE_TICK_RANGE_HPP_
