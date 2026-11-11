#include <gmock/gmock.h>

#include <optional>
#include <string>

#include "core/domain/market_phase.hpp"
#include "ih/orders/actions/order_actions.hpp"
#include "ih/orders/actions/trade_at_last_actions.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
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

  constexpr static Price ClosingPrice{40};

  static auto context(OrderActionMode mode) -> OrderActionContext {
    return {.mode = mode,
            .market_phase = MarketPhase::open(),
            .closing_price = ClosingPrice};
  }

  auto place_limit(LimitOrder order, OrderActionMode mode) -> void {
    place_limit_order(event_listener,
                      order_book,
                      std::nullopt,
                      std::move(order),
                      context(mode));
  }

  auto place_market(MarketOrder order, OrderActionMode mode) -> void {
    place_market_order(event_listener,
                       order_book,
                       std::nullopt,
                       std::move(order),
                       context(mode));
  }

  auto amend_limit(LimitUpdate update, OrderActionMode mode) -> void {
    amend_limit_order(event_listener,
                      order_book,
                      std::nullopt,
                      std::move(update),
                      context(mode));
  }

  auto amend_market(MarketUpdate update, OrderActionMode mode) -> void {
    amend_market_order(event_listener,
                       order_book,
                       std::nullopt,
                       std::move(update),
                       context(mode));
  }

  auto cancel(Side side, OrderId order_id, OrderActionMode mode) -> void {
    OrderCancel request{protocol::Session{protocol::generator::Session{}},
                        side};
    request.order_id = order_id;
    cancel_order(
        event_listener, order_book, std::nullopt, request, context(mode));
  }

  auto rest_limit(Side side,
                  OrderPrice price,
                  OrderId order_id,
                  OrderQuantity quantity = OrderQuantity{420}) -> void {
    order_book.take_page(side).limit_orders().emplace(
        OrderBuilder{}
            .with_order_id(order_id)
            .with_side(side)
            .with_order_price(price)
            .with_order_quantity(quantity)
            .build_limit_order());
  }

  auto rest_partially_filled_limit(Side side,
                                   OrderPrice price,
                                   OrderId order_id,
                                   ExecutedQuantity executed) -> void {
    auto order = OrderBuilder{}
                     .with_order_id(order_id)
                     .with_side(side)
                     .with_order_price(price)
                     .build_limit_order();
    order.execute(executed, ExecutionPrice{price.value()});
    order_book.take_page(side).limit_orders().emplace(order);
  }

  auto rest_market(Side side, OrderId order_id) -> void {
    order_book.take_page(side).market_orders().emplace(
        market_order(side, order_id));
  }

  auto rest_trade_at_last(Side side, OrderId order_id) -> void {
    order_book.take_page(side).trade_at_last_orders().emplace(
        OrderBuilder{}
            .with_order_id(order_id)
            .with_side(side)
            .with_order_price(OrderPrice{ClosingPrice.value()})
            .build_limit_order());
  }

  static auto limit_order(Side side, OrderPrice price, OrderId order_id)
      -> LimitOrder {
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

  static auto reprice_limit(Side side, OrderPrice price, OrderId order_id)
      -> LimitUpdate {
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

  static auto to_market(Side side, OrderId order_id) -> MarketUpdate {
    OrderAttributes attributes;
    attributes.set_client_order_id(ClientOrderId{"amended"});
    attributes.set_time_in_force(TimeInForce::Option::ImmediateOrCancel);
    MarketUpdate update{
        protocol::Session{protocol::generator::Session{}},
        side,
        MarketOrder::Update{.quantity = OrderQuantity{420},
                            .attributes = std::move(attributes)}};
    update.order_id = order_id;
    return update;
  }

  static auto to_limit(Side side, OrderPrice price, OrderId order_id)
      -> LimitUpdate {
    auto update = reprice_limit(side, price, order_id);
    update.time_in_force_specified = true;
    return update;
  }

  static auto IsMarketOrderReplacement(VenueOrderId venue_order_id) {
    return IsClientNotification(
        VariantWith<protocol::OrderModificationConfirmation>(AllOf(
            Field(&protocol::OrderModificationConfirmation::venue_order_id,
                  Optional(Eq(std::move(venue_order_id)))),
            Field(&protocol::OrderModificationConfirmation::client_order_id,
                  Optional(Eq(ClientOrderId{"amended"}))),
            Field(&protocol::OrderModificationConfirmation::order_type,
                  Optional(Eq(OrderType::Option::Market))))));
  }

  static auto IsExecutionReportFor(VenueOrderId venue_order_id,
                                   OrderStatus status) {
    return IsClientNotification(VariantWith<protocol::ExecutionReport>(
        AllOf(Field(&protocol::ExecutionReport::venue_order_id,
                    Optional(Eq(std::move(venue_order_id)))),
              Field(&protocol::ExecutionReport::order_status,
                    Optional(Eq(status))))));
  }

  static auto resize_limit(Side side, OrderQuantity quantity, OrderId order_id)
      -> LimitUpdate {
    OrderAttributes attributes;
    attributes.set_time_in_force(TimeInForce::Option::Day);
    LimitUpdate update{
        protocol::Session{protocol::generator::Session{}},
        side,
        LimitOrder::Update{.price = OrderPrice{ClosingPrice.value()},
                           .quantity = quantity,
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

TEST_F(MatchingEngineOrderActions, AmendsRestingMarketOrderInAuctionCallMode) {
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

  amend_market(std::move(update), OrderActionMode::AuctionCall);

  auto& market_orders = order_book.buy_page().market_orders();
  ASSERT_THAT(market_orders.size(), Eq(1));
  EXPECT_THAT(market_orders.begin()->total_quantity(), Eq(OrderQuantity{5}));
}

TEST_F(MatchingEngineOrderActions,
       RestsLimitOrderAmendedToMarketWithoutCrossingInAuctionCallMode) {
  rest_limit(Side::Option::Sell, OrderPrice{40}, OrderId{1});
  rest_limit(Side::Option::Buy, OrderPrice{30}, OrderId{2});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);

  amend_market(to_market(Side::Option::Buy, OrderId{2}),
               OrderActionMode::AuctionCall);

  EXPECT_THAT(order_book.buy_page().market_orders(),
              ElementsAre(Property(&MarketOrder::id, Eq(OrderId{2}))));
}

TEST_F(MatchingEngineOrderActions,
       RestsMarketOrderAmendedToLimitWithoutCrossingInAuctionCallMode) {
  rest_limit(Side::Option::Sell, OrderPrice{40}, OrderId{1});
  rest_market(Side::Option::Buy, OrderId{2});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);

  amend_limit(to_limit(Side::Option::Buy, OrderPrice{50}, OrderId{2}),
              OrderActionMode::AuctionCall);

  EXPECT_THAT(
      order_book.buy_page().limit_orders(),
      ElementsAre(AllOf(Property(&LimitOrder::id, Eq(OrderId{2})),
                        Property(&LimitOrder::price, Eq(OrderPrice{50})))));
}

TEST_F(MatchingEngineOrderActions,
       FillsLimitOrderAmendedToMarketAfterReplacingItInRegularMode) {
  rest_limit(Side::Option::Sell, OrderPrice{40}, OrderId{1});
  rest_limit(Side::Option::Buy, OrderPrice{30}, OrderId{2});

  InSequence sequence;
  EXPECT_CALL(event_listener, on(IsMarketOrderReplacement(VenueOrderId{"2"})));
  EXPECT_CALL(
      event_listener,
      on(IsExecutionReportFor(VenueOrderId{"2"}, OrderStatus::Option::Filled)));

  amend_market(to_market(Side::Option::Buy, OrderId{2}),
               OrderActionMode::Regular);
}

TEST_F(MatchingEngineOrderActions,
       CancelsUnfilledPartOfLimitOrderAmendedToMarketInRegularMode) {
  rest_limit(
      Side::Option::Sell, OrderPrice{40}, OrderId{1}, OrderQuantity{100});
  rest_limit(Side::Option::Buy, OrderPrice{30}, OrderId{2});

  InSequence sequence;
  EXPECT_CALL(event_listener, on(IsMarketOrderReplacement(VenueOrderId{"2"})));
  EXPECT_CALL(event_listener,
              on(IsExecutionReportFor(VenueOrderId{"2"},
                                      OrderStatus::Option::PartiallyFilled)));
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(
          VariantWith<protocol::OrderCancellationConfirmation>(Field(
              &protocol::OrderCancellationConfirmation::cancellation_text,
              Optional(Eq(CancellationText{
                  "not enough liquidity to fully fill market order"})))))));

  amend_market(to_market(Side::Option::Buy, OrderId{2}),
               OrderActionMode::Regular);
}

TEST_F(MatchingEngineOrderActions,
       KeepsExecutionsOfPartiallyFilledLimitOrderAmendedToMarketInRegularMode) {
  rest_limit(
      Side::Option::Sell, OrderPrice{40}, OrderId{1}, OrderQuantity{100});
  rest_partially_filled_limit(
      Side::Option::Buy, OrderPrice{30}, OrderId{2}, ExecutedQuantity{100});

  EXPECT_CALL(event_listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  AllOf(Field(&protocol::ExecutionReport::venue_order_id,
                              Optional(Eq(VenueOrderId{"2"}))),
                        Field(&protocol::ExecutionReport::cum_executed_quantity,
                              Optional(Eq(CumExecutedQuantity{200}))),
                        Field(&protocol::ExecutionReport::average_price,
                              Optional(Eq(AveragePrice{35}))))))));

  amend_market(to_market(Side::Option::Buy, OrderId{2}),
               OrderActionMode::Regular);
}

TEST_F(MatchingEngineOrderActions,
       RejectsAmendmentToMarketOfTradeAtLastOrderInTradeAtLastMode) {
  rest_trade_at_last(Side::Option::Buy, OrderId{1});

  EXPECT_CALL(
      event_listener,
      on(IsOrderModificationReject(trade_at_last::LimitOrdersOnlyReject)));

  amend_market(to_market(Side::Option::Buy, OrderId{1}),
               OrderActionMode::TradeAtLast);
}

TEST_F(MatchingEngineOrderActions,
       RestsPlacedLimitOrderInTradeAtLastQueueInTradeAtLastMode) {
  place_limit(
      limit_order(
          Side::Option::Buy, OrderPrice{ClosingPrice.value()}, OrderId{1}),
      OrderActionMode::TradeAtLast);

  EXPECT_THAT(order_book.buy_page().trade_at_last_orders().size(), Eq(1));
  EXPECT_THAT(order_book.buy_page().limit_orders().size(), Eq(0));
}

TEST_F(MatchingEngineOrderActions,
       DoesNotCrossTradeAtLastOrdersWhenPlacingInRegularMode) {
  rest_trade_at_last(Side::Option::Sell, OrderId{1});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);

  place_limit(limit_order(Side::Option::Buy, OrderPrice{50}, OrderId{2}),
              OrderActionMode::Regular);

  EXPECT_THAT(order_book.sell_page().trade_at_last_orders().size(), Eq(1));
  EXPECT_THAT(order_book.buy_page().limit_orders().size(), Eq(1));
}

TEST_F(MatchingEngineOrderActions, RejectsPlacedMarketOrderInTradeAtLastMode) {
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(
          Field(&protocol::OrderPlacementReject::reject_text,
                Optional(Eq(RejectText{
                    std::string{trade_at_last::LimitOrdersOnlyReject}})))))));

  place_market(market_order(Side::Option::Buy, OrderId{1}),
               OrderActionMode::TradeAtLast);
}

TEST_F(MatchingEngineOrderActions,
       AmendsOrderRestingInTradeAtLastQueueInTradeAtLastMode) {
  rest_trade_at_last(Side::Option::Buy, OrderId{1});

  amend_limit(resize_limit(Side::Option::Buy, OrderQuantity{50}, OrderId{1}),
              OrderActionMode::TradeAtLast);

  EXPECT_THAT(order_book.buy_page().trade_at_last_orders(),
              ElementsAre(Property(&LimitOrder::total_quantity,
                                   Eq(OrderQuantity{50}))));
}

TEST_F(MatchingEngineOrderActions,
       CancelsOrderRestingInTradeAtLastQueueInTradeAtLastMode) {
  rest_trade_at_last(Side::Option::Buy, OrderId{1});

  cancel(Side::Option::Buy, OrderId{1}, OrderActionMode::TradeAtLast);

  EXPECT_THAT(order_book.buy_page().trade_at_last_orders(), IsEmpty());
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
