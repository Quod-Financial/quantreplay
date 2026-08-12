#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/orders/actions/elimination.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"
#include "tests/tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineAllOrdersElimination : public Test {
  MatchingEngineAllOrdersElimination() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  static auto make_builder(OrderId identifier, Side side) -> OrderBuilder {
    OrderBuilder builder;
    builder.with_order_id(identifier)
        .with_side(side)
        .with_client_order_id(ClientOrderId{"client-123"});
    return builder;
  }

  auto rest_limit_order(OrderId identifier, Side side) -> void {
    order_book.take_page(side).limit_orders().emplace(
        make_builder(identifier, side).build_limit_order());
  }

  auto rest_trade_at_last_order(OrderId identifier, Side side) -> void {
    order_book.take_page(side).trade_at_last_orders().emplace(
        make_builder(identifier, side).build_limit_order());
  }

  auto rest_market_order(OrderId identifier, Side side) -> void {
    order_book.take_page(side).market_orders().emplace(
        make_builder(identifier, side).build_market_order());
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;

  AllOrdersElimination eliminator{event_listener};
};

TEST_F(MatchingEngineAllOrdersElimination, DeletesLimitOrdersFromOrderBook) {
  rest_limit_order(OrderId{1}, Side::Option::Buy);
  rest_limit_order(OrderId{2}, Side::Option::Buy);
  rest_limit_order(OrderId{3}, Side::Option::Sell);
  rest_limit_order(OrderId{4}, Side::Option::Sell);

  eliminator(order_book);

  EXPECT_THAT(order_book.buy_page().limit_orders(), IsEmpty());
  EXPECT_THAT(order_book.sell_page().limit_orders(), IsEmpty());
}

TEST_F(MatchingEngineAllOrdersElimination,
       DeletesTradeAtLastOrdersFromOrderBook) {
  rest_trade_at_last_order(OrderId{1}, Side::Option::Buy);
  rest_trade_at_last_order(OrderId{2}, Side::Option::Buy);
  rest_trade_at_last_order(OrderId{3}, Side::Option::Sell);
  rest_trade_at_last_order(OrderId{4}, Side::Option::Sell);

  eliminator(order_book);

  EXPECT_THAT(order_book.buy_page().trade_at_last_orders(), IsEmpty());
  EXPECT_THAT(order_book.sell_page().trade_at_last_orders(), IsEmpty());
}

TEST_F(MatchingEngineAllOrdersElimination,
       DeletesRestingMarketOrdersFromOrderBook) {
  rest_market_order(OrderId{1}, Side::Option::Buy);
  rest_market_order(OrderId{2}, Side::Option::Sell);

  eliminator(order_book);

  EXPECT_THAT(order_book.buy_page().market_orders(), IsEmpty());
  EXPECT_THAT(order_book.sell_page().market_orders(), IsEmpty());
}

TEST_F(MatchingEngineAllOrdersElimination,
       EmitsOrderRemovedNotificationForEliminatedLimitOrder) {
  rest_limit_order(OrderId{1}, Side::Option::Buy);
  rest_limit_order(OrderId{2}, Side::Option::Sell);

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{1}))))));
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{2}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineAllOrdersElimination,
       EmitsOrderRemovedNotificationForEliminatedTradeAtLastOrder) {
  rest_trade_at_last_order(OrderId{1}, Side::Option::Buy);
  rest_trade_at_last_order(OrderId{2}, Side::Option::Sell);

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{1}))))));
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{2}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineAllOrdersElimination,
       EmitsOrderRemovedNotificationForEliminatedRestingMarketOrder) {
  rest_market_order(OrderId{1}, Side::Option::Buy);
  rest_market_order(OrderId{2}, Side::Option::Sell);

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{1}))))));
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{2}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineAllOrdersElimination,
       DoesNotReportEliminatedOrdersToClients) {
  rest_limit_order(OrderId{1}, Side::Option::Buy);
  rest_trade_at_last_order(OrderId{2}, Side::Option::Buy);
  rest_market_order(OrderId{3}, Side::Option::Sell);

  EXPECT_CALL(event_listener, on(IsClientNotification(_))).Times(0);

  eliminator(order_book);
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
