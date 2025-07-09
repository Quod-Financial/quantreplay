#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_REGULAR_PLACEMENT_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_REGULAR_PLACEMENT_HPP_

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/matchers/order_matcher.hpp"

namespace simulator::trading_system::matching_engine {

class RegularPlacement : private EventReporter {
 public:
  RegularPlacement(EventListener& event_listener,
                   OrderBook& order_book,
                   RegularMatcher& matcher);

  RegularPlacement(const RegularPlacement&) = default;
  RegularPlacement(RegularPlacement&&) = default;
  ~RegularPlacement() override = default;

  auto operator=(const RegularPlacement&) -> RegularPlacement& = delete;
  auto operator=(RegularPlacement&&) -> RegularPlacement& = delete;

  auto operator()(LimitOrder order) -> void;

  auto operator()(MarketOrder order) -> void;

 private:
  auto place_order(LimitOrder order) -> void;

  auto match_ioc_order(LimitOrder order) -> void;

  auto match_fok_order(LimitOrder order) -> void;

  OrderBook& order_book_;
  RegularMatcher& matcher_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_REGULAR_PLACEMENT_HPP_
