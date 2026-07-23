#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_MARKET_AMENDMENT_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_MARKET_AMENDMENT_HPP_

#include <optional>

#include "common/attributes.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_updates.hpp"

namespace simulator::trading_system::matching_engine {

class MarketAmendment : private EventReporter {
 public:
  MarketAmendment(EventListener& event_listener,
                  OrderBook& order_book,
                  std::optional<PriceTick> price_tick);

  MarketAmendment(const MarketAmendment&) = default;
  MarketAmendment(MarketAmendment&&) = default;
  ~MarketAmendment() override = default;

  auto operator=(const MarketAmendment&) -> MarketAmendment& = delete;
  auto operator=(MarketAmendment&&) -> MarketAmendment& = delete;

  auto operator()(MarketUpdate update) -> void;

 private:
  auto amend_order(MarketUpdate update, OrderPage& page) -> void;

  OrderBook& order_book_;
  std::optional<PriceTick> price_tick_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_MARKET_AMENDMENT_HPP_
