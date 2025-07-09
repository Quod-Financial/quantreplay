#include "ih/market_data/cache/depth_cache.hpp"

#include <variant>
#include <vector>

#include "core/common/unreachable.hpp"
#include "core/tools/overload.hpp"
#include "ih/market_data/depth/depth_stats_reader.hpp"
#include "ih/market_data/depth/full_depth_update.hpp"
#include "ih/market_data/depth/incremental_depth_update.hpp"
#include "ih/market_data/tools/algorithms.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine::mdata {

DepthCache::DepthCache(MarketEntryIdGenerator& id_generator)
    : bid_depth_(DepthSheet::create_bid_sheet(id_generator)),
      offer_depth_(DepthSheet::create_offer_sheet(id_generator)) {}

auto DepthCache::configure(const Config config) -> void { config_ = config; }

auto DepthCache::compose_initial(
    const StreamingSettings& settings,
    std::vector<MarketDataEntry>& destination) const -> void {
  if (settings.is_data_type_requested(MdEntryType::Option::Bid)) {
    build_full_update(bid_depth_, settings, destination);
  }
  if (settings.is_data_type_requested(MdEntryType::Option::Offer)) {
    build_full_update(offer_depth_, settings, destination);
  }
}

auto DepthCache::compose_update(const StreamingSettings& settings,
                                std::vector<MarketDataEntry>& destination) const
    -> void {
  if (settings.is_full_update_requested()) {
    compose_initial(settings, destination);
    return;
  }

  if (settings.is_data_type_requested(MdEntryType::Option::Bid)) {
    build_incremental_update(bid_depth_, settings, destination);
  }
  if (settings.is_data_type_requested(MdEntryType::Option::Offer)) {
    build_incremental_update(offer_depth_, settings, destination);
  }
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
  if (to_entry_type(update.order_side) == MdEntryType::Option::Bid) {
    bid_depth_.apply(update);
  } else if (to_entry_type(update.order_side) == MdEntryType::Option::Offer) {
    offer_depth_.apply(update);
  } else {
    log::warn(
        "unable to apply update, cannot conveert order side to a known market "
        "entry type, ignoring {}",
        update);
  }
}

auto DepthCache::build_full_update(
    const auto& sheet,
    const StreamingSettings& settings,
    std::vector<MarketDataEntry>& destination) const -> void {
  const FullDepthUpdate update{destination};
  const auto& excluded_owner = settings.excluded_orders_owner();

  if (config_.allow_orders_exclusion && excluded_owner.has_value()) {
    const auto view = sheet.partial_view(*excluded_owner);
    if (settings.is_top_of_book_only_requested()) {
      LimitedFullDepthUpdateBuilder builder(update, 1);
      builder.build(view);
    } else {
      FullDepthUpdateBuilder builder(update);
      builder.build(view);
    }
  } else {
    const auto view = sheet.view();
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
    const auto view = sheet.partial_view(*excluded_owner);
    if (settings.is_top_of_book_only_requested()) {
      LimitedIncrementalDepthUpdateBuilder builder(update, 1);
      builder.build(view);
    } else {
      IncrementalDepthUpdateBuilder builder(update);
      builder.build(view);
    }
  } else {
    const auto view = sheet.view();
    if (settings.is_top_of_book_only_requested()) {
      LimitedIncrementalDepthUpdateBuilder builder(update, 1);
      builder.build(view);
    } else {
      IncrementalDepthUpdateBuilder builder(update);
      builder.build(view);
    }
  }
}

}  // namespace simulator::trading_system::matching_engine::mdata