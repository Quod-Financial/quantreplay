#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_NO_CROSS_MATCHER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_NO_CROSS_MATCHER_HPP_

#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/matchers/order_matcher.hpp"

namespace simulator::trading_system::matching_engine {

class NoCrossMatcher final : public RegularMatcher {
 public:
  auto match(LimitOrder& /*taker*/) -> void override {}

  auto match(MarketOrder& /*taker*/) -> void override {}

  auto has_facing_orders(const LimitOrder& /*taker*/) -> bool override {
    return false;
  }

  auto has_facing_orders(const MarketOrder& /*taker*/) -> bool override {
    return false;
  }

  auto can_fully_trade(const LimitOrder& /*taker*/) -> bool override {
    return false;
  }
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_NO_CROSS_MATCHER_HPP_
