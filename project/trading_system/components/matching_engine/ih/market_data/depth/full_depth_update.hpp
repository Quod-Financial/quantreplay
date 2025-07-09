#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_FULL_DEPTH_UPDATE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_FULL_DEPTH_UPDATE_HPP_

#include <gsl/gsl>
#include <ranges>
#include <vector>

#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/depth/depth_level.hpp"
#include "ih/market_data/tools/algorithms.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class FullDepthUpdate {
 public:
  explicit FullDepthUpdate(std::vector<MarketDataEntry>& destination);

  auto add(const DepthLevel& level) const -> void;

 private:
  static auto to_snapshot_entry(const DepthLevel& level) -> MarketDataEntry;

  gsl::not_null<std::vector<MarketDataEntry>*> destination_;
};

class FullDepthUpdateBuilder {
 public:
  explicit FullDepthUpdateBuilder(FullDepthUpdate destination);

  auto build(const std::ranges::forward_range auto& depth) -> void {
    static_assert(
        std::same_as<DepthLevel, std::ranges::range_value_t<decltype(depth)>>,
        "cannot build full depth update from a depth range "
        "which does not contain DepthLevel type");

    constexpr auto filter = std::views::filter([](const DepthLevel& level) {
      return level.is_added() || level.is_changed() || level.is_unchanged();
    });

    std::ranges::for_each(depth | filter,
                          [this](const auto& level) { update_.add(level); });
  }

 private:
  FullDepthUpdate update_;
};

class LimitedFullDepthUpdateBuilder {
 public:
  explicit LimitedFullDepthUpdateBuilder(FullDepthUpdate destination,
                                         std::uint32_t limit);

  auto build(const std::ranges::forward_range auto& depth) -> void {
    static_assert(
        std::same_as<DepthLevel, std::ranges::range_value_t<decltype(depth)>>,
        "cannot build limited full depth update from a depth range "
        "which does not contain DepthLevel type");

    constexpr auto filter = std::views::filter([](const DepthLevel& level) {
      return level.is_added() || level.is_changed() || level.is_unchanged();
    });

    std::ranges::for_each(depth | filter | std::views::take(limit_),
                          [this](const auto& level) { update_.add(level); });
  }

 private:
  FullDepthUpdate update_;
  std::uint32_t limit_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_FULL_DEPTH_UPDATE_HPP_
