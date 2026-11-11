#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_LIMIT_TO_MARKET_AMENDMENT_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_LIMIT_TO_MARKET_AMENDMENT_HPP_

#include <gsl/pointers>
#include <optional>

#include "common/attributes.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "ih/orders/matchers/order_matcher.hpp"

namespace simulator::trading_system::matching_engine {

// Executes a resting limit order amended to market as a new market order.
class LimitToMarketAmendment : private EventReporter {
 public:
  LimitToMarketAmendment(EventListener& event_listener,
                         OrderBook& order_book,
                         RegularMatcher& matcher,
                         std::optional<PriceTick> price_tick);

  LimitToMarketAmendment(const LimitToMarketAmendment&) = default;
  LimitToMarketAmendment(LimitToMarketAmendment&&) = default;
  ~LimitToMarketAmendment() override = default;

  auto operator=(const LimitToMarketAmendment&)
      -> LimitToMarketAmendment& = delete;
  auto operator=(LimitToMarketAmendment&&) -> LimitToMarketAmendment& = delete;

  auto operator()(MarketUpdate update) -> OrderBookUpdates;

 private:
  auto execute(MarketOrder& order) -> void;

  gsl::not_null<OrderBook*> order_book_;
  gsl::not_null<RegularMatcher*> matcher_;
  std::optional<PriceTick> price_tick_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_LIMIT_TO_MARKET_AMENDMENT_HPP_
