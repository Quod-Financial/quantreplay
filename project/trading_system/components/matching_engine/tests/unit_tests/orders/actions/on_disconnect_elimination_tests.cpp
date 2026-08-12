#include <gmock/gmock.h>

#include "ih/orders/actions/elimination.hpp"
#include "protocol/types/session.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"
#include "tests/tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

struct MatchingEngineOnDisconnectElimination : public Test {
  MatchingEngineOnDisconnectElimination() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  inline static const protocol::Session DisconnectedSession{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                             protocol::fix::SenderCompId{"SENDER1"},
                             protocol::fix::TargetCompId{"TARGET"}}};
  inline static const protocol::Session ConnectedSession{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                             protocol::fix::SenderCompId{"SENDER2"},
                             protocol::fix::TargetCompId{"TARGET"}}};

  auto make_eliminator() -> OnDisconnectElimination {
    return OnDisconnectElimination{
        event_listener, DisconnectedSession, std::nullopt};
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBuilder builder;
  OrderBook order_book;
};

TEST_F(MatchingEngineOnDisconnectElimination,
       DoesNotEliminateBuyOrderFromConnectedSession) {
  order_book.buy_page().limit_orders().emplace(
      builder.with_side(Side{Side::Option::Buy})
          .with_time_in_force(TimeInForce::Option::Day)
          .with_client_session(ConnectedSession)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  eliminator(order_book);

  ASSERT_FALSE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineOnDisconnectElimination,
       DoesNotEliminateSellOrderFromConnectedSession) {
  order_book.sell_page().limit_orders().emplace(
      builder.with_side(Side{Side::Option::Sell})
          .with_time_in_force(TimeInForce::Option::Day)
          .with_client_session(ConnectedSession)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  eliminator(order_book);

  ASSERT_FALSE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineOnDisconnectElimination, DoesNotEliminateBuyNotDayOrders) {
  order_book.buy_page().limit_orders().emplace(
      builder.with_side(Side{Side::Option::Buy})
          .with_time_in_force(TimeInForce::Option::GoodTillCancel)
          .with_client_session(DisconnectedSession)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  eliminator(order_book);

  ASSERT_FALSE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineOnDisconnectElimination,
       DoesNotEliminateSellNotDayOrders) {
  order_book.sell_page().limit_orders().emplace(
      builder.with_side(Side{Side::Option::Sell})
          .with_time_in_force(TimeInForce::Option::GoodTillCancel)
          .with_client_session(DisconnectedSession)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  eliminator(order_book);

  ASSERT_FALSE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineOnDisconnectElimination,
       DoesNotEliminateTradeAtLastOrderFromConnectedSession) {
  order_book.buy_page().trade_at_last_orders().emplace(
      builder.with_side(Side{Side::Option::Buy})
          .with_time_in_force(TimeInForce::Option::Day)
          .with_client_session(ConnectedSession)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  eliminator(order_book);

  ASSERT_FALSE(order_book.buy_page().trade_at_last_orders().empty());
}

TEST_F(MatchingEngineOnDisconnectElimination,
       EliminatesBuyDayOrdersFromDisconnectedSession) {
  order_book.buy_page().limit_orders().emplace(
      builder.with_side(Side{Side::Option::Buy})
          .with_time_in_force(TimeInForce::Option::Day)
          .with_client_session(DisconnectedSession)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  eliminator(order_book);

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineOnDisconnectElimination,
       EliminatesSellDayOrdersFromDisconnectedSession) {
  order_book.sell_page().limit_orders().emplace(
      builder.with_side(Side{Side::Option::Sell})
          .with_time_in_force(TimeInForce::Option::Day)
          .with_client_session(DisconnectedSession)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  eliminator(order_book);

  ASSERT_TRUE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineOnDisconnectElimination,
       EliminatesBuyTradeAtLastOrdersFromDisconnectedSession) {
  order_book.buy_page().trade_at_last_orders().emplace(
      builder.with_side(Side{Side::Option::Buy})
          .with_time_in_force(TimeInForce::Option::Day)
          .with_client_session(DisconnectedSession)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  eliminator(order_book);

  ASSERT_TRUE(order_book.buy_page().trade_at_last_orders().empty());
}

TEST_F(MatchingEngineOnDisconnectElimination,
       EliminatesSellTradeAtLastOrdersFromDisconnectedSession) {
  order_book.sell_page().trade_at_last_orders().emplace(
      builder.with_side(Side{Side::Option::Sell})
          .with_time_in_force(TimeInForce::Option::Day)
          .with_client_session(DisconnectedSession)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  eliminator(order_book);

  ASSERT_TRUE(order_book.sell_page().trade_at_last_orders().empty());
}

TEST_F(MatchingEngineOnDisconnectElimination,
       EmitsOrderRemovedNotificationOnEliminationBuyOrder) {
  order_book.buy_page().limit_orders().emplace(
      builder.with_order_id(OrderId{123})
          .with_side(Side{Side::Option::Buy})
          .with_time_in_force(TimeInForce::Option::Day)
          .with_client_session(DisconnectedSession)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineOnDisconnectElimination,
       EmitsOrderRemovedNotificationOnEliminationSellOrder) {
  order_book.sell_page().limit_orders().emplace(
      builder.with_order_id(OrderId{123})
          .with_side(Side{Side::Option::Sell})
          .with_time_in_force(TimeInForce::Option::Day)
          .with_client_session(DisconnectedSession)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineOnDisconnectElimination,
       EmitsOrderCancellationConfirmationOnEliminationBuyOrder) {
  order_book.buy_page().limit_orders().emplace(
      builder.with_order_id(OrderId{123})
          .with_side(Side{Side::Option::Buy})
          .with_time_in_force(TimeInForce::Option::Day)
          .with_client_session(DisconnectedSession)
          .with_client_order_id(ClientOrderId{"client-123"})
          .build_limit_order());

  const auto eliminator = make_eliminator();

  EXPECT_CALL(event_listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  VenueOrderId{"123"},
                  OrderStatus::Option::Cancelled,
                  LeavesQuantity{420},
                  ClientOrderId{"client-123"}))));

  eliminator(order_book);
}

TEST_F(MatchingEngineOnDisconnectElimination,
       EmitsOrderCancellationConfirmationOnEliminationSellOrder) {
  order_book.sell_page().limit_orders().emplace(
      builder.with_order_id(OrderId{123})
          .with_side(Side{Side::Option::Sell})
          .with_time_in_force(TimeInForce::Option::Day)
          .with_client_session(DisconnectedSession)
          .with_client_order_id(ClientOrderId{"client-123"})
          .build_limit_order());

  const auto eliminator = make_eliminator();

  EXPECT_CALL(event_listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  VenueOrderId{"123"},
                  OrderStatus::Option::Cancelled,
                  LeavesQuantity{420},
                  ClientOrderId{"client-123"}))));

  eliminator(order_book);
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
