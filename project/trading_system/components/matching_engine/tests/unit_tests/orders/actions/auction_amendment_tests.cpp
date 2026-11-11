#include <gmock/gmock.h>

#include <chrono>
#include <optional>
#include <utility>

#include "core/tools/time.hpp"
#include "ih/common/data/market_data_updates.hpp"
#include "ih/orders/actions/auction_amendment.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineAuctionAmendment : public Test {
  MatchingEngineAuctionAmendment() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;
  AuctionAmendment amendment{event_listener, order_book, std::nullopt};

  static auto arrival(int seconds_since_epoch) -> OrderTime {
    return OrderTime{core::sys_us{std::chrono::seconds{seconds_since_epoch}}};
  }

  auto rest_limit_order(OrderId order_id, OrderPrice price, OrderTime time)
      -> void {
    limit_orders().emplace(OrderBuilder{}
                               .with_order_id(order_id)
                               .with_side(Side::Option::Buy)
                               .with_order_price(price)
                               .with_order_quantity(OrderQuantity{100})
                               .with_order_time(time)
                               .build_limit_order());
  }

  auto rest_market_order(OrderId order_id, OrderTime time) -> void {
    market_orders().emplace(
        OrderBuilder{}
            .with_order_id(order_id)
            .with_side(Side::Option::Buy)
            .with_order_quantity(OrderQuantity{100})
            .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
            .with_order_time(time)
            .build_market_order());
  }

  auto limit_orders() -> LimitOrdersContainer& {
    return order_book.buy_page().limit_orders();
  }

  auto market_orders() -> MarketOrdersContainer& {
    return order_book.buy_page().market_orders();
  }

  static auto market_update(OrderQuantity quantity) -> MarketUpdate {
    OrderAttributes attributes;
    attributes.set_time_in_force(TimeInForce::Option::ImmediateOrCancel);
    return MarketUpdate{
        protocol::Session{protocol::generator::Session{}},
        Side::Option::Buy,
        MarketOrder::Update{.quantity = quantity,
                            .attributes = std::move(attributes)}};
  }

  static auto limit_update(OrderPrice price, OrderQuantity quantity)
      -> LimitUpdate {
    OrderAttributes attributes;
    attributes.set_time_in_force(TimeInForce::Option::Day);
    LimitUpdate update{protocol::Session{protocol::generator::Session{}},
                       Side::Option::Buy,
                       LimitOrder::Update{.price = price,
                                          .quantity = quantity,
                                          .attributes = std::move(attributes)}};
    update.time_in_force_specified = true;
    return update;
  }

  static auto to_market(OrderId order_id,
                        OrderQuantity quantity = OrderQuantity{100})
      -> MarketUpdate {
    auto update = market_update(quantity);
    update.order_id = order_id;
    return update;
  }

  static auto to_limit(OrderId order_id,
                       OrderPrice price,
                       OrderQuantity quantity = OrderQuantity{100})
      -> LimitUpdate {
    auto update = limit_update(price, quantity);
    update.order_id = order_id;
    return update;
  }
};

TEST_F(MatchingEngineAuctionAmendment,
       RejectsAmendmentToMarketOfUnknownOrderAsOrderNotFound) {
  rest_limit_order(OrderId{1}, OrderPrice{10}, arrival(1));
  rest_market_order(OrderId{2}, arrival(2));

  EXPECT_CALL(event_listener, on(IsOrderModificationReject("order not found")));

  amendment(to_market(OrderId{99}));
}

TEST_F(MatchingEngineAuctionAmendment,
       RejectsAmendmentToLimitOfUnknownOrderAsOrderNotFound) {
  rest_limit_order(OrderId{1}, OrderPrice{10}, arrival(1));
  rest_market_order(OrderId{2}, arrival(2));

  EXPECT_CALL(event_listener, on(IsOrderModificationReject("order not found")));

  amendment(to_limit(OrderId{99}, OrderPrice{10}));
}

TEST_F(MatchingEngineAuctionAmendment,
       RestsLimitOrderAmendedToMarketBehindRestingMarketOrders) {
  rest_limit_order(OrderId{1}, OrderPrice{10}, arrival(1));
  rest_market_order(OrderId{2}, arrival(2));

  amendment(to_market(OrderId{1}));

  EXPECT_THAT(market_orders(),
              ElementsAre(Property(&MarketOrder::id, Eq(OrderId{2})),
                          Property(&MarketOrder::id, Eq(OrderId{1}))));
}

TEST_F(MatchingEngineAuctionAmendment,
       RemovesLimitOrderAmendedToMarketFromLimitOrders) {
  rest_limit_order(OrderId{1}, OrderPrice{10}, arrival(1));

  amendment(to_market(OrderId{1}));

  EXPECT_THAT(limit_orders(), IsEmpty());
}

TEST_F(MatchingEngineAuctionAmendment,
       EmitsMarketDepthEntryForLimitOrderAmendedToMarket) {
  rest_limit_order(OrderId{1}, OrderPrice{10}, arrival(1));

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(AllOf(
                  Field(&OrderAdded::order_id, Eq(OrderId{1})),
                  Field(&OrderAdded::order_type, Eq(OrderType::Option::Market)),
                  Field(&OrderAdded::order_price, Eq(std::nullopt)))))));

  amendment(to_market(OrderId{1}));
}

TEST_F(MatchingEngineAuctionAmendment,
       ReturnsLimitRemovalAndMarketAdditionForLimitOrderAmendedToMarket) {
  rest_limit_order(OrderId{1}, OrderPrice{10}, arrival(1));

  ASSERT_THAT(
      amendment(to_market(OrderId{1}, OrderQuantity{80})),
      ElementsAre(OrderBookUpdate{.side = Side::Option::Buy,
                                  .action = OrderBookUpdate::Action::Remove,
                                  .price = OrderPrice{10},
                                  .quantity = LeavesQuantity{100}},
                  OrderBookUpdate{.side = Side::Option::Buy,
                                  .action = OrderBookUpdate::Action::Add,
                                  .price = std::nullopt,
                                  .quantity = LeavesQuantity{80}}));
}

TEST_F(MatchingEngineAuctionAmendment,
       RestsMarketOrderAmendedToLimitBehindOlderOrderAtSamePrice) {
  rest_market_order(OrderId{1}, arrival(1));
  rest_limit_order(OrderId{2}, OrderPrice{10}, arrival(2));

  amendment(to_limit(OrderId{1}, OrderPrice{10}));

  EXPECT_THAT(
      limit_orders(),
      ElementsAre(Property(&LimitOrder::id, Eq(OrderId{2})),
                  AllOf(Property(&LimitOrder::id, Eq(OrderId{1})),
                        Property(&LimitOrder::price, Eq(OrderPrice{10})))));
}

TEST_F(MatchingEngineAuctionAmendment,
       RemovesMarketOrderAmendedToLimitFromMarketOrders) {
  rest_market_order(OrderId{1}, arrival(1));

  amendment(to_limit(OrderId{1}, OrderPrice{10}));

  EXPECT_THAT(market_orders(), IsEmpty());
}

TEST_F(MatchingEngineAuctionAmendment,
       EmitsLimitDepthEntryForMarketOrderAmendedToLimit) {
  rest_market_order(OrderId{1}, arrival(1));

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(AllOf(
                  Field(&OrderAdded::order_id, Eq(OrderId{1})),
                  Field(&OrderAdded::order_type, Eq(OrderType::Option::Limit)),
                  Field(&OrderAdded::order_price, Optional(Eq(Price{10}))))))));

  amendment(to_limit(OrderId{1}, OrderPrice{10}));
}

TEST_F(MatchingEngineAuctionAmendment,
       ReturnsMarketRemovalAndLimitAdditionForMarketOrderAmendedToLimit) {
  rest_market_order(OrderId{1}, arrival(1));

  ASSERT_THAT(
      amendment(to_limit(OrderId{1}, OrderPrice{10}, OrderQuantity{80})),
      ElementsAre(OrderBookUpdate{.side = Side::Option::Buy,
                                  .action = OrderBookUpdate::Action::Remove,
                                  .price = std::nullopt,
                                  .quantity = LeavesQuantity{100}},
                  OrderBookUpdate{.side = Side::Option::Buy,
                                  .action = OrderBookUpdate::Action::Add,
                                  .price = OrderPrice{10},
                                  .quantity = LeavesQuantity{80}}));
}

struct MatchingEngineAuctionAmendmentOfSharedClientOrderId
    : public MatchingEngineAuctionAmendment {
  MatchingEngineAuctionAmendmentOfSharedClientOrderId() {
    limit_orders().emplace(OrderBuilder{}
                               .with_order_id(OrderId{1})
                               .with_side(Side::Option::Buy)
                               .with_client_order_id(ClientOrderId{"shared"})
                               .with_order_price(OrderPrice{10})
                               .with_order_quantity(OrderQuantity{100})
                               .build_limit_order());
    market_orders().emplace(
        OrderBuilder{}
            .with_order_id(OrderId{2})
            .with_side(Side::Option::Buy)
            .with_client_order_id(ClientOrderId{"shared"})
            .with_order_quantity(OrderQuantity{100})
            .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
            .build_market_order());
  }

  static auto shared_to_market(OrderQuantity quantity) -> MarketUpdate {
    auto update = market_update(quantity);
    update.orig_client_order_id = OrigClientOrderId{"shared"};
    return update;
  }

  static auto shared_to_limit(OrderPrice price) -> LimitUpdate {
    auto update = limit_update(price, OrderQuantity{100});
    update.orig_client_order_id = OrigClientOrderId{"shared"};
    return update;
  }
};

TEST_F(MatchingEngineAuctionAmendmentOfSharedClientOrderId,
       PrefersMarketOrderForAmendmentToMarket) {
  amendment(shared_to_market(OrderQuantity{50}));

  EXPECT_THAT(market_orders(),
              ElementsAre(AllOf(Property(&MarketOrder::id, Eq(OrderId{2})),
                                Property(&MarketOrder::total_quantity,
                                         Eq(OrderQuantity{50})))));
  EXPECT_THAT(limit_orders(),
              ElementsAre(Property(&LimitOrder::id, Eq(OrderId{1}))));
}

TEST_F(MatchingEngineAuctionAmendmentOfSharedClientOrderId,
       PrefersLimitOrderForAmendmentToLimit) {
  amendment(shared_to_limit(OrderPrice{20}));

  EXPECT_THAT(
      limit_orders(),
      ElementsAre(AllOf(Property(&LimitOrder::id, Eq(OrderId{1})),
                        Property(&LimitOrder::price, Eq(OrderPrice{20})))));
  EXPECT_THAT(market_orders(),
              ElementsAre(Property(&MarketOrder::id, Eq(OrderId{2}))));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
