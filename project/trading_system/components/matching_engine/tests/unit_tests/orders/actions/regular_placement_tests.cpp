#include <gmock/gmock.h>

#include "ih/orders/actions/regular_placement.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/mocks/regular_matcher_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

MATCHER_P5(IsOrderCancellationConfirmationWithCancellationText,
           venue_order_id,
           order_status,
           leaving_quantity,
           client_order_id,
           cancellation_text,
           "") {
  return ExplainMatchResult(
      AllOf(
          IsOrderCancellationConfirmation(
              venue_order_id, order_status, leaving_quantity, client_order_id),
          VariantWith<protocol::OrderCancellationConfirmation>(
              Field(&protocol::OrderCancellationConfirmation::cancellation_text,
                    Optional(Eq(cancellation_text))))),
      arg,
      result_listener);
}

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
        .with_client_order_id(ClientOrderId{"client-42"})
        .build_limit_order();
  }
};

TEST_F(MatchingEngineRegularPlacementLimitOrderImmediateOrCancel,
       EmitsCancellationConfirmationWhenNoFacingOrders) {
  const auto order = ioc_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  IsOrderCancellationConfirmationWithCancellationText(
                      VenueOrderId{"42"},
                      OrderStatus::Option::Cancelled,
                      LeavesQuantity{0},
                      ClientOrderId{"client-42"},
                      CancellationText{"no facing orders found"}))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrderImmediateOrCancel,
       ReturnsNoUpdatesWhenNoFacingOrders) {
  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  ASSERT_THAT(regular_placement(ioc_limit_order()), IsEmpty());
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

TEST_F(MatchingEngineRegularPlacementLimitOrderImmediateOrCancel,
       ReturnsNoUpdatesWhenFacingOrderExists) {
  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(true));

  ASSERT_THAT(regular_placement(ioc_limit_order()), IsEmpty());
}

struct MatchingEngineRegularPlacementLimitOrderFillOrKill
    : public MatchingEngineRegularPlacement {
  static constexpr OrderId order_id{42};

  static auto fok_limit_order() -> LimitOrder {
    return OrderBuilder{}
        .with_time_in_force(TimeInForce::Option::FillOrKill)
        .with_order_id(order_id)
        .with_client_order_id(ClientOrderId{"client-42"})
        .build_limit_order();
  }
};

TEST_F(MatchingEngineRegularPlacementLimitOrderFillOrKill,
       EmitsCancellationConfirmationWhenNoFacingOrders) {
  const auto order = fok_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  IsOrderCancellationConfirmationWithCancellationText(
                      VenueOrderId{"42"},
                      OrderStatus::Option::Cancelled,
                      LeavesQuantity{0},
                      ClientOrderId{"client-42"},
                      CancellationText{"no facing orders found"}))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrderFillOrKill,
       ReturnsNoUpdatesWhenNoFacingOrders) {
  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  ASSERT_THAT(regular_placement(fok_limit_order()), IsEmpty());
}

TEST_F(MatchingEngineRegularPlacementLimitOrderFillOrKill,
       EmitsCancellationConfirmationWhenOrderCannotBeFullyTraded) {
  const auto order = fok_limit_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(true));
  EXPECT_CALL(matcher, can_fully_trade(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(
          IsOrderCancellationConfirmationWithCancellationText(
              VenueOrderId{"42"},
              OrderStatus::Option::Cancelled,
              LeavesQuantity{0},
              ClientOrderId{"client-42"},
              CancellationText{"not enough liquidity to fill FoK order"}))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrderFillOrKill,
       ReturnsNoUpdatesWhenOrderCannotBeFullyTraded) {
  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(true));
  EXPECT_CALL(matcher, can_fully_trade(A<const LimitOrder&>()))
      .WillOnce(Return(false));

  ASSERT_THAT(regular_placement(fok_limit_order()), IsEmpty());
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

TEST_F(MatchingEngineRegularPlacementLimitOrderFillOrKill,
       ReturnsNoUpdatesWhenOrderCanBeFullyTraded) {
  EXPECT_CALL(matcher, has_facing_orders(A<const LimitOrder&>()))
      .WillOnce(Return(true));
  EXPECT_CALL(matcher, can_fully_trade(A<const LimitOrder&>()))
      .WillOnce(Return(true));

  ASSERT_THAT(regular_placement(fok_limit_order()), IsEmpty());
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
    ord.execute(ExecutedQuantity{ord.leaves_quantity().value()},
                ExecutionPrice{ord.price().value()});
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
    ord.execute(ExecutedQuantity{ord.leaves_quantity().value()},
                ExecutionPrice{ord.price().value()});
  });

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(_))))
      .Times(0);

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementLimitOrder,
       ReturnsNoUpdatesWhenOrderIsExecuted) {
  auto order = limit_order();

  EXPECT_CALL(matcher, match(A<LimitOrder&>())).WillOnce([](LimitOrder& ord) {
    ord.execute(ExecutedQuantity{ord.leaves_quantity().value()},
                ExecutionPrice{ord.price().value()});
  });

  ASSERT_THAT(regular_placement(std::move(order)), IsEmpty());
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

TEST_F(MatchingEngineRegularPlacementLimitOrder,
       ReturnsAddUpdateForRestingOrderWhenOrderIsNotExecuted) {
  const auto order = limit_order();

  EXPECT_CALL(matcher, match(A<LimitOrder&>()));

  ASSERT_THAT(
      regular_placement(order),
      ElementsAre(OrderBookUpdate{.side = order.side(),
                                  .action = OrderBookUpdate::Action::Add,
                                  .price = order.price(),
                                  .quantity = order.leaves_quantity()}));
}

struct MatchingEngineRegularPlacementMarketOrder
    : public MatchingEngineRegularPlacement {
  static constexpr OrderId order_id{42};

  static auto market_order() -> MarketOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_client_order_id(ClientOrderId{"client-42"})
        .build_market_order();
  }
};

TEST_F(MatchingEngineRegularPlacementMarketOrder,
       EmitsCancellationConfirmationWhenNoFacingOrders) {
  const auto order = market_order();

  EXPECT_CALL(matcher, has_facing_orders(A<const MarketOrder&>()))
      .WillOnce(Return(false));

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  IsOrderCancellationConfirmationWithCancellationText(
                      VenueOrderId{"42"},
                      OrderStatus::Option::Cancelled,
                      LeavesQuantity{0},
                      ClientOrderId{"client-42"},
                      CancellationText{"no facing orders found"}))));

  regular_placement(std::move(order));
}

TEST_F(MatchingEngineRegularPlacementMarketOrder,
       ReturnsNoUpdatesWhenNoFacingOrders) {
  EXPECT_CALL(matcher, has_facing_orders(A<const MarketOrder&>()))
      .WillOnce(Return(false));

  ASSERT_THAT(regular_placement(market_order()), IsEmpty());
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

TEST_F(MatchingEngineRegularPlacementMarketOrder,
       ReturnsNoUpdatesWhenFacingOrderExists) {
  EXPECT_CALL(matcher, has_facing_orders(A<const MarketOrder&>()))
      .WillOnce(Return(true));

  ASSERT_THAT(regular_placement(market_order()), IsEmpty());
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
