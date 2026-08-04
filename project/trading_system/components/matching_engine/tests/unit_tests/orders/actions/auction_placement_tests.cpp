#include <gmock/gmock.h>

#include "ih/orders/actions/auction_placement.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineAuctionPlacement : public Test {
  MatchingEngineAuctionPlacement() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;
  AuctionPlacement auction_placement{event_listener, order_book};
};

struct MatchingEngineAuctionPlacementLimitOrder
    : public MatchingEngineAuctionPlacement {
  static constexpr OrderId order_id{42};

  static auto limit_order() -> LimitOrder {
    return OrderBuilder{}.with_order_id(order_id).build_limit_order();
  }

  static auto resting_limit_order(Side side, OrderPrice price, OrderId id)
      -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(id)
        .with_side(side)
        .with_order_price(price)
        .build_limit_order();
  }

  static auto ioc_limit_order() -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
        .build_limit_order();
  }

  static auto fok_limit_order() -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_time_in_force(TimeInForce::Option::FillOrKill)
        .build_limit_order();
  }

  static auto gtc_limit_order() -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_time_in_force(TimeInForce::Option::GoodTillCancel)
        .build_limit_order();
  }

  static auto gtd_limit_order() -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_time_in_force(TimeInForce::Option::GoodTillDate)
        .build_limit_order();
  }
};

TEST_F(MatchingEngineAuctionPlacementLimitOrder, EmitsPlacementConfirmation) {
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementConfirmation>(
          Field(&protocol::OrderPlacementConfirmation::execution_id,
                Optional(Eq(
                    ExecutionId{std::to_string(order_id.value()) + "-1"})))))));

  auction_placement(limit_order());
}

TEST_F(MatchingEngineAuctionPlacementLimitOrder,
       EmitsOrderAddedNotificationWithPrice) {
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(AllOf(
                  Field(&OrderAdded::order_id, Eq(order_id)),
                  Field(&OrderAdded::order_price, Optional(Eq(Price{42}))))))));

  auction_placement(limit_order());
}

TEST_F(MatchingEngineAuctionPlacementLimitOrder, RestsLimitOrderInBook) {
  auction_placement(limit_order());

  ASSERT_THAT(order_book.buy_page().limit_orders().size(), Eq(1));
}

TEST_F(MatchingEngineAuctionPlacementLimitOrder,
       DoesNotCrossExistingFacingOrder) {
  auction_placement(
      resting_limit_order(Side::Option::Sell, OrderPrice{40}, OrderId{1}));

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);

  auction_placement(
      resting_limit_order(Side::Option::Buy, OrderPrice{50}, OrderId{2}));

  EXPECT_THAT(order_book.buy_page().limit_orders().size(), Eq(1));
  EXPECT_THAT(order_book.sell_page().limit_orders().size(), Eq(1));
}

TEST_F(MatchingEngineAuctionPlacementLimitOrder,
       RejectsImmediateOrCancelLimitOrder) {
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(Field(
          &protocol::OrderPlacementReject::reject_text,
          Optional(Eq(RejectText{
              "immediate-or-cancel and fill-or-kill orders are not accepted "
              "during an auction"})))))));

  auction_placement(ioc_limit_order());
}

TEST_F(MatchingEngineAuctionPlacementLimitOrder, RejectsFillOrKillLimitOrder) {
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(Field(
          &protocol::OrderPlacementReject::reject_text,
          Optional(Eq(RejectText{
              "immediate-or-cancel and fill-or-kill orders are not accepted "
              "during an auction"})))))));

  auction_placement(fok_limit_order());
}

TEST_F(MatchingEngineAuctionPlacementLimitOrder,
       DoesNotRestRejectedNonRestingOrder) {
  auction_placement(ioc_limit_order());

  ASSERT_THAT(order_book.buy_page().limit_orders().size(), Eq(0));
}

TEST_F(MatchingEngineAuctionPlacementLimitOrder, AcceptsGoodTillCancelOrder) {
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(_))))
      .Times(0);

  auction_placement(gtc_limit_order());

  ASSERT_THAT(order_book.buy_page().limit_orders().size(), Eq(1));
}

TEST_F(MatchingEngineAuctionPlacementLimitOrder, AcceptsGoodTillDateOrder) {
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(_))))
      .Times(0);

  auction_placement(gtd_limit_order());

  ASSERT_THAT(order_book.buy_page().limit_orders().size(), Eq(1));
}

TEST_F(MatchingEngineAuctionPlacementLimitOrder,
       ReturnsNoUpdatesForRejectedImmediateOrCancelOrder) {
  ASSERT_THAT(auction_placement(ioc_limit_order()), IsEmpty());
}

TEST_F(MatchingEngineAuctionPlacementLimitOrder,
       ReturnsNoUpdatesForRejectedFillOrKillOrder) {
  ASSERT_THAT(auction_placement(fok_limit_order()), IsEmpty());
}

TEST_F(MatchingEngineAuctionPlacementLimitOrder,
       ReturnsAddUpdateForRestingOrder) {
  const auto order = limit_order();

  ASSERT_THAT(
      auction_placement(order),
      ElementsAre(OrderBookUpdate{.side = order.side(),
                                  .action = OrderBookUpdate::Action::Add,
                                  .price = order.price(),
                                  .quantity = order.leaves_quantity()}));
}

struct MatchingEngineAuctionPlacementMarketOrder
    : public MatchingEngineAuctionPlacement {
  static constexpr OrderId order_id{42};

  static auto market_order() -> MarketOrder {
    return OrderBuilder{}.with_order_id(order_id).build_market_order();
  }
};

TEST_F(MatchingEngineAuctionPlacementMarketOrder, EmitsPlacementConfirmation) {
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementConfirmation>(
          Field(&protocol::OrderPlacementConfirmation::execution_id,
                Optional(Eq(
                    ExecutionId{std::to_string(order_id.value()) + "-1"})))))));

  auction_placement(market_order());
}

TEST_F(MatchingEngineAuctionPlacementMarketOrder,
       PublishesOrderAddedNotificationWithoutPrice) {
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(
                  AllOf(Field(&OrderAdded::order_id, Eq(order_id)),
                        Field(&OrderAdded::order_price, Eq(std::nullopt)))))));

  auction_placement(market_order());
}

TEST_F(MatchingEngineAuctionPlacementMarketOrder, RestsMarketOrderInBook) {
  auction_placement(market_order());

  EXPECT_THAT(order_book.buy_page().market_orders().size(), Eq(1));
  EXPECT_THAT(order_book.buy_page().limit_orders().size(), Eq(0));
}

TEST_F(MatchingEngineAuctionPlacementMarketOrder, DoesNotRejectMarketOrder) {
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(_))))
      .Times(0);

  auction_placement(market_order());
}

TEST_F(MatchingEngineAuctionPlacementMarketOrder,
       ReturnsAddUpdateForRestingOrder) {
  const auto order = market_order();

  ASSERT_THAT(
      auction_placement(order),
      ElementsAre(OrderBookUpdate{.side = order.side(),
                                  .action = OrderBookUpdate::Action::Add,
                                  .price = std::nullopt,
                                  .quantity = order.leaves_quantity()}));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
