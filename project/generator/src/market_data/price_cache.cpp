#include "ih/market_data/price_cache.hpp"

#include <cstdint>
#include <mutex>
#include <set>
#include <span>

#include "log/logging.hpp"

namespace simulator::generator::mdata {

auto PriceCache::TopOfBook::replace(const std::vector<MarketDataEntry>& entries)
    -> void {
  clear();

  std::set<Price> bids;
  std::set<Price> offers;
  for (const MarketDataEntry& entry : entries) {
    if (!entry.price.has_value()) {
      continue;
    }
    if (entry.type == MdEntryType::Option::Bid) {
      bids.insert(*entry.price);
    } else if (entry.type == MdEntryType::Option::Offer) {
      offers.insert(*entry.price);
    }
  }

  if (!bids.empty()) {
    best_bid_price_ = *bids.rbegin();
  }
  if (!offers.empty()) {
    best_offer_price_ = *offers.begin();
  }
  bid_depth_levels_ = static_cast<std::uint32_t>(bids.size());
  offer_depth_levels_ = static_cast<std::uint32_t>(offers.size());
}

auto PriceCache::TopOfBook::clear() -> void {
  best_bid_price_.reset();
  best_offer_price_.reset();
  bid_depth_levels_ = 0;
  offer_depth_levels_ = 0;
}

auto PriceCache::TopOfBook::state() const -> MarketState {
  MarketState state;

  if (best_bid_price_.has_value()) {
    state.best_bid_price = best_bid_price_->value();
  }
  if (best_offer_price_.has_value()) {
    state.best_offer_price = best_offer_price_->value();
  }

  state.bid_depth_levels = bid_depth_levels_;
  state.offer_depth_levels = offer_depth_levels_;

  return state;
}

auto PriceCache::add(const MdRequestId& request_id) -> void {
  const std::unique_lock lock{mutex_};
  books_.try_emplace(request_id);
}

auto PriceCache::process(const protocol::MarketDataSnapshot& snapshot) -> void {
  const std::unique_lock lock{mutex_};
  if (TopOfBook* const book = find_to_modify(snapshot.request_id)) {
    book->replace(snapshot.market_data_entries);
  }
}

auto PriceCache::reset(const MdRequestId& request_id) -> void {
  const std::unique_lock lock{mutex_};
  if (const auto book = books_.find(request_id); book != books_.end()) {
    book->second.clear();
  }
}

auto PriceCache::market_state(const MdRequestId& order_book,
                              std::span<const MdRequestId> external) const
    -> MarketState {
  MarketState result;
  const std::shared_lock lock{mutex_};

  if (const auto book = books_.find(order_book); book != books_.end()) {
    result = book->second.state();
  }

  auto& best_bid_price = result.best_bid_price;
  auto& best_offer_price = result.best_offer_price;
  for (const MdRequestId& request_id : external) {
    const auto book = books_.find(request_id);
    if (book == books_.end()) {
      continue;
    }

    const MarketState state = book->second.state();
    if (state.best_bid_price.has_value() &&
        (!best_bid_price.has_value() ||
         *state.best_bid_price > *best_bid_price)) {
      best_bid_price = state.best_bid_price;
    }
    if (state.best_offer_price.has_value() &&
        (!best_offer_price.has_value() ||
         *state.best_offer_price < *best_offer_price)) {
      best_offer_price = state.best_offer_price;
    }
  }

  return result;
}

auto PriceCache::find_to_modify(const std::optional<MdRequestId>& request_id)
    -> TopOfBook* {
  if (!request_id.has_value()) {
    log::warn("ignoring market data without a subscription identifier");
    return nullptr;
  }

  const auto book = books_.find(*request_id);
  if (book == books_.end()) {
    log::debug(
        "ignoring market data of the `{}' subscription, which is not used as a "
        "random order generation price source",
        *request_id);
    return nullptr;
  }

  return &book->second;
}

}  // namespace simulator::generator::mdata
