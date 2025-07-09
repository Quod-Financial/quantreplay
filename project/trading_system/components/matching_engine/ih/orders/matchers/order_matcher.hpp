#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_ORDER_MATCHER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_ORDER_MATCHER_HPP_

#include <cstdint>

#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"

namespace simulator::trading_system::matching_engine {

class RegularMatcher {
 public:
  RegularMatcher() = default;
  RegularMatcher(const RegularMatcher&) = default;
  RegularMatcher(RegularMatcher&&) = default;
  virtual ~RegularMatcher() = default;

  auto operator=(const RegularMatcher&) -> RegularMatcher& = default;
  auto operator=(RegularMatcher&&) -> RegularMatcher& = default;

  virtual auto match(LimitOrder& taker) -> void = 0;

  virtual auto match(MarketOrder& taker) -> void = 0;

  virtual auto has_facing_orders(const LimitOrder& taker) -> bool = 0;

  virtual auto has_facing_orders(const MarketOrder& taker) -> bool = 0;

  virtual auto can_fully_trade(const LimitOrder& taker) -> bool = 0;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_ORDER_MATCHER_HPP_
