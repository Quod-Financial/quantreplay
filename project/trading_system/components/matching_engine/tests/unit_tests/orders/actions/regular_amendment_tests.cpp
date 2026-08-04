#include <gmock/gmock.h>

#include <algorithm>

#include "ih/orders/actions/regular_amendment.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "ih/orders/replies/modification_reply_builders.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/mocks/regular_matcher_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

[[nodiscard]]
auto removal(Side side, OrderPrice price, LeavesQuantity quantity)
    -> OrderBookUpdate {
  return OrderBookUpdate{.side = side,
                         .action = OrderBookUpdate::Action::Remove,
                         .price = price,
                         .quantity = quantity};
}

[[nodiscard]]
auto addition(Side side, OrderPrice price, LeavesQuantity quantity)
    -> OrderBookUpdate {
  return OrderBookUpdate{.side = side,
                         .action = OrderBookUpdate::Action::Add,
                         .price = price,
                         .quantity = quantity};
}

struct MatchingEngineRegularAmendment : public Test {
  MatchingEngineRegularAmendment() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;
  NiceMock<RegularMatcherMock> matcher;
  RegularAmendment amendment{event_listener, order_book, matcher, std::nullopt};

  auto rest_limit_order(OrderId order_id,
                        OrderPrice price,
                        OrderQuantity quantity,
                        CumExecutedQuantity executed = CumExecutedQuantity{0},
                        Side side = Side::Option::Buy) -> LimitOrder {
    auto order = OrderBuilder{}
                     .with_order_id(order_id)
                     .with_side(side)
                     .with_order_price(price)
                     .with_order_quantity(quantity)
                     .with_time_in_force(TimeInForce::Option::GoodTillCancel)
                     .build_limit_order();
    if (static_cast<double>(executed) > 0.0) {
      order.execute(ExecutedQuantity{static_cast<double>(executed)},
                    ExecutionPrice{static_cast<double>(price)});
    }
    order_book.take_page(side).limit_orders().emplace(order);
    return order;
  }

  static auto amend_to(OrderId order_id,
                       OrderPrice price,
                       OrderQuantity quantity,
                       Side side = Side::Option::Buy,
                       std::optional<OrigClientOrderId> orig_client_order_id =
                           std::nullopt) -> LimitUpdate {
    OrderAttributes attributes;
    attributes.set_time_in_force(TimeInForce::Option::GoodTillCancel);
    LimitUpdate update{protocol::Session{protocol::generator::Session{}},
                       side,
                       LimitOrder::Update{.price = price,
                                          .quantity = quantity,
                                          .attributes = std::move(attributes)}};
    update.order_id = order_id;
    update.orig_client_order_id = std::move(orig_client_order_id);
    return update;
  }

  static auto amend_to_with_time_in_force(OrderId order_id,
                                          OrderPrice price,
                                          OrderQuantity quantity,
                                          TimeInForce::Option time_in_force)
      -> LimitUpdate {
    OrderAttributes attributes;
    attributes.set_time_in_force(time_in_force);
    LimitUpdate update{protocol::Session{protocol::generator::Session{}},
                       Side::Option::Buy,
                       LimitOrder::Update{.price = price,
                                          .quantity = quantity,
                                          .attributes = std::move(attributes)}};
    update.order_id = order_id;
    return update;
  }
};

TEST_F(MatchingEngineRegularAmendment, ReturnsNoUpdatesWhenOrderNotFound) {
  ASSERT_THAT(
      amendment(amend_to(OrderId{99}, OrderPrice{10}, OrderQuantity{5})),
      IsEmpty());
}

TEST_F(MatchingEngineRegularAmendment,
       EmitsRejectNotificationWithOrderNotFoundReason) {
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderModificationReject>(
          AllOf(Field(&protocol::OrderModificationReject::reject_text,
                      Optional(Eq(RejectText{"order not found"}))),
                Field(&protocol::OrderModificationReject::venue_order_id,
                      Optional(Eq(VenueOrderId{"99"}))))))));

  amendment(amend_to(OrderId{99}, OrderPrice{10}, OrderQuantity{5}));
}

TEST_F(MatchingEngineRegularAmendment, ReturnsNoUpdatesWithInvalidQuantity) {
  rest_limit_order(
      OrderId{42}, OrderPrice{10}, OrderQuantity{10}, CumExecutedQuantity{5});

  ASSERT_THAT(
      amendment(amend_to(OrderId{42}, OrderPrice{10}, OrderQuantity{5})),
      IsEmpty());
}

TEST_F(MatchingEngineRegularAmendment,
       EmitsRejectNotificationWithInvalidQuantityReasonAndOrderStatus) {
  rest_limit_order(
      OrderId{42}, OrderPrice{10}, OrderQuantity{10}, CumExecutedQuantity{5});

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderModificationReject>(
          AllOf(Field(&protocol::OrderModificationReject::reject_text,
                      Optional(Eq(RejectText{"invalid quantity"}))),
                Field(&protocol::OrderModificationReject::order_status,
                      Optional(Eq(OrderStatus::Option::PartiallyFilled))))))));

  amendment(amend_to(OrderId{42}, OrderPrice{10}, OrderQuantity{5}));
}

TEST_F(MatchingEngineRegularAmendment, ReturnsNoUpdatesWhenTimeInForceChanged) {
  rest_limit_order(OrderId{42}, OrderPrice{10}, OrderQuantity{10});

  ASSERT_THAT(amendment(amend_to_with_time_in_force(OrderId{42},
                                                    OrderPrice{10},
                                                    OrderQuantity{5},
                                                    TimeInForce::Option::Day)),
              IsEmpty());
}

TEST_F(MatchingEngineRegularAmendment,
       EmitsRejectNotificationWithTimeInForceReasonAndOrderStatus) {
  rest_limit_order(OrderId{42}, OrderPrice{10}, OrderQuantity{10});

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderModificationReject>(
          AllOf(Field(&protocol::OrderModificationReject::reject_text,
                      Optional(
                          Eq(RejectText{"time in force can not be changed"}))),
                Field(&protocol::OrderModificationReject::order_status,
                      Optional(Eq(OrderStatus::Option::New))))))));

  amendment(amend_to_with_time_in_force(
      OrderId{42}, OrderPrice{10}, OrderQuantity{5}, TimeInForce::Option::Day));
}

TEST_F(MatchingEngineRegularAmendment,
       EmitsOrderRemovedFromBookNotificationForPreviousOrderState) {
  rest_limit_order(OrderId{42}, OrderPrice{10}, OrderQuantity{10});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(AllOf(
                  Field(&OrderRemoved::order_id, Eq(OrderId{42})),
                  Field(&OrderRemoved::order_price, Optional(Eq(Price{10}))),
                  Field(&OrderRemoved::order_side, Eq(Side::Option::Buy)))))));

  amendment(amend_to(OrderId{42}, OrderPrice{15}, OrderQuantity{20}));
}

TEST_F(MatchingEngineRegularAmendment,
       EmitsModificationConfirmationWithExecutionIdAndOrigClientOrderId) {
  rest_limit_order(OrderId{42}, OrderPrice{10}, OrderQuantity{10});

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<
                              protocol::OrderModificationConfirmation>(AllOf(
          Field(&protocol::OrderModificationConfirmation::execution_id,
                Ne(std::nullopt)),
          Field(&protocol::OrderModificationConfirmation::orig_client_order_id,
                Optional(Eq(OrigClientOrderId{"orig-1"}))))))));

  amendment(amend_to(OrderId{42},
                     OrderPrice{15},
                     OrderQuantity{20},
                     Side::Option::Buy,
                     OrigClientOrderId{"orig-1"}));
}

TEST_F(MatchingEngineRegularAmendment,
       ReturnsRemovalOfPreviousStateFollowedByAdditionOfAmendedState) {
  rest_limit_order(OrderId{42}, OrderPrice{10}, OrderQuantity{10});

  ASSERT_THAT(
      amendment(amend_to(OrderId{42}, OrderPrice{15}, OrderQuantity{20})),
      ElementsAre(
          removal(Side::Option::Buy, OrderPrice{10}, LeavesQuantity{10}),
          addition(Side::Option::Buy, OrderPrice{15}, LeavesQuantity{20})));
}

TEST_F(MatchingEngineRegularAmendment,
       EmitsOrderAddedToBookNotificationWhenOrderRestsAfterAmend) {
  rest_limit_order(OrderId{42}, OrderPrice{10}, OrderQuantity{10});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(AllOf(
                  Field(&OrderAdded::order_id, Eq(OrderId{42})),
                  Field(&OrderAdded::order_price, Optional(Eq(Price{15}))),
                  Field(&OrderAdded::order_quantity, Eq(Quantity{20})))))));

  amendment(amend_to(OrderId{42}, OrderPrice{15}, OrderQuantity{20}));
}

TEST_F(MatchingEngineRegularAmendment,
       ReplacesOrderInBookWithAmendedStateWhenOrderRests) {
  rest_limit_order(OrderId{42}, OrderPrice{10}, OrderQuantity{10});

  amendment(amend_to(OrderId{42}, OrderPrice{15}, OrderQuantity{20}));

  const auto& resting_orders = order_book.buy_page().limit_orders();
  ASSERT_EQ(resting_orders.size(), 1);
  ASSERT_EQ(resting_orders.begin()->price(), OrderPrice{15});
  ASSERT_EQ(resting_orders.begin()->total_quantity(), OrderQuantity{20});
}

TEST_F(MatchingEngineRegularAmendment,
       ReturnsOnlyRemovalWhenOrderIsFullyExecutedByMatch) {
  rest_limit_order(OrderId{42}, OrderPrice{10}, OrderQuantity{10});

  EXPECT_CALL(matcher, match(A<LimitOrder&>())).WillOnce([](LimitOrder& order) {
    order.execute(ExecutedQuantity{order.leaves_quantity().value()},
                  ExecutionPrice{order.price().value()});
  });

  ASSERT_THAT(
      amendment(amend_to(OrderId{42}, OrderPrice{10}, OrderQuantity{20})),
      ElementsAre(
          removal(Side::Option::Buy, OrderPrice{10}, LeavesQuantity{10})));
}

TEST_F(MatchingEngineRegularAmendment,
       DoesNotEmitOrderAddedToBookNotificationWhenOrderIsFullyExecutedByMatch) {
  rest_limit_order(OrderId{42}, OrderPrice{10}, OrderQuantity{10});

  EXPECT_CALL(matcher, match(A<LimitOrder&>())).WillOnce([](LimitOrder& order) {
    order.execute(ExecutedQuantity{order.leaves_quantity().value()},
                  ExecutionPrice{order.price().value()});
  });

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(_))))
      .Times(0);

  amendment(amend_to(OrderId{42}, OrderPrice{10}, OrderQuantity{20}));
}

TEST_F(MatchingEngineRegularAmendment,
       RemovesOrderFromBookWhenOrderIsFullyExecutedByMatch) {
  rest_limit_order(OrderId{42}, OrderPrice{10}, OrderQuantity{10});

  EXPECT_CALL(matcher, match(A<LimitOrder&>())).WillOnce([](LimitOrder& order) {
    order.execute(ExecutedQuantity{order.leaves_quantity().value()},
                  ExecutionPrice{order.price().value()});
  });

  amendment(amend_to(OrderId{42}, OrderPrice{10}, OrderQuantity{20}));

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineRegularAmendment, AmendsRestingSellSideOrder) {
  rest_limit_order(OrderId{42},
                   OrderPrice{10},
                   OrderQuantity{10},
                   CumExecutedQuantity{0},
                   Side::Option::Sell);

  ASSERT_THAT(
      amendment(amend_to(
          OrderId{42}, OrderPrice{15}, OrderQuantity{20}, Side::Option::Sell)),
      ElementsAre(
          removal(Side::Option::Sell, OrderPrice{10}, LeavesQuantity{10}),
          addition(Side::Option::Sell, OrderPrice{15}, LeavesQuantity{20})));
}

TEST_F(MatchingEngineRegularAmendment,
       AmendsCorrectOrderWhenMultipleOrdersRestInSamePage) {
  rest_limit_order(OrderId{42}, OrderPrice{10}, OrderQuantity{10});
  rest_limit_order(OrderId{43}, OrderPrice{20}, OrderQuantity{30});

  ASSERT_THAT(
      amendment(amend_to(OrderId{43}, OrderPrice{25}, OrderQuantity{15})),
      ElementsAre(
          removal(Side::Option::Buy, OrderPrice{20}, LeavesQuantity{30}),
          addition(Side::Option::Buy, OrderPrice{25}, LeavesQuantity{15})));

  const auto& resting_orders = order_book.buy_page().limit_orders();
  const auto untouched_order = std::find_if(
      resting_orders.begin(),
      resting_orders.end(),
      [](const LimitOrder& order) { return order.id() == OrderId{42}; });
  ASSERT_NE(untouched_order, resting_orders.end());
  ASSERT_EQ(untouched_order->price(), OrderPrice{10});
}

TEST_F(MatchingEngineRegularAmendment,
       RoundsAveragePriceToPriceTickInModificationConfirmation) {
  auto order = OrderBuilder{}
                   .with_order_id(OrderId{42})
                   .with_side(Side::Option::Buy)
                   .with_order_price(OrderPrice{10})
                   .with_order_quantity(OrderQuantity{10})
                   .with_time_in_force(TimeInForce::Option::GoodTillCancel)
                   .build_limit_order();
  order.execute(ExecutedQuantity{4}, ExecutionPrice{10.2});
  order_book.take_page(Side::Option::Buy).limit_orders().emplace(order);

  RegularAmendment amendment_with_tick{
      event_listener, order_book, matcher, PriceTick{0.5}};

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderModificationConfirmation>(Field(
                      &protocol::OrderModificationConfirmation::average_price,
                      Optional(Eq(AveragePrice{10.0})))))));

  amendment_with_tick(amend_to(OrderId{42}, OrderPrice{10}, OrderQuantity{20}));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
