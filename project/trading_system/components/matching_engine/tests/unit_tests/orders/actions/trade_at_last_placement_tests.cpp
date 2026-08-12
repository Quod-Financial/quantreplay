#include <gmock/gmock.h>

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

#include "core/domain/market_phase.hpp"
#include "core/tools/time.hpp"
#include "ih/orders/actions/trade_at_last_actions.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/matchers/regular_order_matcher.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

[[nodiscard]]
auto RejectedWith(std::string_view reason) {
  return IsClientNotification(VariantWith<protocol::OrderPlacementReject>(
      Field(&protocol::OrderPlacementReject::reject_text,
            Optional(Eq(RejectText{std::string{reason}})))));
}

struct MatchingEngineTradeAtLastMarketOrderRejection : public Test {
  static auto market_order() -> MarketOrder {
    return OrderBuilder{}.with_order_id(OrderId{1}).build_market_order();
  }

  NiceMock<EventListenerMock> event_listener;
};

TEST_F(MatchingEngineTradeAtLastMarketOrderRejection,
       ReportsThatOnlyLimitOrdersAreAccepted) {
  auto order = market_order();

  EXPECT_CALL(event_listener,
              on(RejectedWith(trade_at_last::LimitOrdersOnlyReject)));

  trade_at_last::reject_market_order(event_listener, order);
}

struct MatchingEngineTradeAtLastPlacement : public Test {
  MatchingEngineTradeAtLastPlacement() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  constexpr static Price ClosingPrice{42};
  constexpr static MarketPhase TradeAtLastPhase{
      TradingPhase::Option::PostTrading, TradingStatus::Option::Resume};

  static auto order(Side side,
                    OrderId identifier,
                    OrderQuantity quantity = OrderQuantity{100},
                    OrderTime time = arrival(1)) -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(identifier)
        .with_side(side)
        .with_order_quantity(quantity)
        .with_order_price(OrderPrice{ClosingPrice.value()})
        .with_order_time(time)
        .build_limit_order();
  }

  static auto order_priced(Side side, OrderPrice price) -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(OrderId{1})
        .with_side(side)
        .with_order_price(price)
        .build_limit_order();
  }

  static auto order_with(TimeInForce time_in_force) -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(OrderId{1})
        .with_time_in_force(time_in_force)
        .with_order_price(OrderPrice{ClosingPrice.value()})
        .build_limit_order();
  }

  static auto arrival(int seconds_since_epoch) -> OrderTime {
    return OrderTime{core::sys_us{std::chrono::seconds{seconds_since_epoch}}};
  }

  auto rest_in_regular_queue(Side side, OrderPrice price) -> void {
    order_book.take_page(side).limit_orders().emplace(
        order_priced(side, price));
  }

  auto trade_at_last_orders(Side side) -> LimitOrdersContainer& {
    return order_book.take_page(side).trade_at_last_orders();
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;
  RegularOrderMatcher matcher{event_listener,
                              order_book,
                              std::nullopt,
                              TradeAtLastPhase,
                              LimitOrderQueue::TradeAtLast};
};

struct MatchingEngineTradeAtLastPlacementWithClosingPrice
    : public MatchingEngineTradeAtLastPlacement {
  TradeAtLastPlacement placement{
      event_listener, order_book, matcher, ClosingPrice};
};

TEST_F(MatchingEngineTradeAtLastPlacementWithClosingPrice,
       RejectsLimitOrderPricedAboveTheClosingPrice) {
  EXPECT_CALL(event_listener,
              on(RejectedWith(trade_at_last::PriceNotAtClosingPriceReject)));

  placement(order_priced(Side::Option::Buy, OrderPrice{43}));
}

TEST_F(MatchingEngineTradeAtLastPlacementWithClosingPrice,
       RejectsLimitOrderPricedBelowTheClosingPrice) {
  EXPECT_CALL(event_listener,
              on(RejectedWith(trade_at_last::PriceNotAtClosingPriceReject)));

  placement(order_priced(Side::Option::Buy, OrderPrice{41}));
}

TEST_F(MatchingEngineTradeAtLastPlacementWithClosingPrice,
       DoesNotRestRejectedOrder) {
  placement(order_priced(Side::Option::Buy, OrderPrice{41}));

  ASSERT_THAT(trade_at_last_orders(Side::Option::Buy), IsEmpty());
}

TEST_F(MatchingEngineTradeAtLastPlacementWithClosingPrice,
       ConfirmsDayLimitOrderAtTheClosingPrice) {
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderPlacementConfirmation>(_))));

  placement(order(Side::Option::Buy, OrderId{1}));
}

TEST_F(MatchingEngineTradeAtLastPlacementWithClosingPrice,
       RestsAcceptedOrderInTheTradeAtLastQueue) {
  placement(order(Side::Option::Buy, OrderId{1}));

  EXPECT_THAT(trade_at_last_orders(Side::Option::Buy),
              ElementsAre(Property(&LimitOrder::id, Eq(OrderId{1}))));
  EXPECT_THAT(order_book.buy_page().limit_orders(), IsEmpty());
}

TEST_F(MatchingEngineTradeAtLastPlacementWithClosingPrice,
       CrossesIncomingOrderAgainstTheOppositeTradeAtLastQueue) {
  placement(order(Side::Option::Sell, OrderId{1}));

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(
                  AllOf(Field(&Trade::trade_price, Eq(ClosingPrice)),
                        Field(&Trade::traded_quantity, Eq(Quantity{100})))))));

  placement(
      order(Side::Option::Buy, OrderId{2}, OrderQuantity{100}, arrival(2)));
}

TEST_F(MatchingEngineTradeAtLastPlacementWithClosingPrice,
       LeavesBothQueuesEmptyAfterAFullCross) {
  placement(order(Side::Option::Sell, OrderId{1}));
  placement(
      order(Side::Option::Buy, OrderId{2}, OrderQuantity{100}, arrival(2)));

  EXPECT_THAT(trade_at_last_orders(Side::Option::Buy), IsEmpty());
  EXPECT_THAT(trade_at_last_orders(Side::Option::Sell), IsEmpty());
}

TEST_F(MatchingEngineTradeAtLastPlacementWithClosingPrice,
       StampsCrossTradesWithTheTradeAtLastPhase) {
  placement(order(Side::Option::Sell, OrderId{1}));

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(
                  Field(&Trade::market_phase, Eq(TradeAtLastPhase))))));

  placement(
      order(Side::Option::Buy, OrderId{2}, OrderQuantity{100}, arrival(2)));
}

TEST_F(MatchingEngineTradeAtLastPlacementWithClosingPrice,
       RestsTheRemainderWhenTheOppositeQueueIsExhausted) {
  placement(order(Side::Option::Sell, OrderId{1}, OrderQuantity{40}));

  placement(
      order(Side::Option::Buy, OrderId{2}, OrderQuantity{100}, arrival(2)));

  EXPECT_THAT(trade_at_last_orders(Side::Option::Sell), IsEmpty());
  EXPECT_THAT(trade_at_last_orders(Side::Option::Buy),
              ElementsAre(Property(&LimitOrder::leaves_quantity,
                                   Eq(LeavesQuantity{60}))));
}

TEST_F(MatchingEngineTradeAtLastPlacementWithClosingPrice,
       FillsTheOldestRestingOrderFirst) {
  placement(
      order(Side::Option::Buy, OrderId{1}, OrderQuantity{100}, arrival(1)));
  placement(
      order(Side::Option::Buy, OrderId{2}, OrderQuantity{100}, arrival(2)));

  placement(
      order(Side::Option::Sell, OrderId{3}, OrderQuantity{100}, arrival(3)));

  ASSERT_THAT(trade_at_last_orders(Side::Option::Buy),
              ElementsAre(Property(&LimitOrder::id, Eq(OrderId{2}))));
}

TEST_F(MatchingEngineTradeAtLastPlacementWithClosingPrice,
       DoesNotCrossOrdersRestingInTheRegularQueue) {
  rest_in_regular_queue(Side::Option::Sell, OrderPrice{ClosingPrice.value()});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);

  placement(order(Side::Option::Buy, OrderId{2}));

  EXPECT_THAT(order_book.sell_page().limit_orders(), SizeIs(1));
  EXPECT_THAT(trade_at_last_orders(Side::Option::Buy), SizeIs(1));
}

struct MatchingEngineTradeAtLastPlacementTimeInForce
    : public MatchingEngineTradeAtLastPlacementWithClosingPrice,
      public WithParamInterface<TimeInForce::Option> {};

TEST_P(MatchingEngineTradeAtLastPlacementTimeInForce,
       RejectsLimitOrderThatIsNotADayOrder) {
  EXPECT_CALL(event_listener,
              on(RejectedWith(trade_at_last::DayOrdersOnlyReject)));

  placement(order_with(TimeInForce{GetParam()}));
}

INSTANTIATE_TEST_SUITE_P(NonDayTimeInForce,
                         MatchingEngineTradeAtLastPlacementTimeInForce,
                         Values(TimeInForce::Option::ImmediateOrCancel,
                                TimeInForce::Option::FillOrKill,
                                TimeInForce::Option::GoodTillCancel,
                                TimeInForce::Option::GoodTillDate));

struct MatchingEngineTradeAtLastPlacementWithoutClosingPrice
    : public MatchingEngineTradeAtLastPlacement {
  TradeAtLastPlacement placement{
      event_listener, order_book, matcher, std::nullopt};
};

TEST_F(MatchingEngineTradeAtLastPlacementWithoutClosingPrice,
       RejectsDayLimitOrder) {
  EXPECT_CALL(event_listener,
              on(RejectedWith(trade_at_last::ClosingPriceUnavailableReject)));

  placement(order(Side::Option::Buy, OrderId{1}));
}

TEST_F(MatchingEngineTradeAtLastPlacementWithoutClosingPrice,
       DoesNotRestDayLimitOrder) {
  placement(order(Side::Option::Buy, OrderId{1}));

  ASSERT_THAT(trade_at_last_orders(Side::Option::Buy), IsEmpty());
}

TEST_F(MatchingEngineTradeAtLastPlacementWithoutClosingPrice,
       ReportsTheTimeInForceViolationOfANonDayOrderFirst) {
  EXPECT_CALL(event_listener,
              on(RejectedWith(trade_at_last::DayOrdersOnlyReject)));

  placement(order_with(TimeInForce::Option::ImmediateOrCancel));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
