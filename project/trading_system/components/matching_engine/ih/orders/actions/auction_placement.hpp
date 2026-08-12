#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_PLACEMENT_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_PLACEMENT_HPP_

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"

namespace simulator::trading_system::matching_engine {

class AuctionPlacement : private EventReporter {
 public:
  AuctionPlacement(EventListener& event_listener, OrderBook& order_book);

  AuctionPlacement(const AuctionPlacement&) = default;
  AuctionPlacement(AuctionPlacement&&) = default;
  ~AuctionPlacement() override = default;

  auto operator=(const AuctionPlacement&) -> AuctionPlacement& = delete;
  auto operator=(AuctionPlacement&&) -> AuctionPlacement& = delete;

  auto operator()(LimitOrder order) -> OrderBookUpdates;

  auto operator()(MarketOrder order) -> OrderBookUpdates;

 private:
  OrderBook& order_book_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_PLACEMENT_HPP_
