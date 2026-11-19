#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_UNCROSS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_UNCROSS_HPP_

#include <gsl/pointers>
#include <optional>

#include "core/domain/attributes.hpp"
#include "core/domain/market_phase.hpp"
#include "core/tools/price_tick_ranges.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/matchers/auction_price_calculator.hpp"

namespace simulator::trading_system::matching_engine::order {

class AuctionUncross : EventReporter {
 public:
  AuctionUncross(EventListener& event_listener,
                 MarketPhase auction_phase,
                 const core::PriceTickRanges& price_tick_ranges);

  auto operator()(OrderBook& book,
                  const std::optional<AuctionResult>& auction_result) const
      -> void;

 private:
  auto cross_book(OrderBook& book, const AuctionResult& result) const -> void;

  template <typename BuyOrderIt, typename SellOrderIt>
  auto cross_sequences(BuyOrderIt buy,
                       BuyOrderIt buy_end,
                       SellOrderIt sell,
                       SellOrderIt sell_end,
                       ExecutionPrice price) const -> void;

  template <typename BuyOrder, typename SellOrder>
  auto cross(BuyOrder& buy_order,
             SellOrder& sell_order,
             ExecutionPrice price) const -> void;

  auto clear_market_orders(MarketOrdersContainer& orders) const -> void;

  static auto remove_filled_limit_orders(OrderBook& book) -> void;

  MarketPhase auction_phase_;
  gsl::not_null<const core::PriceTickRanges*> price_tick_ranges_;
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_UNCROSS_HPP_
