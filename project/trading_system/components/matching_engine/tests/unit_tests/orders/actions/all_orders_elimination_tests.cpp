#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/orders/actions/elimination.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"
#include "tests/tools/order_test_tools.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

struct MatchingEngineAllOrdersElimination : public Test {
  NiceMock<EventListenerMock> event_listener;
  OrderBuilder builder;
  OrderBook order_book;

  AllOrdersElimination eliminator{event_listener};
};

TEST_F(MatchingEngineAllOrdersElimination, DeletesBuySideOrdersFromOrderBook) {
  const auto order1 =
      builder.with_side(Side{Side::Option::Buy}).build_limit_order();
  const auto order2 =
      builder.with_side(Side{Side::Option::Buy}).build_limit_order();

  order_book.buy_page().limit_orders().emplace(order1);
  order_book.buy_page().limit_orders().emplace(order2);

  eliminator(order_book);

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineAllOrdersElimination,
       EmitsOrderRemovedNotificationOnEliminationBuySideOrderFromOrderBook) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineAllOrdersElimination, DeletesSellSideOrdersFromOrderBook) {
  const auto order1 =
      builder.with_side(Side{Side::Option::Sell}).build_limit_order();
  const auto order2 =
      builder.with_side(Side{Side::Option::Sell}).build_limit_order();
  order_book.sell_page().limit_orders().emplace(order1);
  order_book.sell_page().limit_orders().emplace(order2);

  eliminator(order_book);

  ASSERT_TRUE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineAllOrdersElimination,
       EmitsOrderRemovedNotificationOnEliminationSellSideOrderFromOrderBook) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
