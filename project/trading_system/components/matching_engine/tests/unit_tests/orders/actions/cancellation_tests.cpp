#include <gmock/gmock.h>

#include "ih/orders/actions/cancellation.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "ih/orders/replies/cancellation_reply_builders.hpp"
#include "protocol/types/session.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineCancellation : public Test {
  MatchingEngineCancellation() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBuilder builder;
  OrderBook order_book;
  Cancellation cancellation{
      event_listener, order_book, std::nullopt, LimitOrderQueue::Regular};

  const protocol::Session client_session{protocol::generator::Session{}};

  auto rest(const LimitOrder& order) -> void {
    order_book.take_page(order.side()).limit_orders().emplace(order);
  }

  auto rest(const MarketOrder& order) -> void {
    order_book.take_page(order.side()).market_orders().emplace(order);
  }

  auto make_cancel(Side side, OrderId order_id) const -> OrderCancel {
    auto cancel = OrderCancel{client_session, side};
    cancel.order_id = order_id;
    return cancel;
  }

  static auto cancel_by_order_id(Side side, OrderId order_id) -> OrderCancel {
    OrderCancel cancel{protocol::Session{protocol::generator::Session{}}, side};
    cancel.order_id = order_id;
    return cancel;
  }
};

TEST_F(MatchingEngineCancellation,
       EmitsOrderCancellationRejectWithRejectTextIfOrderNotFoundInOrderBook) {
  const auto cancel = make_cancel(Side::Option::Buy, OrderId{123});

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderCancellationReject>(
          Field(&protocol::OrderCancellationReject::reject_text,
                Optional(Eq(RejectText{"order not found"})))))));

  cancellation(cancel);
}

TEST_F(MatchingEngineCancellation, ReturnsNoUpdatesWhenOrderNotFound) {
  const auto cancel = make_cancel(Side::Option::Buy, OrderId{123});

  ASSERT_THAT(cancellation(cancel), IsEmpty());
}

TEST_F(MatchingEngineCancellation,
       EmitsOrderRemovedWithoutPriceWhenCancellingMarketOrder) {
  rest(OrderBuilder{}
           .with_order_id(OrderId{42})
           .with_side(Side::Option::Buy)
           .build_market_order());

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(AllOf(
                  Field(&OrderRemoved::order_id, Eq(OrderId{42})),
                  Field(&OrderRemoved::order_price, Eq(std::nullopt)))))));

  cancellation(cancel_by_order_id(Side::Option::Buy, OrderId{42}));
}

TEST_F(MatchingEngineCancellation,
       EmitsCancellationConfirmationWithMarketOrderType) {
  rest(OrderBuilder{}
           .with_order_id(OrderId{42})
           .with_side(Side::Option::Buy)
           .build_market_order());

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::order_type,
                      Optional(Eq(OrderType::Option::Market)))))));

  cancellation(cancel_by_order_id(Side::Option::Buy, OrderId{42}));
}

TEST_F(MatchingEngineCancellation, RemovesRestingMarketOrderFromBook) {
  rest(OrderBuilder{}
           .with_order_id(OrderId{42})
           .with_side(Side::Option::Buy)
           .build_market_order());

  cancellation(cancel_by_order_id(Side::Option::Buy, OrderId{42}));

  ASSERT_THAT(order_book.buy_page().market_orders().size(), Eq(0));
}

TEST_F(MatchingEngineCancellation, ResolvesRestingMarketOrderByClientOrderId) {
  rest(OrderBuilder{}
           .with_side(Side::Option::Buy)
           .with_client_session(client_session)
           .with_client_order_id(ClientOrderId{"CL-1"})
           .build_market_order());

  OrderCancel cancel{client_session, Side::Option::Buy};
  cancel.client_order_id = ClientOrderId{"CL-1"};
  cancellation(cancel);

  ASSERT_THAT(order_book.buy_page().market_orders().size(), Eq(0));
}

TEST_F(MatchingEngineCancellation,
       ResolvesRestingMarketOrderByOrigClientOrderId) {
  rest(OrderBuilder{}
           .with_side(Side::Option::Buy)
           .with_client_session(client_session)
           .with_client_order_id(ClientOrderId{"CL-1"})
           .build_market_order());

  OrderCancel cancel{client_session, Side::Option::Buy};
  cancel.orig_client_order_id = OrigClientOrderId{"CL-1"};
  cancellation(cancel);

  ASSERT_THAT(order_book.buy_page().market_orders().size(), Eq(0));
}

TEST_F(MatchingEngineCancellation,
       EmitsCancellationConfirmationWithLimitOrderType) {
  rest(OrderBuilder{}
           .with_order_id(OrderId{7})
           .with_side(Side::Option::Buy)
           .build_limit_order());

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::order_type,
                      Optional(Eq(OrderType::Option::Limit)))))));

  cancellation(cancel_by_order_id(Side::Option::Buy, OrderId{7}));
}

TEST_F(MatchingEngineCancellation, DeletesOrderFromOrderBook) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_client_session(client_session)
                         .build_limit_order();
  rest(order);

  const auto cancel = make_cancel(Side::Option::Buy, OrderId{123});

  cancellation(cancel);

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineCancellation,
       EmitsOrderRemovedNotificationIfOrderRemoved) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_client_session(client_session)
                         .build_limit_order();
  rest(order);

  const auto cancel = make_cancel(Side::Option::Buy, OrderId{123});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  cancellation(cancel);
}

TEST_F(MatchingEngineCancellation,
       EmitsOrderCancellationConfirmationWhenOrderCancelled) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_client_session(client_session)
                         .build_limit_order();
  rest(order);

  const auto cancel = make_cancel(Side::Option::Buy, OrderId{123});

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(_))));

  cancellation(cancel);
}

TEST_F(MatchingEngineCancellation,
       SetsOrderStatusCancelledInOrderCancellationConfirmation) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_client_session(client_session)
                         .build_limit_order();
  rest(order);

  const auto cancel = make_cancel(Side::Option::Buy, OrderId{123});

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::order_status,
                      Optional(Eq(OrderStatus::Option::Cancelled)))))));

  cancellation(cancel);
}

TEST_F(MatchingEngineCancellation,
       SetsLeavingQuantityToZeroInOrderCancellationConfirmation) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_order_quantity(OrderQuantity{420})
                         .with_client_session(client_session)
                         .build_limit_order();
  rest(order);

  const auto cancel = make_cancel(Side::Option::Buy, OrderId{123});

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(
          VariantWith<protocol::OrderCancellationConfirmation>(
              Field(&protocol::OrderCancellationConfirmation::leaving_quantity,
                    Optional(Eq(LeavesQuantity{0})))))));

  cancellation(cancel);
}

TEST_F(MatchingEngineCancellation,
       SetsExecutionIdInOrderCancellationConfirmation) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_client_session(client_session)
                         .build_limit_order();
  rest(order);

  const auto cancel = make_cancel(Side::Option::Buy, OrderId{123});

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::execution_id,
                      Ne(std::nullopt))))));

  cancellation(cancel);
}

TEST_F(MatchingEngineCancellation,
       SetsClientOrderIdInOrderCancellationConfirmation) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_client_session(client_session)
                         .build_limit_order();
  rest(order);

  auto cancel = make_cancel(Side::Option::Buy, OrderId{123});
  cancel.client_order_id = ClientOrderId{"CL-1"};

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::client_order_id,
                      Optional(Eq(ClientOrderId{"CL-1"})))))));

  cancellation(cancel);
}

TEST_F(MatchingEngineCancellation,
       SetsOriginalClientOrderIdInOrderCancellationConfirmation) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_client_session(client_session)
                         .build_limit_order();
  rest(order);

  auto cancel = make_cancel(Side::Option::Buy, OrderId{123});
  cancel.orig_client_order_id = OrigClientOrderId{"ORIG-1"};

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(
          VariantWith<protocol::OrderCancellationConfirmation>(Field(
              &protocol::OrderCancellationConfirmation::orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"ORIG-1"})))))));

  cancellation(cancel);
}

TEST_F(MatchingEngineCancellation, ReturnsRemoveUpdateForCancelledMarketOrder) {
  const auto order = OrderBuilder{}
                         .with_order_id(OrderId{42})
                         .with_side(Side::Option::Buy)
                         .with_order_quantity(OrderQuantity{7})
                         .build_market_order();
  rest(order);

  ASSERT_THAT(
      cancellation(cancel_by_order_id(Side::Option::Buy, OrderId{42})),
      ElementsAre(OrderBookUpdate{.side = Side::Option::Buy,
                                  .action = OrderBookUpdate::Action::Remove,
                                  .price = std::nullopt,
                                  .quantity = order.leaves_quantity()}));
}

TEST_F(MatchingEngineCancellation, ReturnsRemoveUpdateForCancelledLimitOrder) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_client_session(client_session)
                         .build_limit_order();
  rest(order);

  ASSERT_THAT(
      cancellation(make_cancel(Side::Option::Buy, OrderId{123})),
      ElementsAre(OrderBookUpdate{.side = Side::Option::Buy,
                                  .action = OrderBookUpdate::Action::Remove,
                                  .price = order.price(),
                                  .quantity = order.leaves_quantity()}));
}

TEST_F(MatchingEngineCancellation,
       RejectsCancellationOfAnOrderRestingInTheTradeAtLastQueue) {
  order_book.take_page(Side::Option::Buy)
      .trade_at_last_orders()
      .emplace(builder.with_order_id(OrderId{123})
                   .with_side(Side::Option::Buy)
                   .build_limit_order());

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderCancellationReject>(
          Field(&protocol::OrderCancellationReject::reject_text,
                Optional(Eq(RejectText{"order not found"})))))));

  cancellation(cancel_by_order_id(Side::Option::Buy, OrderId{123}));

  ASSERT_THAT(order_book.buy_page().trade_at_last_orders(), SizeIs(1));
}

struct MatchingEngineTradeAtLastCancellation : public Test {
  MatchingEngineTradeAtLastCancellation() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  auto rest_trade_at_last(const LimitOrder& order) -> void {
    order_book.take_page(order.side()).trade_at_last_orders().emplace(order);
  }

  auto rest_regular(const LimitOrder& order) -> void {
    order_book.take_page(order.side()).limit_orders().emplace(order);
  }

  auto identifiable_order() const -> OrderBuilder {
    return OrderBuilder{}
        .with_order_id(OrderId{123})
        .with_side(Side::Option::Buy)
        .with_client_session(client_session)
        .with_client_order_id(ClientOrderId{"CL-1"});
  }

  auto make_cancel() const -> OrderCancel {
    OrderCancel cancel{client_session, Side::Option::Buy};
    cancel.order_id = OrderId{123};
    return cancel;
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;
  Cancellation cancellation{
      event_listener, order_book, std::nullopt, LimitOrderQueue::TradeAtLast};

  const protocol::Session client_session{protocol::generator::Session{}};
};

TEST_F(MatchingEngineTradeAtLastCancellation,
       RejectsCancellationOfAnOrderRestingInTheRegularQueue) {
  rest_regular(identifiable_order().build_limit_order());

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderCancellationReject>(
          Field(&protocol::OrderCancellationReject::reject_text,
                Optional(Eq(RejectText{"order not found"})))))));

  cancellation(make_cancel());

  ASSERT_THAT(order_book.buy_page().limit_orders(), SizeIs(1));
}

TEST_F(MatchingEngineTradeAtLastCancellation,
       EmitsCancellationConfirmationForTradeAtLastOrder) {
  rest_trade_at_last(identifiable_order().build_limit_order());

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::order_status,
                      Optional(Eq(OrderStatus::Option::Cancelled)))))));

  cancellation(make_cancel());
}

TEST_F(MatchingEngineTradeAtLastCancellation,
       RemovesTradeAtLastOrderResolvedByOrderId) {
  rest_trade_at_last(identifiable_order().build_limit_order());

  cancellation(make_cancel());

  ASSERT_THAT(order_book.buy_page().trade_at_last_orders(), IsEmpty());
}

TEST_F(MatchingEngineTradeAtLastCancellation,
       RemovesTradeAtLastOrderResolvedByClientOrderId) {
  rest_trade_at_last(identifiable_order().build_limit_order());

  OrderCancel cancel{client_session, Side::Option::Buy};
  cancel.client_order_id = ClientOrderId{"CL-1"};
  cancellation(cancel);

  ASSERT_THAT(order_book.buy_page().trade_at_last_orders(), IsEmpty());
}

TEST_F(MatchingEngineTradeAtLastCancellation,
       RemovesTradeAtLastOrderResolvedByOrigClientOrderId) {
  rest_trade_at_last(identifiable_order().build_limit_order());

  OrderCancel cancel{client_session, Side::Option::Buy};
  cancel.orig_client_order_id = OrigClientOrderId{"CL-1"};
  cancellation(cancel);

  ASSERT_THAT(order_book.buy_page().trade_at_last_orders(), IsEmpty());
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
