#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_INCREMENTAL_DEPTH_UPDATE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_INCREMENTAL_DEPTH_UPDATE_HPP_

#include <gsl/gsl>
#include <ranges>
#include <vector>

#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/depth/depth_level.hpp"
#include "ih/market_data/tools/algorithms.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class IncrementalDepthUpdate {
 public:
  explicit IncrementalDepthUpdate(std::vector<MarketDataEntry>& destination);

  auto add_new_level(const DepthLevel& level) const -> void;

  auto add_changed_level(const DepthLevel& level) const -> void;

  auto add_removed_level(const DepthLevel& level) const -> void;

 private:
  static auto create_entry(const DepthLevel& level) -> MarketDataEntry;

  gsl::not_null<std::vector<MarketDataEntry>*> destination_;
};

class IncrementalDepthUpdateBuilder {
 public:
  explicit IncrementalDepthUpdateBuilder(IncrementalDepthUpdate destination);

  auto build(const std::ranges::forward_range auto& depth) -> void {
    static_assert(
        std::same_as<DepthLevel, std::ranges::range_value_t<decltype(depth)>>,
        "cannot build incremental depth update from a depth range "
        "which does not contain DepthLevel type");

    constexpr auto filter = std::views::filter([](const DepthLevel& level) {
      return level.is_added() || level.is_changed() || level.is_removed();
    });

    std::ranges::for_each(depth | filter, [this](const auto& level) {
      add_updated_level(level);
    });
  }

 private:
  auto add_updated_level(const DepthLevel& level) const -> void;

  IncrementalDepthUpdate update_;
};

class LimitedIncrementalDepthUpdateBuilder {
  struct Context {
    // Number of levels processed within the window.
    std::size_t window_levels_pocessed = 0;

    // A factor that is used to correct a window of visible levels.
    // Negative value means that levels were removed from the window.
    // Positive value means that levels were added to the window.
    std::int64_t window_correction_factor = 0;
  };

 public:
  explicit LimitedIncrementalDepthUpdateBuilder(
      IncrementalDepthUpdate destination, std::uint32_t limit);

  auto build(const std::ranges::forward_range auto& depth) -> void {
    static_assert(
        std::same_as<DepthLevel, std::ranges::range_value_t<decltype(depth)>>,
        "cannot build limited incremental depth update from a depth range "
        "which does not contain DepthLevel type");

    constexpr auto filter = std::views::filter(
        [](const DepthLevel& level) { return !level.is_invisible(); });

    Context context{};
    for (const auto& level : depth | filter) {
      if (context.window_levels_pocessed < limit_) {
        handle_in_window_level(level, context);
      } else if (context.window_correction_factor != 0) {
        handle_out_window_level(level, context);
      } else {
        break;
      }
    }
  }

 private:
  auto handle_in_window_level(const DepthLevel& level, Context& context) const
      -> void;

  auto handle_out_window_level(const DepthLevel& level, Context& context) const
      -> void;

  IncrementalDepthUpdate update_;
  std::uint32_t limit_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_INCREMENTAL_DEPTH_UPDATE_HPP_
