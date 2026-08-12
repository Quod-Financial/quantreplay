#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_AUCTION_PRICE_CALCULATOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_AUCTION_PRICE_CALCULATOR_HPP_

#include <functional>
#include <map>
#include <optional>
#include <vector>

#include "core/domain/attributes.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"

namespace simulator::trading_system::matching_engine {

struct AuctionResult {
  Price price;
  Quantity quantity;
  Quantity imbalance;
  TradeCondition imbalance_side;

  [[nodiscard]]
  auto operator==(const AuctionResult&) const -> bool = default;
};

class AuctionPriceCalculator {
 public:
  explicit AuctionPriceCalculator(const OrderBook& book,
                                  std::optional<Price> reference_price);

  auto process(const OrderBookUpdates& updates) -> void;

  [[nodiscard]]
  auto auction_result() const -> std::optional<AuctionResult>;

 private:
  struct PriceLevel {
    double cumulative_qty_buy{0};
    double cumulative_qty_sell{0};
    double tradable_qty{0};
    double imbalance{0};
  };

  using CandidateIterator = std::map<double, PriceLevel>::iterator;
  using Candidates = std::vector<CandidateIterator>;

  struct ImbalanceTieBreak {
    Candidates candidates;
    bool same_sign{true};
  };

  auto rebuild() -> void;

  auto fill_price_candidates(double best_bid, double best_offer) -> void;

  auto accumulate_quantity() -> void;

  auto accumulate_quantity_buy() -> void;
  auto accumulate_quantity_sell() -> void;

  template <typename DataIterator, typename Comparator, typename MemberAccessor>
  auto accumulate_limit_quantity(
      DataIterator data_begin,
      DataIterator data_end,
      LimitOrdersContainer::const_iterator orders_begin,
      LimitOrdersContainer::const_iterator orders_end,
      double initial_quantity,
      Comparator price_is_eligible,
      MemberAccessor cumulative_member) -> void;

  [[nodiscard]]
  auto can_update_incrementally() const -> bool;

  [[nodiscard]]
  auto apply_update(const OrderBookUpdate& update) -> bool;

  auto apply_market_delta(Side side, double quantity_delta) -> void;

  [[nodiscard]]
  auto apply_limit_delta(Side side,
                         OrderPrice price,
                         double quantity_delta,
                         bool might_vanish) -> bool;

  [[nodiscard]]
  auto has_limit_order_at(OrderPrice price) const -> bool;

  static auto recompute_range(CandidateIterator first, CandidateIterator last)
      -> void;

  auto select_indicative_price() -> void;

  [[nodiscard]]
  auto select_by_max_tradable_quantity() -> Candidates;

  [[nodiscard]]
  auto select_by_min_absolute_imbalance(const Candidates& candidates)
      -> ImbalanceTieBreak;

  [[nodiscard]]
  auto select_by_imbalance_sign(const Candidates& candidates)
      -> CandidateIterator;

  [[nodiscard]]
  auto select_by_reference_price(const Candidates& candidates)
      -> CandidateIterator;

  [[nodiscard]]
  auto make_result(CandidateIterator iter) const -> AuctionResult;

  [[nodiscard]]
  auto resolve_imbalance_side(double imbalance) const -> TradeCondition;

  std::reference_wrapper<const OrderBook> order_book_;
  std::map<double, PriceLevel> price_levels_;
  std::optional<Price> reference_price_;
  std::optional<AuctionResult> auction_result_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_AUCTION_PRICE_CALCULATOR_HPP_
