#include "ih/market_data/depth/full_depth_update.hpp"

#include <stdexcept>

namespace simulator::trading_system::matching_engine::mdata {

FullDepthUpdate::FullDepthUpdate(std::vector<MarketDataEntry>& destination)
    : destination_{&destination} {}

auto FullDepthUpdate::add(const DepthLevel& level) const -> void {
  if (level.is_invisible() || level.is_removed()) [[unlikely]] {
    throw std::logic_error(
        "FullDepthUpdate::add: cannot add invisible or removed depth level "
        "to the full depth update");
  }
  destination_->emplace_back(to_snapshot_entry(level));
}

auto FullDepthUpdate::to_snapshot_entry(const DepthLevel& level)
    -> MarketDataEntry {
  MarketDataEntry entry{};
  entry.id = level.id();
  entry.price = level.price();
  entry.quantity = level.quantity();
  entry.type = level.type();
  return entry;
}

FullDepthUpdateBuilder::FullDepthUpdateBuilder(FullDepthUpdate destination)
    : update_(std::move(destination)) {}

LimitedFullDepthUpdateBuilder::LimitedFullDepthUpdateBuilder(
    FullDepthUpdate destination, const std::uint32_t limit)
    : update_(std::move(destination)), limit_(limit) {}

}  // namespace simulator::trading_system::matching_engine::mdata