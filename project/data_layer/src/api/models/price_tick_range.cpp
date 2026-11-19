#include "api/models/price_tick_range.hpp"

#include <optional>

#include "ih/common/exceptions.hpp"

namespace simulator::data_layer {

auto PriceTickRange::create(const Patch& snapshot) -> PriceTickRange {
  if (!snapshot.tick_size_.has_value()) {
    throw RequiredAttributeMissing{"PriceTickRange", "TickSize"};
  }

  PriceTickRange range;
  range.tick_size_ = *snapshot.tick_size_;
  range.upper_limit_ = snapshot.upper_limit_;
  return range;
}

auto PriceTickRange::tick_size() const noexcept -> double { return tick_size_; }

auto PriceTickRange::upper_limit() const noexcept -> std::optional<double> {
  return upper_limit_;
}

auto PriceTickRange::Patch::tick_size() const noexcept
    -> std::optional<double> {
  return tick_size_;
}

auto PriceTickRange::Patch::with_tick_size(double tick_size) noexcept
    -> Patch& {
  tick_size_ = tick_size;
  return *this;
}

auto PriceTickRange::Patch::upper_limit() const noexcept
    -> std::optional<double> {
  return upper_limit_;
}

auto PriceTickRange::Patch::with_upper_limit(
    std::optional<double> upper_limit) noexcept -> Patch& {
  upper_limit_ = upper_limit;
  return *this;
}

}  // namespace simulator::data_layer
