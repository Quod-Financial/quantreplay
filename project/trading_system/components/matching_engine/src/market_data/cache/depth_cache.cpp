#include "ih/market_data/cache/depth_cache.hpp"

#include <algorithm>
#include <ranges>
#include <variant>
#include <vector>

#include "core/common/unreachable.hpp"
#include "core/tools/overload.hpp"
#include "ih/market_data/depth/depth_level.hpp"
#include "ih/market_data/depth/depth_stats_reader.hpp"
#include "ih/market_data/depth/full_depth_update.hpp"
#include "ih/market_data/depth/incremental_depth_update.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine::mdata {

namespace {

[[nodiscard]]
auto bid_side_requested(const StreamingSettings& settings) -> bool {
  return settings.is_data_type_requested(MdEntryType::Option::Bid) ||
         settings.is_data_type_requested(MdEntryType::Option::MarketBid);
}

[[nodiscard]]
auto offer_side_requested(const StreamingSettings& settings) -> bool {
  return settings.is_data_type_requested(MdEntryType::Option::Offer) ||
         settings.is_data_type_requested(MdEntryType::Option::MarketOffer);
}

[[nodiscard]]
auto requested_entries_filter(const StreamingSettings& settings) {
  return std::views::filter([&settings](const DepthLevel& level) {
    return settings.is_data_type_requested(level.type());
  });
}

}  // namespace

DepthCache::DepthCache(MarketEntryIdGenerator& id_generator)
    : bid_depth_(DepthSheet::create_bid_sheet(id_generator)),
      offer_depth_(DepthSheet::create_offer_sheet(id_generator)) {}

auto DepthCache::configure(const Config config) -> void { config_ = config; }

auto DepthCache::compose_initial(
    const StreamingSettings& settings,
    std::vector<MarketDataEntry>& destination) const -> void {
  if (bid_side_requested(settings)) {
    build_full_update(bid_depth_, settings, destination);
  }
  if (offer_side_requested(settings)) {
    build_full_update(offer_depth_, settings, destination);
  }
}

auto DepthCache::compose_update(const StreamingSettings& settings,
                                std::vector<MarketDataEntry>& destination) const
    -> void {
  if (bid_side_requested(settings)) {
    build_incremental_update(bid_depth_, settings, destination);
  }
  if (offer_side_requested(settings)) {
    build_incremental_update(offer_depth_, settings, destination);
  }
}

auto DepthCache::has_update(const StreamingSettings& settings) const -> bool {
  if (settings.is_top_of_book_only_requested()) {
    // The limited builder decides level visibility via stateful window
    // correction, so emptiness cannot be predicted per level.
    std::vector<MarketDataEntry> update;
    compose_update(settings, update);
    return !update.empty();
  }

  return (bid_side_requested(settings) &&
          sheet_has_update(bid_depth_, settings)) ||
         (offer_side_requested(settings) &&
          sheet_has_update(offer_depth_, settings));
}

auto DepthCache::capture(protocol::InstrumentState& state) const -> void {
  DepthStatsReader reader;

  reader.read(bid_depth_.view());
  state.current_bid_depth = CurrentBidDepth(reader.levels_count());
  if (const auto tob_price = reader.tob_price()) {
    state.best_bid_price = BestBidPrice(*tob_price);
  }

  reader.read(offer_depth_.view());
  state.current_offer_depth = CurrentOfferDepth(reader.levels_count());
  if (const auto tob_price = reader.tob_price()) {
    state.best_offer_price = BestOfferPrice(*tob_price);
  }
}

auto DepthCache::update(const std::vector<OrderBookNotification>& updates)
    -> void {
  bid_depth_.fold();
  offer_depth_.fold();

  const auto handler =
      core::overload([this](const OrderAdded& update) { apply(update); },
                     [this](const OrderReduced& update) { apply(update); },
                     [this](const OrderRemoved& update) { apply(update); },
                     [](const auto&) {});  // Ignore other types of updates

  for (const auto& update : updates) {
    std::visit(handler, update.value);
  }
}

auto DepthCache::apply(const auto& update) -> void {
  switch (static_cast<Side::Option>(update.order_side)) {
    case Side::Option::Buy:
      bid_depth_.apply(update);
      return;
    case Side::Option::Sell:
    case Side::Option::SellShort:
    case Side::Option::SellShortExempt:
      offer_depth_.apply(update);
      return;
  }
  log::warn(
      "unable to apply update, cannot convert order side to a known market "
      "entry type, ignoring {}",
      update);
}

auto DepthCache::build_full_update(
    const auto& sheet,
    const StreamingSettings& settings,
    std::vector<MarketDataEntry>& destination) const -> void {
  const FullDepthUpdate update{destination};
  const auto& excluded_owner = settings.excluded_orders_owner();

  if (config_.allow_orders_exclusion && excluded_owner.has_value()) {
    auto view = sheet.partial_view(*excluded_owner) |
                requested_entries_filter(settings);
    if (settings.is_top_of_book_only_requested()) {
      LimitedFullDepthUpdateBuilder builder(update, 1);
      builder.build(view);
    } else {
      FullDepthUpdateBuilder builder(update);
      builder.build(view);
    }
  } else {
    auto view = sheet.view() | requested_entries_filter(settings);
    if (settings.is_top_of_book_only_requested()) {
      LimitedFullDepthUpdateBuilder builder(update, 1);
      builder.build(view);
    } else {
      FullDepthUpdateBuilder builder(update);
      builder.build(view);
    }
  }
}

auto DepthCache::build_incremental_update(
    const auto& sheet,
    const StreamingSettings& settings,
    std::vector<MarketDataEntry>& destination) const -> void {
  const IncrementalDepthUpdate update{destination};
  const auto& excluded_owner = settings.excluded_orders_owner();

  if (config_.allow_orders_exclusion && excluded_owner.has_value()) {
    auto view = sheet.partial_view(*excluded_owner) |
                requested_entries_filter(settings);
    if (settings.is_top_of_book_only_requested()) {
      LimitedIncrementalDepthUpdateBuilder builder(update, 1);
      builder.build(view);
    } else {
      IncrementalDepthUpdateBuilder builder(update);
      builder.build(view);
    }
  } else {
    auto view = sheet.view() | requested_entries_filter(settings);
    if (settings.is_top_of_book_only_requested()) {
      LimitedIncrementalDepthUpdateBuilder builder(update, 1);
      builder.build(view);
    } else {
      IncrementalDepthUpdateBuilder builder(update);
      builder.build(view);
    }
  }
}

auto DepthCache::sheet_has_update(
    const auto& sheet, const StreamingSettings& settings) const -> bool {
  const auto updated = [](const DepthLevel& level) {
    return level.is_updated();
  };
  const auto& excluded_owner = settings.excluded_orders_owner();

  if (config_.allow_orders_exclusion && excluded_owner.has_value()) {
    return std::ranges::any_of(sheet.partial_view(*excluded_owner) |
                                   requested_entries_filter(settings),
                               updated);
  }
  return std::ranges::any_of(sheet.view() | requested_entries_filter(settings),
                             updated);
}

}  // namespace simulator::trading_system::matching_engine::mdata
