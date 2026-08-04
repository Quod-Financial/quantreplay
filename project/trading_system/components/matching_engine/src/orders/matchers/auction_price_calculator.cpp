#include "ih/orders/matchers/auction_price_calculator.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <optional>
#include <ranges>
#include <utility>
#include <vector>

#include "core/common/unreachable.hpp"

namespace simulator::trading_system::matching_engine {

namespace {

[[nodiscard]]
auto is_buy_side(Side side) -> bool {
  switch (static_cast<Side::Option>(side)) {
    case Side::Option::Buy:
      return true;
    case Side::Option::Sell:
    case Side::Option::SellShort:
    case Side::Option::SellShortExempt:
      return false;
  }

  core::unreachable();
}

[[nodiscard]]
auto best_limit_price(const LimitOrdersContainer& limits)
    -> std::optional<double> {
  return limits.empty()
             ? std::nullopt
             : std::make_optional(static_cast<double>(limits.begin()->price()));
}

[[nodiscard]]
auto crossing_prices(const OrderBook& book)
    -> std::optional<std::pair<double, double>> {
  const auto best_bid = best_limit_price(book.buy_page().limit_orders());
  const auto best_offer = best_limit_price(book.sell_page().limit_orders());

  if (!best_bid.has_value() || !best_offer.has_value() ||
      *best_bid < *best_offer) {
    return std::nullopt;
  }

  return std::optional<std::pair<double, double>>{
      std::in_place, *best_bid, *best_offer};
}

[[nodiscard]]
auto total_market_quantity(const OrderPage& page) -> double {
  double quantity = 0.0;
  for (const auto& order : page.market_orders()) {
    quantity += order.leaves_quantity().value();
  }
  return quantity;
}

[[nodiscard]]
auto total_limit_quantity_same_price(LimitOrdersContainer::const_iterator& iter,
                                     LimitOrdersContainer::const_iterator end)
    -> double {
  double cumulative_qty = iter->leaves_quantity().value();

  const auto same_price = iter->price();
  ++iter;

  for (; iter != end && iter->price() == same_price; ++iter) {
    cumulative_qty += iter->leaves_quantity().value();
  }

  return cumulative_qty;
}

[[nodiscard]]
auto limit_order_exists_at(const LimitOrdersContainer& limits,
                           Side side,
                           double price) -> bool {
  const auto iter =
      is_buy_side(side)
          ? std::lower_bound(limits.begin(),
                             limits.end(),
                             price,
                             [](const LimitOrder& order, double threshold) {
                               return static_cast<double>(order.price()) >
                                      threshold;
                             })
          : std::lower_bound(limits.begin(),
                             limits.end(),
                             price,
                             [](const LimitOrder& order, double threshold) {
                               return static_cast<double>(order.price()) <
                                      threshold;
                             });

  return iter != limits.end() && static_cast<double>(iter->price()) == price;
}

}  // namespace

AuctionPriceCalculator::AuctionPriceCalculator(
    const OrderBook& book, std::optional<Price> reference_price)
    : order_book_{book},
      price_levels_{},
      reference_price_{std::move(reference_price)},
      auction_result_{} {
  rebuild();
}

auto AuctionPriceCalculator::process(const OrderBookUpdates& updates) -> void {
  if (!can_update_incrementally()) {
    rebuild();
    return;
  }

  bool touched = false;
  for (const auto& update : updates) {
    touched |= apply_update(update);
  }

  if (touched) {
    select_indicative_price();
  }
}

auto AuctionPriceCalculator::auction_result() const
    -> std::optional<AuctionResult> {
  return auction_result_;
}

auto AuctionPriceCalculator::rebuild() -> void {
  price_levels_.clear();
  auction_result_.reset();

  const auto crossing = crossing_prices(order_book_.get());
  if (!crossing.has_value()) {
    return;
  }

  const auto [best_bid, best_offer] = *crossing;
  fill_price_candidates(best_bid, best_offer);
  accumulate_quantity();
  select_indicative_price();
}

auto AuctionPriceCalculator::fill_price_candidates(double best_bid,
                                                   double best_offer) -> void {
  auto collect = [&](const OrderPage& page) {
    for (const auto& order : page.limit_orders()) {
      const auto candidate = order.price().value();
      if (candidate < best_offer || candidate > best_bid) {
        return;
      }
      price_levels_.emplace(candidate, PriceLevel{});
    }
  };

  collect(order_book_.get().buy_page());
  collect(order_book_.get().sell_page());
}

auto AuctionPriceCalculator::accumulate_quantity() -> void {
  accumulate_quantity_buy();
  accumulate_quantity_sell();
  recompute_range(price_levels_.begin(), price_levels_.end());
}

template <typename DataIterator, typename Comparator, typename MemberAccessor>
auto AuctionPriceCalculator::accumulate_limit_quantity(
    DataIterator data_begin,
    DataIterator data_end,
    LimitOrdersContainer::const_iterator orders_begin,
    LimitOrdersContainer::const_iterator orders_end,
    double initial_quantity,
    Comparator price_is_eligible,
    MemberAccessor cumulative_member) -> void {
  double cumulative_qty = initial_quantity;
  auto order_iter = orders_begin;

  auto data_iter = data_begin;
  for (; data_iter != data_end && order_iter != orders_end; ++data_iter) {
    if (price_is_eligible(order_iter->price().value(), data_iter->first)) {
      cumulative_qty += total_limit_quantity_same_price(order_iter, orders_end);
    }
    cumulative_member(data_iter->second) = cumulative_qty;
  }

  for (; data_iter != data_end; ++data_iter) {
    cumulative_member(data_iter->second) = cumulative_qty;
  }
}

auto AuctionPriceCalculator::accumulate_quantity_buy() -> void {
  const auto& buy_limit_orders = order_book_.get().buy_page().limit_orders();
  accumulate_limit_quantity(
      price_levels_.rbegin(),
      price_levels_.rend(),
      buy_limit_orders.begin(),
      buy_limit_orders.end(),
      total_market_quantity(order_book_.get().buy_page()),
      [](double order_price, double candidate_price) {
        return order_price >= candidate_price;
      },
      std::mem_fn(&PriceLevel::cumulative_qty_buy));
}

auto AuctionPriceCalculator::accumulate_quantity_sell() -> void {
  const auto& sell_limit_orders = order_book_.get().sell_page().limit_orders();
  accumulate_limit_quantity(
      price_levels_.begin(),
      price_levels_.end(),
      sell_limit_orders.begin(),
      sell_limit_orders.end(),
      total_market_quantity(order_book_.get().sell_page()),
      [](double order_price, double candidate_price) {
        return order_price <= candidate_price;
      },
      std::mem_fn(&PriceLevel::cumulative_qty_sell));
}

auto AuctionPriceCalculator::can_update_incrementally() const -> bool {
  if (price_levels_.empty()) {
    return false;
  }

  const auto crossing = crossing_prices(order_book_.get());
  if (!crossing.has_value()) {
    return false;
  }

  const auto [best_bid, best_offer] = *crossing;
  return best_bid == price_levels_.rbegin()->first &&
         best_offer == price_levels_.begin()->first;
}

auto AuctionPriceCalculator::apply_update(const OrderBookUpdate& update)
    -> bool {
  const bool removal = update.action == OrderBookUpdate::Action::Remove;
  const double quantity_delta =
      removal ? -update.quantity.value() : update.quantity.value();

  if (!update.price.has_value()) {
    apply_market_delta(update.side, quantity_delta);
    return true;
  }

  return apply_limit_delta(update.side, *update.price, quantity_delta, removal);
}

auto AuctionPriceCalculator::apply_market_delta(Side side,
                                                double quantity_delta) -> void {
  const bool buy = is_buy_side(side);
  const auto member = buy ? std::mem_fn(&PriceLevel::cumulative_qty_buy)
                          : std::mem_fn(&PriceLevel::cumulative_qty_sell);
  for (auto& entry : price_levels_ | std::views::values) {
    member(entry) += quantity_delta;
  }

  recompute_range(price_levels_.begin(), price_levels_.end());
}

auto AuctionPriceCalculator::apply_limit_delta(Side side,
                                               OrderPrice price,
                                               double quantity_delta,
                                               bool might_vanish) -> bool {
  const double key = static_cast<double>(price);
  if (key < price_levels_.begin()->first ||
      key > price_levels_.rbegin()->first) {
    return false;
  }

  auto pos = price_levels_.find(key);
  if (pos == price_levels_.end()) {
    pos = price_levels_.emplace(key, PriceLevel{}).first;
    pos->second.cumulative_qty_buy = std::next(pos)->second.cumulative_qty_buy;
    pos->second.cumulative_qty_sell =
        std::prev(pos)->second.cumulative_qty_sell;
  }

  if (is_buy_side(side)) {
    const auto range_end = std::next(pos);
    for (auto iter = price_levels_.begin(); iter != range_end; ++iter) {
      iter->second.cumulative_qty_buy += quantity_delta;
    }
    recompute_range(price_levels_.begin(), range_end);
  } else {
    for (auto iter = pos; iter != price_levels_.end(); ++iter) {
      iter->second.cumulative_qty_sell += quantity_delta;
    }
    recompute_range(pos, price_levels_.end());
  }

  if (might_vanish && !has_limit_order_at(price)) {
    price_levels_.erase(pos);
  }

  return true;
}

auto AuctionPriceCalculator::has_limit_order_at(OrderPrice price) const
    -> bool {
  const double threshold = static_cast<double>(price);
  return limit_order_exists_at(order_book_.get().buy_page().limit_orders(),
                               Side::Option::Buy,
                               threshold) ||
         limit_order_exists_at(order_book_.get().sell_page().limit_orders(),
                               Side::Option::Sell,
                               threshold);
}

auto AuctionPriceCalculator::recompute_range(CandidateIterator first,
                                             CandidateIterator last) -> void {
  for (auto iter = first; iter != last; ++iter) {
    auto& [cumulative_qty_buy, cumulative_qty_sell, tradable_qty, imbalance] =
        iter->second;
    tradable_qty = std::min(cumulative_qty_buy, cumulative_qty_sell);
    imbalance = cumulative_qty_buy - cumulative_qty_sell;
  }
}

auto AuctionPriceCalculator::select_indicative_price() -> void {
  if (price_levels_.empty()) {
    auction_result_.reset();
    return;
  }

  const auto candidates = select_by_max_tradable_quantity();
  if (candidates.size() == 1) {
    auction_result_ = make_result(candidates.front());
    return;
  }

  auto [tied_candidates, same_sign] =
      select_by_min_absolute_imbalance(candidates);
  if (tied_candidates.size() == 1) {
    auction_result_ = make_result(tied_candidates.front());
    return;
  }

  // A zero imbalance gives the sign rule nothing to choose by, so a defined
  // reference price decides instead.
  const bool balanced = tied_candidates.front()->second.imbalance == 0;
  if (reference_price_.has_value() && (balanced || !same_sign)) {
    auction_result_ = make_result(select_by_reference_price(tied_candidates));
    return;
  }

  if (same_sign && !balanced) {
    auction_result_ = make_result(select_by_imbalance_sign(tied_candidates));
    return;
  }

  auction_result_ = make_result(tied_candidates.front());
}

auto AuctionPriceCalculator::select_by_max_tradable_quantity() -> Candidates {
  Candidates candidates;
  double max_tradable = std::numeric_limits<double>::lowest();

  for (auto iter = price_levels_.begin(); iter != price_levels_.end(); ++iter) {
    if (const auto& tradable_qty = iter->second.tradable_qty;
        candidates.empty() || tradable_qty > max_tradable) {
      max_tradable = tradable_qty;
      candidates.clear();
      candidates.push_back(iter);
    } else if (tradable_qty == max_tradable) {
      candidates.push_back(iter);
    }
  }

  return candidates;
}

auto AuctionPriceCalculator::select_by_min_absolute_imbalance(
    const Candidates& candidates) -> ImbalanceTieBreak {
  Candidates tied_candidates;
  bool same_sign = true;
  double min_abs_imbalance = std::numeric_limits<double>::max();

  for (auto iter : candidates) {
    const double imbalance = iter->second.imbalance;
    const double abs_imbalance = std::abs(imbalance);
    if (tied_candidates.empty() || abs_imbalance < min_abs_imbalance) {
      min_abs_imbalance = abs_imbalance;
      tied_candidates.clear();
      tied_candidates.push_back(iter);
      same_sign = true;
    } else if (abs_imbalance == min_abs_imbalance) {
      same_sign = same_sign &&
                  std::signbit(tied_candidates.front()->second.imbalance) ==
                      std::signbit(imbalance);
      tied_candidates.push_back(iter);
    }
  }

  return ImbalanceTieBreak{.candidates = std::move(tied_candidates),
                           .same_sign = same_sign};
}

auto AuctionPriceCalculator::select_by_imbalance_sign(
    const Candidates& candidates) -> CandidateIterator {
  // negative imbalance -> lowest price candidate
  // positive imbalance -> highest price candidate
  const bool negative_imbalance = candidates.front()->second.imbalance < 0;
  return negative_imbalance ? candidates.front() : candidates.back();
}

auto AuctionPriceCalculator::select_by_reference_price(
    const Candidates& candidates) -> CandidateIterator {
  auto closest = candidates.front();
  double min_abs_diff = std::numeric_limits<double>::max();

  for (auto iter : candidates) {
    if (const auto abs_diff = std::abs(iter->first - reference_price_->value());
        abs_diff < min_abs_diff) {
      min_abs_diff = abs_diff;
      closest = iter;
    }
  }

  return closest;
}

auto AuctionPriceCalculator::make_result(CandidateIterator iter) const
    -> AuctionResult {
  const auto& [price, level] = *iter;
  return AuctionResult{
      .price = Price{price},
      .quantity = Quantity{level.tradable_qty},
      .imbalance = Quantity{std::abs(level.imbalance)},
      .imbalance_side = resolve_imbalance_side(level.imbalance)};
}

auto AuctionPriceCalculator::resolve_imbalance_side(double imbalance) const
    -> TradeCondition {
  using Option = TradeCondition::Option;

  if (imbalance > 0) {
    return Option::ImbalanceMoreBuyers;
  }
  if (imbalance < 0) {
    return Option::ImbalanceMoreSellers;
  }

  double max_buy_surplus = 0;
  double max_sell_surplus = 0;
  for (const auto& level : price_levels_ | std::views::values) {
    max_buy_surplus = std::max(max_buy_surplus, level.imbalance);
    max_sell_surplus = std::max(max_sell_surplus, -level.imbalance);
  }

  return max_sell_surplus > max_buy_surplus ? Option::ImbalanceMoreSellers
                                            : Option::ImbalanceMoreBuyers;
}

}  // namespace simulator::trading_system::matching_engine
