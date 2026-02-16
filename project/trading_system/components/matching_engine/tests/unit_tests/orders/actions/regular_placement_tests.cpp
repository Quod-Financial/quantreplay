#include <gmock/gmock.h>

#include "ih/orders/actions/regular_placement.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/mocks/regular_matcher_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

struct MatchingEngineRegularPlacement : public Test {
  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;
  NiceMock<RegularMatcherMock> matcher;
  RegularPlacement regular_placement{event_listener, order_book, matcher};
};

struct MatchingEngineRegularPlacementLimitOrderImmediateOrCancel
    : public MatchingEngineRegularPlacement {
  static constexpr OrderId order_id{42};

  static auto ioc_limit_order() -> LimitOrder {
    return OrderBuilder{}
        .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
        .with_order_id(order_id)
        .build_limit_order();
  }
};

TEST_F(MatchingEngineRegularPlacementLimitOrderImmediateOrCancel,
       EmitsPlacementRejectWithExecutionIdWhenNoFacingOrders) {
  const auto order = ioc_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(Field(
          &protocol::OrderPlacementReject::execution_id,
          Optional(
              Eq(ExecutionId{std::to_string(order_id.value()) + "-1"})))))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrderImmediateOrCancel,
       EmitsPlacementRejectWithRejectTextWhenNoFacingOrders) {
  const auto order = ioc_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(
          Field(&protocol::OrderPlacementReject::reject_text,
                Optional(Eq(RejectText{"no facing orders found"})))))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrderImmediateOrCancel,
       EmitsPlacementConfirmationWhenHasFacingOrders) {
  auto order = ioc_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(true));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementConfirmation>(
          Field(&protocol::OrderPlacementConfirmation::execution_id,
                Optional(Eq(
                    ExecutionId{std::to_string(order_id.value()) + "-1"})))))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrderImmediateOrCancel,
       MatchesOrderWhenFacingOrderExists) {
  auto order = ioc_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(true));

  EXPECT_CALL(matcher, match(A<LimitOrder&>()));

  regular_placement(std::move(order));
}

struct MatchingEngineRegularPlacementLimitOrderFillOrKill
    : public MatchingEngineRegularPlacement {
  static constexpr OrderId order_id{42};

  static auto fok_limit_order() -> LimitOrder {
    return OrderBuilder{}
        .with_time_in_force(TimeInForce::Option::FillOrKill)
        .with_order_id(order_id)
        .build_limit_order();
  }
};

TEST_F(MatchingEngineRegularPlacementLimitOrderFillOrKill,
       EmitsPlacementRejectWithExecutionIdWhenNoFacingOrders) {
  const auto order = fok_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(Field(
          &protocol::OrderPlacementReject::execution_id,
          Optional(
              Eq(ExecutionId{std::to_string(order_id.value()) + "-1"})))))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrderFillOrKill,
       EmitsPlacementRejectWithRejectTextWhenNoFacingOrders) {
  const auto order = fok_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(
          Field(&protocol::OrderPlacementReject::reject_text,
                Optional(Eq(RejectText{"no facing orders found"})))))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrderFillOrKill,
       EmitsPlacementRejectWithExecutionIdWhenOrderCannotBeFullyTraded) {
  const auto order = fok_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(true));
  EXPECT_CALL(matcher, can_fully_trade(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(Field(
          &protocol::OrderPlacementReject::execution_id,
          Optional(
              Eq(ExecutionId{std::to_string(order_id.value()) + "-1"})))))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrderFillOrKill,
       EmitsPlacementRejectWithRejectTextWhenOrderCannotBeFullyTraded) {
  const auto order = fok_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(true));
  EXPECT_CALL(matcher, can_fully_trade(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(Field(
          &protocol::OrderPlacementReject::reject_text,
          Optional(
              Eq(RejectText{"not enough liquidity to fill FoK order"})))))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrderFillOrKill,
       EmitsPlacementConfirmationWhenOrderCanBeFullyTraded) {
  auto order = fok_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(true));
  EXPECT_CALL(matcher, can_fully_trade(A<const LimitOrder&>()))
      .WillOnce(Return(true));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementConfirmation>(
          Field(&protocol::OrderPlacementConfirmation::execution_id,
                Optional(Eq(
                    ExecutionId{std::to_string(order_id.value()) + "-1"})))))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrderFillOrKill,
       MatchesOrderWhenOrderCanBeFullyTraded) {
  auto order = fok_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(true));
  EXPECT_CALL(matcher, can_fully_trade(A<const LimitOrder&>()))
      .WillOnce(Return(true));

  EXPECT_CALL(matcher, match(A<LimitOrder&>()));

  regular_placement(std::move(order));
}

struct MatchingEngineRegularPlacementLimitOrder
    : public MatchingEngineRegularPlacement {
  static constexpr OrderId order_id{42};

  static auto limit_order() -> LimitOrder {
    return OrderBuilder{}.with_order_id(order_id).build_limit_order();
  }
};

TEST_F(MatchingEngineRegularPlacementLimitOrder, EmitsPlacementConfirmation) {
  auto order = limit_order();

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementConfirmation>(
          Field(&protocol::OrderPlacementConfirmation::execution_id,
                Optional(Eq(
                    ExecutionId{std::to_string(order_id.value()) + "-1"})))))));

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(_))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrder, MatchesOrder) {
  auto order = limit_order();

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderPlacementConfirmation>(_))));

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(_))));

  EXPECT_CALL(matcher, match(A<LimitOrder&>()));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrder,
       DoesNotAddOrderToBookWhenOrderIsExecuted) {
  auto order = limit_order();

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderPlacementConfirmation>(_))));

  EXPECT_CALL(matcher, match(A<LimitOrder&>())).WillOnce([](LimitOrder& ord) {
    ord.execute(ExecutedQuantity{ord.leaves_quantity().value()});
  });

  regular_placement(std::move(order));

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineRegularPlacementLimitOrder,
       DoesNotEmitOrderAddedOrderBookNotificationWhenOrderIsExecuted) {
  auto order = limit_order();

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderPlacementConfirmation>(_))));
  EXPECT_CALL(matcher, match(A<LimitOrder&>())).WillOnce([](LimitOrder& ord) {
    ord.execute(ExecutedQuantity{ord.leaves_quantity().value()});
  });

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(_))))
      .Times(0);

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrder,
       AddsOrderToBookWhenOrderIsNotExecuted) {
  auto order = limit_order();

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderPlacementConfirmation>(_))));
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(_))));
  EXPECT_CALL(matcher, match(A<LimitOrder&>()));

  regular_placement(std::move(order));

  ASSERT_EQ(order_book.buy_page().limit_orders().size(), 1);
}

TEST_F(MatchingEngineRegularPlacementLimitOrder,
       EmitsOrderAddedOrderBookNotificationWhenOrderIsExecuted) {
  auto order = limit_order();

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderPlacementConfirmation>(_))));
  EXPECT_CALL(matcher, match(A<LimitOrder&>()));

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(
                  Field(&OrderAdded::order_id, Eq(order_id))))));

  regular_placement(std::move(order));
}

struct MatchingEngineRegularPlacementMarketOrder
    : public MatchingEngineRegularPlacement {
  static constexpr OrderId order_id{42};

  static auto market_order() -> MarketOrder {
    return OrderBuilder{}.with_order_id(order_id).build_market_order();
  }
};

TEST_F(MatchingEngineRegularPlacementMarketOrder,
       EmitsPlacementRejectWithExecutionIdWhenNoFacingOrders) {
  const auto order = market_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const MarketOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(Field(
          &protocol::OrderPlacementReject::execution_id,
          Optional(
              Eq(ExecutionId{std::to_string(order_id.value()) + "-1"})))))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementMarketOrder,
       EmitsPlacementRejectWithRejectTextWhenNoFacingOrders) {
  const auto order = market_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const MarketOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(
          Field(&protocol::OrderPlacementReject::reject_text,
                Optional(Eq(RejectText{"no facing orders found"})))))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementMarketOrder,
       EmitsPlacementConfirmationWhenHasFacingOrders) {
  auto order = market_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const MarketOrder&>()))
      .WillOnce(Return(true));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementConfirmation>(
          Field(&protocol::OrderPlacementConfirmation::execution_id,
                Optional(Eq(
                    ExecutionId{std::to_string(order_id.value()) + "-1"})))))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementMarketOrder,
       MatchesOrderWhenFacingOrderExists) {
  auto order = market_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const MarketOrder&>()))
      .WillOnce(Return(true));

  EXPECT_CALL(matcher, match(A<MarketOrder&>()));

  regular_placement(std::move(order));
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
