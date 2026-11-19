#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_MARKET_AMENDMENT_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_MARKET_AMENDMENT_HPP_

#include <gsl/pointers>

#include "core/tools/price_tick_ranges.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"

namespace simulator::trading_system::matching_engine {

class MarketAmendment : private EventReporter {
 public:
  MarketAmendment(EventListener& event_listener,
                  OrderBook& order_book,
                  const core::PriceTickRanges& price_tick_ranges);

  MarketAmendment(const MarketAmendment&) = default;
  MarketAmendment(MarketAmendment&&) = default;
  ~MarketAmendment() override = default;

  auto operator=(const MarketAmendment&) -> MarketAmendment& = delete;
  auto operator=(MarketAmendment&&) -> MarketAmendment& = delete;

  auto operator()(MarketUpdate update) -> OrderBookUpdates;

 private:
  auto amend_order(MarketUpdate update, OrderPage& page) -> OrderBookUpdates;

  OrderBook& order_book_;
  gsl::not_null<const core::PriceTickRanges*> price_tick_ranges_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_MARKET_AMENDMENT_HPP_
