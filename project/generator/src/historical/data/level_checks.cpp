#include "ih/historical/data/level_checks.hpp"

#include <optional>

namespace simulator::generator::historical {
namespace {

[[nodiscard]]
auto is_empty(const std::optional<double> qty) noexcept -> bool {
  return !qty.has_value() || qty.value() <= 0.0;
}

}  // namespace

auto has_valid_bid(const historical::Level& level) noexcept -> bool {
  return level.bid_price().has_value() && !is_empty(level.bid_quantity());
}

auto has_valid_offer(const historical::Level& level) noexcept -> bool {
  return level.offer_price().has_value() && !is_empty(level.offer_quantity());
}

}  // namespace simulator::generator::historical
