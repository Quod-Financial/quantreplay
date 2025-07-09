#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_STATS_READER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_STATS_READER_HPP_

#include <cstdint>
#include <optional>
#include <ranges>

#include "core/domain/attributes.hpp"
#include "ih/market_data/depth/depth_level.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class DepthStatsReader {
 public:
  auto tob_quantity() const -> std::optional<Quantity> { return tob_quantity_; }

  auto tob_price() const -> std::optional<Price> { return tob_price_; }

  auto levels_count() const -> std::uint32_t { return levels_count_; }

  auto read(const std::ranges::forward_range auto& depth) -> void {
    static_assert(
        std::same_as<DepthLevel, std::ranges::range_value_t<decltype(depth)>>,
        "cannot build full depth update from a depth range "
        "which does not contain DepthLevel type");

    const auto filtered = std::views::filter(depth, [](const auto& level) {
      return level.is_added() || level.is_changed() || level.is_unchanged();
    });

    examine_first_level(filtered);
    determine_levels_count(filtered);
  }

 private:
  auto examine_first_level(auto depth) -> void {
    tob_quantity_ = std::nullopt;
    tob_price_ = std::nullopt;
    levels_count_ = 0;

    for (const auto& level : std::views::take(depth, 1)) {
      tob_quantity_ = level.quantity();
      tob_price_ = level.price();
    }
  }

  auto determine_levels_count(auto depth) -> void {
    levels_count_ = static_cast<std::uint32_t>(std::ranges::distance(depth));
  }

  std::optional<Quantity> tob_quantity_;
  std::optional<Price> tob_price_;
  std::uint32_t levels_count_{0};
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_STATS_READER_HPP_
