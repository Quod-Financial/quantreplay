#include "ih/market_data/depth/incremental_depth_update.hpp"

#include <stdexcept>

#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/depth/depth_level.hpp"

namespace simulator::trading_system::matching_engine::mdata {

IncrementalDepthUpdate::IncrementalDepthUpdate(
    std::vector<MarketDataEntry>& destination)
    : destination_{&destination} {}

auto IncrementalDepthUpdate::add_new_level(const DepthLevel& level) const
    -> void {
  if (level.is_removed() || level.is_invisible()) [[unlikely]] {
    throw std::logic_error(
        "IncrementalDepthUpdate::add_new_level: cannot add removed or "
        "invisible level as new level to the incremental depth update");
  }

  auto entry = create_entry(level);
  entry.action = MarketEntryAction::Option::New;
  entry.quantity = level.quantity();
  destination_->emplace_back(std::move(entry));
}

auto IncrementalDepthUpdate::add_changed_level(const DepthLevel& level) const
    -> void {
  if (level.is_added() || level.is_removed() || level.is_unchanged() ||
      level.is_invisible()) [[unlikely]] {
    throw std::logic_error(
        "IncrementalDepthUpdate::add_changed_level: cannot add added, removed, "
        "unchanged or invisible level as changed level "
        "to the incremental depth update");
  }

  auto entry = create_entry(level);
  entry.action = MarketEntryAction::Option::Change;
  entry.quantity = level.quantity();
  destination_->emplace_back(std::move(entry));
}

auto IncrementalDepthUpdate::add_removed_level(const DepthLevel& level) const
    -> void {
  if (level.is_added() || level.is_invisible()) [[unlikely]] {
    throw std::logic_error(
        "IncrementalDepthUpdate::add_removed_level: cannot add added or "
        "invisible level as deleted level to the incremental depth update");
  }

  auto entry = create_entry(level);
  entry.action = MarketEntryAction::Option::Delete;
  destination_->emplace_back(std::move(entry));
}

auto IncrementalDepthUpdate::create_entry(const DepthLevel& level)
    -> MarketDataEntry {
  MarketDataEntry entry{};
  entry.id = level.id();
  entry.price = level.price();
  entry.type = level.type();
  return entry;
}

IncrementalDepthUpdateBuilder::IncrementalDepthUpdateBuilder(
    IncrementalDepthUpdate destination)
    : update_(std::move(destination)) {}

auto IncrementalDepthUpdateBuilder::add_updated_level(
    const DepthLevel& level) const -> void {
  if (level.is_added()) {
    update_.add_new_level(level);
  } else if (level.is_changed()) {
    update_.add_changed_level(level);
  } else if (level.is_removed()) {
    update_.add_removed_level(level);
  }
}

LimitedIncrementalDepthUpdateBuilder::LimitedIncrementalDepthUpdateBuilder(
    IncrementalDepthUpdate destination, const std::uint32_t limit)
    : update_(std::move(destination)), limit_(limit) {}

auto LimitedIncrementalDepthUpdateBuilder::handle_in_window_level(
    const DepthLevel& level, Context& context) const -> void {
  if (level.is_added()) {
    update_.add_new_level(level);
    context.window_correction_factor++;
  } else if (level.is_changed()) {
    update_.add_changed_level(level);
  } else if (level.is_removed()) {
    update_.add_removed_level(level);
    context.window_correction_factor--;
  }

  // Ignoring unchanged levels, as they should not be reported
  // in incremental update, but still incrementing the number of processed
  // levels inside the window of visible levels.
  context.window_levels_pocessed++;
}

auto LimitedIncrementalDepthUpdateBuilder::handle_out_window_level(
    const DepthLevel& level, Context& context) const -> void {
  const bool must_add_level = context.window_correction_factor < 0;
  const bool must_remove_level = context.window_correction_factor > 0;

  if (must_add_level && !level.is_removed()) {
    update_.add_new_level(level);
    context.window_correction_factor++;
  } else if (must_remove_level && !level.is_added()) {
    update_.add_removed_level(level);
    context.window_correction_factor--;
  }
}

}  // namespace simulator::trading_system::matching_engine::mdata