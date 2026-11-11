#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_AMENDMENT_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_AMENDMENT_HPP_

#include <gsl/pointers>
#include <optional>

#include "common/attributes.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"

namespace simulator::trading_system::matching_engine {

// Amends an order during the auction call without crossing; an order found
// only among orders of the other type is converted and rests anew.
class AuctionAmendment : private EventReporter {
 public:
  AuctionAmendment(EventListener& event_listener,
                   OrderBook& order_book,
                   std::optional<PriceTick> price_tick);

  AuctionAmendment(const AuctionAmendment&) = default;
  AuctionAmendment(AuctionAmendment&&) = default;
  ~AuctionAmendment() override = default;

  auto operator=(const AuctionAmendment&) -> AuctionAmendment& = delete;
  auto operator=(AuctionAmendment&&) -> AuctionAmendment& = delete;

  auto operator()(LimitUpdate update) -> OrderBookUpdates;

  auto operator()(MarketUpdate update) -> OrderBookUpdates;

 private:
  auto rest_as_limit_order(LimitUpdate update, OrderPage& page)
      -> OrderBookUpdates;

  auto rest_as_market_order(MarketUpdate update, OrderPage& page)
      -> OrderBookUpdates;

  gsl::not_null<OrderBook*> order_book_;
  std::optional<PriceTick> price_tick_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_AMENDMENT_HPP_
