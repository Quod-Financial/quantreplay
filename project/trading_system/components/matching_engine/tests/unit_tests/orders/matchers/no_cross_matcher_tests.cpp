#include <gmock/gmock.h>

#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/matchers/no_cross_matcher.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineNoCrossMatcher : public Test {
  NoCrossMatcher matcher;

  static auto limit_order(OrderQuantity quantity) -> LimitOrder {
    return OrderBuilder{}.with_order_quantity(quantity).build_limit_order();
  }

  static auto market_order(OrderQuantity quantity) -> MarketOrder {
    return OrderBuilder{}.with_order_quantity(quantity).build_market_order();
  }
};

TEST_F(MatchingEngineNoCrossMatcher, DoesNotReportFacingOrdersForLimitTaker) {
  auto taker = limit_order(OrderQuantity{100});

  ASSERT_THAT(matcher.has_facing_orders(taker), IsFalse());
}

TEST_F(MatchingEngineNoCrossMatcher, DoesNotReportFacingOrdersForMarketTaker) {
  auto taker = market_order(OrderQuantity{100});

  ASSERT_THAT(matcher.has_facing_orders(taker), IsFalse());
}

TEST_F(MatchingEngineNoCrossMatcher, ReportsLimitTakerCanNotBeFullyTraded) {
  auto taker = limit_order(OrderQuantity{100});

  ASSERT_THAT(matcher.can_fully_trade(taker), IsFalse());
}

TEST_F(MatchingEngineNoCrossMatcher, DoesNotTradeLimitTaker) {
  auto taker = limit_order(OrderQuantity{100});

  matcher.match(taker);

  ASSERT_THAT(taker.leaves_quantity(), Eq(Quantity{100}));
}

TEST_F(MatchingEngineNoCrossMatcher, DoesNotTradeMarketTaker) {
  auto taker = market_order(OrderQuantity{100});

  matcher.match(taker);

  ASSERT_THAT(taker.leaves_quantity(), Eq(Quantity{100}));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
