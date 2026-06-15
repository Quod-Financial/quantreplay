#include <gmock/gmock.h>

#include "ih/orders/actions/cancellation.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "protocol/types/session.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"
#include "tests/tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineCancellation : public Test {
  NiceMock<EventListenerMock> event_listener;
  OrderBuilder builder;
  OrderBook order_book;
  protocol::Session session{protocol::generator::Session{}};
  Cancellation cancellation{event_listener, order_book, std::nullopt};

  MatchingEngineCancellation() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  auto make_cancel(Side side, OrderId order_id) const -> OrderCancel {
    auto cancel = OrderCancel{session, side};
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

TEST_F(MatchingEngineCancellation, DeletesOrderFromOrderBook) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_client_session(session)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto cancel = make_cancel(Side::Option::Buy, OrderId{123});

  cancellation(cancel);

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineCancellation,
       EmitsOrderRemovedNotificationIfOrderRemoved) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_client_session(session)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

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
                         .with_client_session(session)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

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
                         .with_client_session(session)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

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
                         .with_client_session(session)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

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
                         .with_client_session(session)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

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
                         .with_client_session(session)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

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
                         .with_client_session(session)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

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

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
