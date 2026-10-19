#include "ih/market_data/market_data_book_cache.hpp"

#include <algorithm>
#include <cstddef>
#include <mutex>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

#include "log/logging.hpp"

namespace simulator::generator::mdata {
namespace {

[[nodiscard]]
auto is_book_entry(const MarketDataEntry& entry) -> bool {
  return entry.type == MdEntryType::Option::Bid ||
         entry.type == MdEntryType::Option::Offer;
}

// An entry without identifier stands for the whole level of its price.
[[nodiscard]]
auto matches(const MarketDataEntry& candidate,
             const MarketDataEntry& entry) -> bool {
  if (entry.id.has_value()) {
    return candidate.id == entry.id;
  }
  return !candidate.id.has_value() && entry.price.has_value() &&
         candidate.price == entry.price;
}

// Tells whether the entry has to be placed ahead of the candidate, entries of
// the same price keep the order they have been received in.
[[nodiscard]]
auto precedes(const MarketDataEntry& entry,
              const MarketDataEntry& candidate) -> bool {
  if (!entry.price.has_value() || !candidate.price.has_value()) {
    return false;
  }
  return entry.type == MdEntryType::Option::Bid
             ? *entry.price > *candidate.price
             : *entry.price < *candidate.price;
}

auto merge_into(MarketDataEntry& target,
                const MarketDataEntry& source) -> void {
  if (source.type.has_value()) {
    target.type = source.type;
  }
  if (source.price.has_value()) {
    target.price = source.price;
  }
  if (source.quantity.has_value()) {
    target.quantity = source.quantity;
  }
  if (source.buyer_id.has_value()) {
    target.buyer_id = source.buyer_id;
  }
  if (source.seller_id.has_value()) {
    target.seller_id = source.seller_id;
  }
  if (source.time.has_value()) {
    target.time = source.time;
  }
}

auto trim_side(std::vector<MarketDataEntry>& side, MarketDepth depth) -> void {
  std::size_t levels = 0;
  std::optional<Price> level_price;
  for (auto entry = side.begin(); entry != side.end(); ++entry) {
    if (levels == 0 || entry->price != level_price) {
      level_price = entry->price;
      ++levels;
    }
    if (levels > static_cast<std::size_t>(depth.value())) {
      side.erase(entry, side.end());
      return;
    }
  }
}

[[nodiscard]]
auto from_fix_feed(const protocol::MarketDataSnapshot& snapshot) -> bool {
  return std::holds_alternative<protocol::fix::Session>(snapshot.session.value);
}

}  // namespace

MarketDataBookCache::Book::Book(InstrumentDescriptor instrument,
                                const std::vector<MarketDataEntry>& entries,
                                MarketDepth depth)
    : instrument_{std::move(instrument)}, depth_{depth} {
  for (const MarketDataEntry& entry : entries) {
    if (std::vector<MarketDataEntry>* const side = side_of(entry)) {
      side->push_back(entry);
    }
  }
  trim();
}

auto MarketDataBookCache::Book::apply(const MarketDataEntry& entry)
    -> EntryOutcome {
  if (entry.type.has_value() && !is_book_entry(entry)) {
    log::debug(
        "skipping market data entry of type `{}', which does not describe an "
        "order book level",
        entry.type);
    return EntryOutcome::Skipped;
  }

  if (!entry.action.has_value()) {
    log::warn(
        "an incremental market data entry has no supported update action and "
        "can not be applied: {}",
        entry);
    return EntryOutcome::Unmatched;
  }

  switch (entry.action->value()) {
    case MarketEntryAction::Option::New:
      return insert(entry);
    case MarketEntryAction::Option::Change:
      return change(entry);
    case MarketEntryAction::Option::Delete:
      return remove(entry);
  }
  return EntryOutcome::Skipped;
}

auto MarketDataBookCache::Book::trim() -> void {
  if (depth_ == AllDepthLevels) {
    return;
  }
  trim_side(bids_, depth_);
  trim_side(offers_, depth_);
}

auto MarketDataBookCache::Book::make_snapshot(
    const protocol::MarketDataUpdate& update) const
    -> protocol::MarketDataSnapshot {
  protocol::MarketDataSnapshot snapshot{update.session};
  snapshot.instrument = instrument_;
  snapshot.request_id = update.request_id;
  snapshot.market_data_entries.reserve(bids_.size() + offers_.size());
  snapshot.market_data_entries.insert(
      snapshot.market_data_entries.end(), bids_.begin(), bids_.end());
  snapshot.market_data_entries.insert(
      snapshot.market_data_entries.end(), offers_.begin(), offers_.end());
  return snapshot;
}

auto MarketDataBookCache::Book::insert(MarketDataEntry entry) -> EntryOutcome {
  std::vector<MarketDataEntry>* const side = side_of(entry);
  if (side == nullptr) {
    log::warn(
        "a new market data entry has no entry type and can not be applied: {}",
        entry);
    return EntryOutcome::Unmatched;
  }

  // An identifier is unique across both sides of the book, and a level
  // without identifier is unique on its side.
  if (entry.id.has_value()) {
    const auto has_same_id = [&entry](const MarketDataEntry& candidate) {
      return candidate.id == entry.id;
    };
    std::erase_if(bids_, has_same_id);
    std::erase_if(offers_, has_same_id);
  } else {
    std::erase_if(*side, [&entry](const MarketDataEntry& candidate) {
      return matches(candidate, entry);
    });
  }

  entry.action.reset();
  const auto position =
      std::ranges::find_if(*side, [&entry](const MarketDataEntry& candidate) {
        return precedes(entry, candidate);
      });
  side->insert(position, std::move(entry));
  return EntryOutcome::Applied;
}

auto MarketDataBookCache::Book::change(const MarketDataEntry& entry)
    -> EntryOutcome {
  const std::optional<Location> location = locate(entry);
  if (!location.has_value()) {
    if (!entry.type.has_value() || !entry.price.has_value()) {
      log::warn(
          "a changed market data entry is not in the book and has no entry "
          "type or price to be added with, it can not be applied: {}",
          entry);
      return EntryOutcome::Unmatched;
    }
    log::debug(
        "adding a changed market data entry, which is not in the book, as a "
        "new entry: {}",
        entry);
    return insert(entry);
  }

  MarketDataEntry& stored = *location->entry;
  const bool moves =
      (entry.price.has_value() && entry.price != stored.price) ||
      (entry.type.has_value() && entry.type != stored.type);
  if (!moves) {
    merge_into(stored, entry);
    return EntryOutcome::Applied;
  }

  auto changed = std::move(stored);
  location->side->erase(location->entry);
  merge_into(changed, entry);
  return insert(std::move(changed));
}

auto MarketDataBookCache::Book::remove(const MarketDataEntry& entry)
    -> EntryOutcome {
  const std::optional<Location> location = locate(entry);
  if (!location.has_value()) {
    log::debug(
        "ignoring a deleted market data entry, which is not in the book: {}",
        entry);
    return EntryOutcome::Skipped;
  }

  location->side->erase(location->entry);
  return EntryOutcome::Applied;
}

auto MarketDataBookCache::Book::locate(const MarketDataEntry& entry)
    -> std::optional<Location> {
  const auto find_in =
      [&entry](std::vector<MarketDataEntry>& side) -> std::optional<Location> {
    const auto found =
        std::ranges::find_if(side, [&entry](const MarketDataEntry& candidate) {
          return matches(candidate, entry);
        });
    if (found == side.end()) {
      return std::nullopt;
    }
    return std::make_optional<Location>(&side, found);
  };

  // An identifier is never reused by the other side, so it alone identifies
  // the entry, whatever entry type the update carries.
  if (entry.id.has_value()) {
    if (auto location = find_in(bids_)) {
      return location;
    }
    return find_in(offers_);
  }
  if (std::vector<MarketDataEntry>* const side = side_of(entry)) {
    return find_in(*side);
  }
  return std::nullopt;
}

auto MarketDataBookCache::Book::side_of(const MarketDataEntry& entry)
    -> std::vector<MarketDataEntry>* {
  if (entry.type == MdEntryType::Option::Bid) {
    return &bids_;
  }
  if (entry.type == MdEntryType::Option::Offer) {
    return &offers_;
  }
  return nullptr;
}

auto MarketDataBookCache::track(const MdRequestId& request_id,
                                MarketDepth depth) -> void {
  const std::lock_guard lock{mutex_};
  books_.insert_or_assign(
      request_id,
      TrackedBook{.depth = depth, .book = std::nullopt, .diverged = false});
}

auto MarketDataBookCache::reset(const MdRequestId& request_id) -> void {
  const std::lock_guard lock{mutex_};
  books_.erase(request_id);
}

auto MarketDataBookCache::process(const protocol::MarketDataSnapshot& snapshot)
    -> bool {
  if (!snapshot.request_id.has_value() || !from_fix_feed(snapshot)) {
    return true;
  }

  const std::lock_guard lock{mutex_};
  const auto tracked = books_.find(*snapshot.request_id);
  if (tracked == books_.end()) {
    return true;
  }

  if (tracked->second.diverged) {
    log::debug(
        "ignoring a market data full refresh of the `{}' subscription, whose "
        "book has diverged from the feed",
        *snapshot.request_id);
    return false;
  }

  tracked->second.book.emplace(
      snapshot.instrument, snapshot.market_data_entries, tracked->second.depth);
  log::debug("cached the order book of the `{}' market data subscription",
             *snapshot.request_id);
  return true;
}

auto MarketDataBookCache::process(const protocol::MarketDataUpdate& update)
    -> UpdateOutcome {
  if (!update.request_id.has_value()) {
    log::debug(
        "ignoring a market data incremental refresh without a request "
        "identifier");
    return {};
  }
  const MdRequestId& request_id = *update.request_id;

  const std::lock_guard lock{mutex_};
  const auto found = books_.find(request_id);
  if (found == books_.end()) {
    log::debug(
        "ignoring a market data incremental refresh of the `{}' subscription, "
        "which is not, or no longer, an incremental subscription of the "
        "generator",
        request_id);
    return {};
  }
  TrackedBook& tracked = found->second;

  if (tracked.diverged) {
    log::debug(
        "ignoring a market data incremental refresh of the `{}' subscription, "
        "whose book has diverged from the feed",
        request_id);
    return {};
  }

  if (!tracked.book.has_value()) {
    log::warn(
        "ignoring a market data incremental refresh of the `{}' subscription, "
        "its full refresh has not been received yet",
        request_id);
    return {};
  }

  bool changed = false;
  for (const MarketDataEntry& entry : update.market_data_entries) {
    const EntryOutcome outcome = tracked.book->apply(entry);
    if (outcome == EntryOutcome::Unmatched) {
      tracked.book.reset();
      tracked.diverged = true;
      return UpdateOutcome{.snapshot = std::nullopt, .diverged = true};
    }
    changed = changed || outcome == EntryOutcome::Applied;
  }

  if (!changed) {
    return {};
  }

  // Trimmed only after the whole update, as a level one entry pushes beyond
  // the depth can be brought back by a later entry, which the feed then does
  // not send again.
  tracked.book->trim();
  return UpdateOutcome{.snapshot = tracked.book->make_snapshot(update),
                       .diverged = false};
}

}  // namespace simulator::generator::mdata
