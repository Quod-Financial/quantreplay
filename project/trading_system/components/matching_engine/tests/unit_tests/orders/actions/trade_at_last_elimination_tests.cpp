#include <gmock/gmock.h>

#include <optional>

#include "ih/orders/actions/elimination.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"
#include "tests/tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineTradeAtLastElimination : public Test {
  MatchingEngineTradeAtLastElimination() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  static auto make_order(OrderId identifier, Side side) -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(identifier)
        .with_side(side)
        .with_client_order_id(ClientOrderId{"client-123"})
        .build_limit_order();
  }

  auto rest_trade_at_last(OrderId identifier, Side side) -> void {
    trade_at_last_orders(side).emplace(make_order(identifier, side));
  }

  auto rest_partially_filled_trade_at_last(OrderId identifier,
                                           ExecutedQuantity filled) -> void {
    auto resting = make_order(identifier, Side::Option::Buy);
    resting.execute(filled, ExecutionPrice{42});
    trade_at_last_orders(Side::Option::Buy).emplace(resting);
  }

  auto rest_regular(OrderId identifier, Side side) -> void {
    order_book.take_page(side).limit_orders().emplace(
        make_order(identifier, side));
  }

  auto trade_at_last_orders(Side side) -> LimitOrdersContainer& {
    return order_book.take_page(side).trade_at_last_orders();
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;
  TradeAtLastElimination elimination{event_listener, std::nullopt};
};

TEST_F(MatchingEngineTradeAtLastElimination, ExpiresAllBuyOrders) {
  rest_trade_at_last(OrderId{1}, Side::Option::Buy);
  rest_trade_at_last(OrderId{2}, Side::Option::Buy);

  elimination(order_book);

  ASSERT_THAT(trade_at_last_orders(Side::Option::Buy), IsEmpty());
}

TEST_F(MatchingEngineTradeAtLastElimination, ExpiresAllSellOrders) {
  rest_trade_at_last(OrderId{1}, Side::Option::Sell);
  rest_trade_at_last(OrderId{2}, Side::Option::Sell);

  elimination(order_book);

  ASSERT_THAT(trade_at_last_orders(Side::Option::Sell), IsEmpty());
}

TEST_F(MatchingEngineTradeAtLastElimination,
       KeepsOrdersRestingInRegularQueues) {
  rest_regular(OrderId{1}, Side::Option::Buy);
  rest_regular(OrderId{2}, Side::Option::Sell);

  elimination(order_book);

  EXPECT_THAT(order_book.buy_page().limit_orders(), SizeIs(1));
  EXPECT_THAT(order_book.sell_page().limit_orders(), SizeIs(1));
}

TEST_F(MatchingEngineTradeAtLastElimination,
       EmitsOrderRemovedNotificationForEachExpiredOrder) {
  rest_trade_at_last(OrderId{1}, Side::Option::Buy);
  rest_trade_at_last(OrderId{2}, Side::Option::Sell);

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{1}))))));
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{2}))))));

  elimination(order_book);
}

TEST_F(MatchingEngineTradeAtLastElimination,
       EmitsCancellationConfirmationForExpiredOrder) {
  rest_trade_at_last(OrderId{123}, Side::Option::Buy);

  EXPECT_CALL(event_listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  VenueOrderId{"123"},
                  OrderStatus::Option::Cancelled,
                  LeavesQuantity{420},
                  ClientOrderId{"client-123"}))));

  elimination(order_book);
}

TEST_F(MatchingEngineTradeAtLastElimination,
       ReportsRemainingQuantityOfPartiallyFilledOrder) {
  rest_partially_filled_trade_at_last(OrderId{123}, ExecutedQuantity{120});

  EXPECT_CALL(event_listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  VenueOrderId{"123"},
                  OrderStatus::Option::Cancelled,
                  LeavesQuantity{300},
                  ClientOrderId{"client-123"}))));

  elimination(order_book);
}

TEST_F(MatchingEngineTradeAtLastElimination,
       EmitsNothingWhenTradeAtLastQueuesAreEmpty) {
  rest_regular(OrderId{1}, Side::Option::Buy);

  EXPECT_CALL(event_listener, on(_)).Times(0);

  elimination(order_book);
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
