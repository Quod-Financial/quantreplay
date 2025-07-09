#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_REGULAR_ORDER_MATCHER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_REGULAR_ORDER_MATCHER_HPP_

#include <functional>

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/matchers/order_matcher.hpp"

namespace simulator::trading_system::matching_engine {

class RegularOrderMatcher : public RegularMatcher, private EventReporter {
 public:
  RegularOrderMatcher(EventListener& event_listener, OrderBook& order_book);

  RegularOrderMatcher(const RegularOrderMatcher&) = default;
  RegularOrderMatcher(RegularOrderMatcher&&) = default;
  ~RegularOrderMatcher() override = default;

  auto operator=(const RegularOrderMatcher&) -> RegularOrderMatcher& = delete;

  auto operator=(RegularOrderMatcher&&) -> RegularOrderMatcher& = delete;

  auto match(LimitOrder& taker) -> void override;

  auto match(MarketOrder& taker) -> void override;

  auto has_facing_orders(const LimitOrder& taker) -> bool override;

  auto has_facing_orders(const MarketOrder& taker) -> bool override;

  auto can_fully_trade(const LimitOrder& taker) -> bool override;

 private:
  // Determines if the given limit resting order matches by price.
  using PriceMatchCriteria = std::function<bool(const LimitOrder&)>;

  auto find_matching_orders(const LimitOrder& taker)
      -> std::vector<LimitOrder*>;

  auto find_matching_orders(const MarketOrder& taker)
      -> std::vector<LimitOrder*>;

  auto trade_taker(LimitOrder& taker,
                   const std::vector<LimitOrder*>& makers) -> void;

  auto trade_ioc_taker(LimitOrder& taker,
                       const std::vector<LimitOrder*>& makers) -> void;

  auto trade_market_taker(MarketOrder& taker,
                          const std::vector<LimitOrder*>& makers) -> void;

  static auto remove_filled_orders(LimitOrdersContainer& side) -> void;

  auto take_opposite_limit_orders(Side taker_side) -> LimitOrdersContainer&;

  static auto make_price_criteria(const LimitOrder& taker)
      -> PriceMatchCriteria;

  template <typename TakerOrderType>
  static auto compute_trade(const TakerOrderType& taker,
                            const LimitOrder& maker)
      -> std::pair<ExecutionPrice, ExecutedQuantity>;

  OrderBook& order_book_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_REGULAR_ORDER_MATCHER_HPP_
