#include <gmock/gmock.h>

#include <optional>

#include "ih/orders/actions/order_actions.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineOrderActions : public Test {
  MatchingEngineOrderActions() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;

  auto place_limit(LimitOrder order, OrderActionMode mode) -> void {
    place_limit_order(
        event_listener, order_book, std::nullopt, std::move(order), mode);
  }

  auto place_market(MarketOrder order, OrderActionMode mode) -> void {
    place_market_order(
        event_listener, order_book, std::nullopt, std::move(order), mode);
  }

  auto amend_limit(LimitUpdate update, OrderActionMode mode) -> void {
    amend_limit_order(
        event_listener, order_book, std::nullopt, std::move(update), mode);
  }

  auto amend_market(MarketUpdate update) -> void {
    amend_market_order(
        event_listener, order_book, std::nullopt, std::move(update));
  }

  auto rest_limit(Side side, OrderPrice price, OrderId order_id) -> void {
    order_book.take_page(side).limit_orders().emplace(
        OrderBuilder{}
            .with_order_id(order_id)
            .with_side(side)
            .with_order_price(price)
            .build_limit_order());
  }

  static auto limit_order(Side side,
                          OrderPrice price,
                          OrderId order_id) -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_side(side)
        .with_order_price(price)
        .build_limit_order();
  }

  static auto market_order(Side side, OrderId order_id) -> MarketOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_side(side)
        .build_market_order();
  }

  static auto reprice_limit(Side side,
                            OrderPrice price,
                            OrderId order_id) -> LimitUpdate {
    OrderAttributes attributes;
    attributes.set_time_in_force(TimeInForce::Option::Day);
    LimitUpdate update{protocol::Session{protocol::generator::Session{}},
                       side,
                       LimitOrder::Update{.price = price,
                                          .quantity = OrderQuantity{420},
                                          .attributes = std::move(attributes)}};
    update.order_id = order_id;
    return update;
  }
};

TEST_F(MatchingEngineOrderActions, CrossesPlacedLimitOrderInRegularMode) {
  rest_limit(Side::Option::Sell, OrderPrice{40}, OrderId{1});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(1);

  place_limit(limit_order(Side::Option::Buy, OrderPrice{50}, OrderId{2}),
              OrderActionMode::Regular);
}

TEST_F(MatchingEngineOrderActions,
       RestsCrossableLimitOrderWithoutCrossingInAuctionCallMode) {
  rest_limit(Side::Option::Sell, OrderPrice{40}, OrderId{1});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);

  place_limit(limit_order(Side::Option::Buy, OrderPrice{50}, OrderId{2}),
              OrderActionMode::AuctionCall);

  EXPECT_THAT(order_book.buy_page().limit_orders().size(), Eq(1));
  EXPECT_THAT(order_book.sell_page().limit_orders().size(), Eq(1));
}

TEST_F(MatchingEngineOrderActions, RestsMarketOrderInAuctionCallMode) {
  place_market(market_order(Side::Option::Buy, OrderId{1}),
               OrderActionMode::AuctionCall);

  EXPECT_THAT(order_book.buy_page().market_orders().size(), Eq(1));
  EXPECT_THAT(order_book.buy_page().limit_orders().size(), Eq(0));
}

TEST_F(MatchingEngineOrderActions,
       RepricesLimitOrderWithoutCrossingInAuctionCallMode) {
  rest_limit(Side::Option::Sell, OrderPrice{40}, OrderId{1});
  rest_limit(Side::Option::Buy, OrderPrice{30}, OrderId{2});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);

  amend_limit(reprice_limit(Side::Option::Buy, OrderPrice{50}, OrderId{2}),
              OrderActionMode::AuctionCall);

  auto& buy_orders = order_book.buy_page().limit_orders();
  ASSERT_THAT(buy_orders.size(), Eq(1));
  EXPECT_THAT(buy_orders.begin()->price(), Eq(OrderPrice{50}));
  EXPECT_THAT(order_book.sell_page().limit_orders().size(), Eq(1));
}

TEST_F(MatchingEngineOrderActions,
       RoutesMarketAmendmentToMarketAmendmentOperation) {
  auto resting = OrderBuilder{}
                     .with_order_id(OrderId{1})
                     .with_side(Side::Option::Buy)
                     .with_order_quantity(OrderQuantity{10})
                     .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
                     .build_market_order();
  order_book.take_page(Side::Option::Buy).market_orders().emplace(resting);

  OrderAttributes attributes;
  attributes.set_time_in_force(TimeInForce::Option::ImmediateOrCancel);
  MarketUpdate update{protocol::Session{protocol::generator::Session{}},
                      Side::Option::Buy,
                      MarketOrder::Update{.quantity = OrderQuantity{5},
                                          .attributes = std::move(attributes)}};
  update.order_id = OrderId{1};

  amend_market(std::move(update));

  auto& market_orders = order_book.buy_page().market_orders();
  ASSERT_THAT(market_orders.size(), Eq(1));
  EXPECT_THAT(market_orders.begin()->total_quantity(), Eq(OrderQuantity{5}));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
