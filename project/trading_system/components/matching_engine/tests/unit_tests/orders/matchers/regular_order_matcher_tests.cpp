#include <gmock/gmock.h>

#include "ih/common/events/client_notification.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/matchers/regular_order_matcher.hpp"
#include "mocks/mock_execution_reports_listener.hpp"
#include "tools/order_test_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

// region BuyLimitOrderMatching

struct BuyLimitOrderMatching : public Test {
  auto make_aggressor(OrderId ord_id,
                      Price price,
                      Quantity quantity) -> LimitOrder {
    return order_builder_.with_order_id(ord_id)
        .with_order_price(static_cast<OrderPrice>(price))
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_side(Side::Option::Buy)
        .with_time_in_force(TimeInForce::Option::Day)
        .build_limit_order();
  }

  auto make_ioc_aggressor(OrderId order_id,
                          Price price,
                          Quantity quantity) -> LimitOrder {
    return order_builder_.with_order_id(order_id)
        .with_order_price(static_cast<OrderPrice>(price))
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_side(Side::Option::Buy)
        .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
        .build_limit_order();
  }

  auto add_resting_order(OrderId ord_id,
                         Price price,
                         Quantity quantity) -> void {
    resting_orders().emplace(
        order_builder_.with_order_id(ord_id)
            .with_order_price(static_cast<OrderPrice>(price))
            .with_order_quantity(static_cast<OrderQuantity>(quantity))
            .with_side(Side::Option::Sell)
            .build_limit_order());
  }

  auto resting_orders() -> LimitOrdersContainer& {
    return book_.sell_page().limit_orders();
  }

 private:
  OrderBook book_;
  OrderBuilder order_builder_;

 public:
  MockExecutionReportsListener listener;
  RegularOrderMatcher matcher{listener, book_};
};

TEST_F(BuyLimitOrderMatching, DoesNotDetectFacingOrdersInEmptyPage) {
  ASSERT_THAT(resting_orders(), IsEmpty());

  LimitOrder order = make_aggressor(OrderId{1}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(BuyLimitOrderMatching, DoesNotDetectFacingOrderWhenAllHaveWorsePrice) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});
  add_resting_order(OrderId{2}, Price{102}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(BuyLimitOrderMatching, DetectsFacingOrders) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});
  add_resting_order(OrderId{2}, Price{102}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsTrue());
}

TEST_F(BuyLimitOrderMatching, DetectsTakerCanNotBeFullyTraded) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{99});
  add_resting_order(OrderId{2}, Price{101}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.can_fully_trade(order), IsFalse());
}

TEST_F(BuyLimitOrderMatching, DetectsTakerCanBeFullyTraded) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{55});
  add_resting_order(OrderId{2}, Price{100}, Quantity{55});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.can_fully_trade(order), IsTrue());
}

TEST_F(BuyLimitOrderMatching, SkipsMatchingIfSellSideIsEmpty) {
  ASSERT_THAT(resting_orders(), IsEmpty());

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, IsEmpty());
}

TEST_F(BuyLimitOrderMatching, SkipsMatchingIfSellSideHasWorsePrices) {
  add_resting_order(OrderId{1}, Price{99.0001}, Quantity{100});
  add_resting_order(OrderId{2}, Price{100}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{99.0000}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, IsEmpty());
}

TEST_F(BuyLimitOrderMatching, MatchesAggressorAtSamePrice) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));
}

TEST_F(BuyLimitOrderMatching, MatchesAggressorWithBetterPrice) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{99.9999});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));
}

TEST_F(BuyLimitOrderMatching, MatchesAggrssorWithMultipleRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{2}, Price{100}, Quantity{50});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(4));
}

TEST_F(BuyLimitOrderMatching, RemovesFilledRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{2}, Price{100}, Quantity{50});
  ASSERT_THAT(resting_orders(), SizeIs(2));

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(resting_orders(), IsEmpty());
}

TEST_F(BuyLimitOrderMatching, DoesNotRemoveNonFilledRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{51});
  ASSERT_THAT(resting_orders(), SizeIs(1));

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{50});
  matcher.match(order);

  ASSERT_THAT(resting_orders(), SizeIs(1));
}

TEST_F(BuyLimitOrderMatching, MatchesByRestingOrderPrice) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));
  EXPECT_THAT(listener.reports[0].execution_price, Eq(Price{99}));
  EXPECT_THAT(listener.reports[1].execution_price, Eq(Price{99}));
}

TEST_F(BuyLimitOrderMatching, MatchesMinimalLeavesQuantity) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));
  EXPECT_THAT(listener.reports[0].executed_quantity, Eq(Quantity{50}));
  EXPECT_THAT(listener.reports[1].executed_quantity, Eq(Quantity{50}));
}

TEST_F(BuyLimitOrderMatching, FullyMatchesIocOrderWithRestingOrder) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});
  matcher.match(order);

  EXPECT_THAT(order.executed(), IsTrue());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(BuyLimitOrderMatching, FullyMatchesIocOrderWithMultipleRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{2}, Price{100}, Quantity{50});
  add_resting_order(OrderId{3}, Price{101}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(OrderId{4}, Price{100}, Quantity{100});
  matcher.match(order);

  EXPECT_THAT(order.executed(), IsTrue());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(BuyLimitOrderMatching, PartiallyMatchesIocOrderWithRestingOrder) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{3}, Price{101}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{75});
  matcher.match(order);

  EXPECT_THAT(order.executed(), IsFalse());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Cancelled));
  EXPECT_THAT(order.leaves_quantity(), Eq(Quantity{25}));
}

TEST_F(BuyLimitOrderMatching,
       PartiallyMatchesIocOrderWithMultipleRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{25});
  add_resting_order(OrderId{2}, Price{100}, Quantity{25});
  add_resting_order(OrderId{3}, Price{101}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(OrderId{4}, Price{100}, Quantity{75});
  matcher.match(order);

  EXPECT_THAT(order.executed(), IsFalse());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Cancelled));
  EXPECT_THAT(order.leaves_quantity(), Eq(Quantity{25}));
}

TEST_F(BuyLimitOrderMatching, ThrowsExceptionWhenIocOrderHasNoFacingOrders) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{100});

  ASSERT_THROW(matcher.match(order), std::logic_error);
}

// endregion BuyLimitOrderMatching

// region SellLimitOrderMatching

struct SellLimitOrderMatching : public Test {
  auto make_aggressor(OrderId ord_id,
                      Price price,
                      Quantity quantity) -> LimitOrder {
    return order_builder_.with_order_id(ord_id)
        .with_order_price(static_cast<OrderPrice>(price))
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_side(Side::Option::Sell)
        .build_limit_order();
  }

  auto make_ioc_aggressor(OrderId order_id,
                          Price price,
                          Quantity quantity) -> LimitOrder {
    return order_builder_.with_order_id(order_id)
        .with_order_price(static_cast<OrderPrice>(price))
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_side(Side::Option::Sell)
        .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
        .build_limit_order();
  }

  auto add_resting_order(OrderId ord_id,
                         Price price,
                         Quantity quantity) -> void {
    resting_orders().emplace(
        order_builder_.with_order_id(ord_id)
            .with_order_price(static_cast<OrderPrice>(price))
            .with_order_quantity(static_cast<OrderQuantity>(quantity))
            .with_side(Side::Option::Buy)
            .build_limit_order());
  }

  auto resting_orders() -> LimitOrdersContainer& {
    return book_.buy_page().limit_orders();
  }

 private:
  OrderBook book_;
  OrderBuilder order_builder_;

 public:
  MockExecutionReportsListener listener;
  RegularOrderMatcher matcher{listener, book_};
};

TEST_F(SellLimitOrderMatching, DoesNotDetectFacingOrdersInEmptyPage) {
  ASSERT_THAT(resting_orders(), IsEmpty());

  LimitOrder order = make_aggressor(OrderId{1}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(SellLimitOrderMatching, DoesNotDetectFacingOrderWhenAllHaveWorsePrice) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});
  add_resting_order(OrderId{2}, Price{98}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(SellLimitOrderMatching, DetectsFacingOrders) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});
  add_resting_order(OrderId{2}, Price{99}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsTrue());
}

TEST_F(SellLimitOrderMatching, DetectsTakerCanNotBeFullyTraded) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{99});
  add_resting_order(OrderId{2}, Price{99}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.can_fully_trade(order), IsFalse());
}

TEST_F(SellLimitOrderMatching, DetectsTakerCanBeFullyTraded) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{55});
  add_resting_order(OrderId{2}, Price{100}, Quantity{55});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.can_fully_trade(order), IsTrue());
}

TEST_F(SellLimitOrderMatching, SkipsMatchingIfBuySideIsEmpty) {
  ASSERT_THAT(resting_orders(), IsEmpty());

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, IsEmpty());
}

TEST_F(SellLimitOrderMatching, SkipsMatchingIfBuySideHasWorsePrices) {
  add_resting_order(OrderId{1}, Price{99.9999}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100.0000}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, IsEmpty());
}

TEST_F(SellLimitOrderMatching, MatchesAggressorAtSamePrice) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));
}

TEST_F(SellLimitOrderMatching, MatchesAggressorWithBetterPrice) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100.0001});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));
}

TEST_F(SellLimitOrderMatching, MatchesAggrssorWithMultipleRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{2}, Price{100}, Quantity{50});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(4));
}

TEST_F(SellLimitOrderMatching, RemovesFilledRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{2}, Price{100}, Quantity{50});
  ASSERT_THAT(resting_orders(), SizeIs(2));

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(resting_orders(), IsEmpty());
}

TEST_F(SellLimitOrderMatching, DoesNotRemoveNonFilledRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{51});
  ASSERT_THAT(resting_orders(), SizeIs(1));

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{50});
  matcher.match(order);

  ASSERT_THAT(resting_orders(), SizeIs(1));
}

TEST_F(SellLimitOrderMatching, MatchesByRestingOrderPrice) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));
  EXPECT_THAT(listener.reports[0].execution_price, Eq(Price{101}));
  EXPECT_THAT(listener.reports[1].execution_price, Eq(Price{101}));
}

TEST_F(SellLimitOrderMatching, MatchesMinimalLeavesQuantity) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));
  EXPECT_THAT(listener.reports[0].executed_quantity, Eq(Quantity{50}));
  EXPECT_THAT(listener.reports[1].executed_quantity, Eq(Quantity{50}));
}

TEST_F(SellLimitOrderMatching, FullyMatchesIocOrderWithRestingOrder) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});
  matcher.match(order);

  EXPECT_THAT(order.executed(), IsTrue());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(SellLimitOrderMatching, FullyMatchesIocOrderWithMultipleRestingOrders) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{50});
  add_resting_order(OrderId{2}, Price{100}, Quantity{50});
  add_resting_order(OrderId{3}, Price{99}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(OrderId{4}, Price{100}, Quantity{100});
  matcher.match(order);

  EXPECT_THAT(order.executed(), IsTrue());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(SellLimitOrderMatching, PartiallyMatchesIocOrderWithRestingOrder) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{3}, Price{99}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{75});
  matcher.match(order);

  EXPECT_THAT(order.executed(), IsFalse());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Cancelled));
  EXPECT_THAT(order.leaves_quantity(), Eq(Quantity{25}));
}

TEST_F(SellLimitOrderMatching,
       PartiallyMatchesIocOrderWithMultipleRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{25});
  add_resting_order(OrderId{2}, Price{100}, Quantity{25});
  add_resting_order(OrderId{3}, Price{99}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(OrderId{4}, Price{100}, Quantity{75});
  matcher.match(order);

  EXPECT_THAT(order.executed(), IsFalse());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Cancelled));
  EXPECT_THAT(order.leaves_quantity(), Eq(Quantity{25}));
}

TEST_F(SellLimitOrderMatching, ThrowsExceptionWhenIocOrderHasNoFacingOrders) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{100});

  ASSERT_THROW(matcher.match(order), std::logic_error);
}

// endregion SellLimitOrderMatching

// region BuyMarketOrderMatching

struct BuyMarketOrderMatching : public Test {
  auto make_aggressor(OrderId ord_id, Quantity quantity) -> MarketOrder {
    return order_builder_.with_order_id(ord_id)
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_side(Side::Option::Buy)
        .build_market_order();
  }

  auto add_resting_order(OrderId ord_id,
                         Price price,
                         Quantity quantity) -> void {
    resting_orders().emplace(
        order_builder_.with_order_id(ord_id)
            .with_order_price(static_cast<OrderPrice>(price))
            .with_order_quantity(static_cast<OrderQuantity>(quantity))
            .with_side(Side::Option::Sell)
            .build_limit_order());
  }

  auto resting_orders() -> LimitOrdersContainer& {
    return book_.sell_page().limit_orders();
  }

 private:
  OrderBook book_;
  OrderBuilder order_builder_;

 public:
  MockExecutionReportsListener listener;
  RegularOrderMatcher matcher{listener, book_};
};

TEST_F(BuyMarketOrderMatching, DoesNotDetectFacingOrdersInEmptyPage) {
  const MarketOrder order = make_aggressor(OrderId{1}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(BuyMarketOrderMatching, DetectsOppositeFacingOrders) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  const MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsTrue());
}

TEST_F(BuyMarketOrderMatching, ThrowsExceptionWhenMatchedWithoutFacingOrders) {
  MarketOrder order = make_aggressor(OrderId{1}, Quantity{100});

  ASSERT_THROW(matcher.match(order), std::logic_error);
}

TEST_F(BuyMarketOrderMatching, FullyMatchesWithFacingOrder) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{101});

  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));

  EXPECT_THAT(listener.reports[0].execution_price, Optional(Eq(Price{100})));
  EXPECT_THAT(listener.reports[0].executed_quantity,
              Optional(Eq(Quantity{100})));
  EXPECT_THAT(listener.reports[0].order_status,
              Optional(Eq(OrderStatus::Option::Filled)));
}

TEST_F(BuyMarketOrderMatching, PartiallyMatchesWithFacingOrder) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{99});

  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));

  EXPECT_THAT(listener.reports[0].execution_price, Optional(Eq(Price{100})));
  EXPECT_THAT(listener.reports[0].executed_quantity,
              Optional(Eq(Quantity{99})));
  EXPECT_THAT(listener.reports[0].order_status,
              Optional(Eq(OrderStatus::Option::Cancelled)));
}

TEST_F(BuyMarketOrderMatching, MatchesWithSeveralFacingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});
  add_resting_order(OrderId{2}, Price{101}, Quantity{150});

  MarketOrder order = make_aggressor(OrderId{3}, Quantity{200});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(4));

  EXPECT_THAT(listener.reports[0].execution_price, Optional(Eq(Price{100})));
  EXPECT_THAT(listener.reports[0].executed_quantity,
              Optional(Eq(Quantity{100})));
  EXPECT_THAT(listener.reports[0].order_status,
              Optional(Eq(OrderStatus::Option::PartiallyFilled)));

  EXPECT_THAT(listener.reports[2].execution_price, Optional(Eq(Price{101})));
  EXPECT_THAT(listener.reports[2].executed_quantity,
              Optional(Eq(Quantity{100})));
  EXPECT_THAT(listener.reports[2].order_status,
              Optional(Eq(OrderStatus::Option::Filled)));
}

// endregion BuyMarketOrderMatching

// region SellMarketOrderMatching

struct SellMarketOrderMatching : public Test {
  auto make_aggressor(OrderId ord_id, Quantity quantity) -> MarketOrder {
    return order_builder_.with_order_id(ord_id)
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_side(Side::Option::Sell)
        .build_market_order();
  }

  auto add_resting_order(OrderId ord_id,
                         Price price,
                         Quantity quantity) -> void {
    resting_orders().emplace(
        order_builder_.with_order_id(ord_id)
            .with_order_price(static_cast<OrderPrice>(price))
            .with_order_quantity(static_cast<OrderQuantity>(quantity))
            .with_side(Side::Option::Buy)
            .build_limit_order());
  }

  auto resting_orders() -> LimitOrdersContainer& {
    return book_.buy_page().limit_orders();
  }

 private:
  OrderBook book_;
  OrderBuilder order_builder_;

 public:
  MockExecutionReportsListener listener;
  RegularOrderMatcher matcher{listener, book_};
};

TEST_F(SellMarketOrderMatching, DoesNotDetectFacingOrdersInEmptyPage) {
  const MarketOrder order = make_aggressor(OrderId{1}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(SellMarketOrderMatching, DetectsOppositeFacingOrders) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  const MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsTrue());
}

TEST_F(SellMarketOrderMatching, ThrowsExceptionWhenMatchedWithoutFacingOrders) {
  MarketOrder order = make_aggressor(OrderId{1}, Quantity{100});

  ASSERT_THROW(matcher.match(order), std::logic_error);
}

TEST_F(SellMarketOrderMatching, FullyMatchesWithFacingOrder) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{101});

  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));

  EXPECT_THAT(listener.reports[0].execution_price, Optional(Eq(Price{100})));
  EXPECT_THAT(listener.reports[0].executed_quantity,
              Optional(Eq(Quantity{100})));
  EXPECT_THAT(listener.reports[0].order_status,
              Optional(Eq(OrderStatus::Option::Filled)));
}

TEST_F(SellMarketOrderMatching, PartiallyMatchesWithFacingOrder) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{99});

  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(2));

  EXPECT_THAT(listener.reports[0].execution_price, Optional(Eq(Price{100})));
  EXPECT_THAT(listener.reports[0].executed_quantity,
              Optional(Eq(Quantity{99})));
  EXPECT_THAT(listener.reports[0].order_status,
              Optional(Eq(OrderStatus::Option::Cancelled)));
}

TEST_F(SellMarketOrderMatching, MatchesWithSeveralFacingOrders) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});
  add_resting_order(OrderId{2}, Price{100}, Quantity{150});

  MarketOrder order = make_aggressor(OrderId{3}, Quantity{200});
  matcher.match(order);

  ASSERT_THAT(listener.reports, SizeIs(4));

  EXPECT_THAT(listener.reports[0].execution_price, Optional(Eq(Price{101})));
  EXPECT_THAT(listener.reports[0].executed_quantity,
              Optional(Eq(Quantity{100})));
  EXPECT_THAT(listener.reports[0].order_status,
              Optional(Eq(OrderStatus::Option::PartiallyFilled)));

  EXPECT_THAT(listener.reports[2].execution_price, Optional(Eq(Price{100})));
  EXPECT_THAT(listener.reports[2].executed_quantity,
              Optional(Eq(Quantity{100})));
  EXPECT_THAT(listener.reports[2].order_status,
              Optional(Eq(OrderStatus::Option::Filled)));
}

// endregion SellMarketOrderMatching

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test