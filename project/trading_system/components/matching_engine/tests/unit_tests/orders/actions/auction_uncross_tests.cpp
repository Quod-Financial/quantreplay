#include <gmock/gmock.h>

#include <cstdint>
#include <optional>
#include <vector>

#include "core/domain/market_phase.hpp"
#include "core/domain/party.hpp"
#include "ih/orders/actions/auction_uncross.hpp"
#include "ih/orders/book/order_book.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

MATCHER_P5(IsExecutionReport,
           price,
           quantity,
           side,
           execution_type,
           order_status,
           "") {
  using namespace ::testing;
  return ExplainMatchResult(
      VariantWith<protocol::ExecutionReport>(AllOf(
          Field(&protocol::ExecutionReport::execution_price, Optional(price)),
          Field(&protocol::ExecutionReport::executed_quantity,
                Optional(quantity)),
          Field(&protocol::ExecutionReport::side, Optional(side)),
          Field(&protocol::ExecutionReport::execution_type,
                Optional(execution_type)),
          Field(&protocol::ExecutionReport::order_status,
                Optional(order_status)))),
      arg,
      result_listener);
}

struct MatchingEngineAuctionUncross : public Test {
  MatchingEngineAuctionUncross() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  static auto auction_phase() -> MarketPhase {
    return MarketPhase{TradingPhase::Option::OpeningAuction,
                       TradingStatus::Option::Halt};
  }

  static auto make_party(PartyId party_id) -> Party {
    return Party{std::move(party_id),
                 PartyIdSource::Option::BIC,
                 PartyRole::Option::ExecutingFirm};
  }

  auto add_buy_limit(OrderPrice price,
                     OrderQuantity quantity,
                     std::vector<Party> parties = {}) -> void {
    order_book.buy_page().limit_orders().emplace(
        make_limit(Side::Option::Buy, price, quantity, std::move(parties)));
  }

  auto add_sell_limit(OrderPrice price,
                      OrderQuantity quantity,
                      std::vector<Party> parties = {}) -> void {
    order_book.sell_page().limit_orders().emplace(
        make_limit(Side::Option::Sell, price, quantity, std::move(parties)));
  }

  auto add_buy_market(OrderQuantity quantity,
                      std::vector<Party> parties = {}) -> void {
    order_book.buy_page().market_orders().emplace(
        make_market(Side::Option::Buy, quantity, std::move(parties)));
  }

  auto add_sell_market(OrderQuantity quantity,
                       std::vector<Party> parties = {}) -> void {
    order_book.sell_page().market_orders().emplace(
        make_market(Side::Option::Sell, quantity, std::move(parties)));
  }

  auto uncross() -> std::optional<AuctionResult> {
    return AuctionUncross{
        event_listener, auction_phase(), std::nullopt}(order_book);
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;

 private:
  auto make_limit(Side side,
                  OrderPrice price,
                  OrderQuantity quantity,
                  std::vector<Party> parties) -> LimitOrder {
    return builder_.with_order_id(OrderId{next_id_++})
        .with_side(side)
        .with_order_price(price)
        .with_order_quantity(quantity)
        .with_order_parties(std::move(parties))
        .build_limit_order();
  }

  auto make_market(Side side,
                   OrderQuantity quantity,
                   std::vector<Party> parties) -> MarketOrder {
    return builder_.with_order_id(OrderId{next_id_++})
        .with_side(side)
        .with_order_quantity(quantity)
        .with_order_parties(std::move(parties))
        .build_market_order();
  }

  OrderBuilder builder_;
  std::uint64_t next_id_{1};
};

TEST_F(MatchingEngineAuctionUncross,
       CrossesCrossingLimitOrdersAtClearingPrice) {
  add_buy_limit(
      OrderPrice{110}, OrderQuantity{100}, {make_party(PartyId{"Buyer"})});
  add_sell_limit(
      OrderPrice{100}, OrderQuantity{100}, {make_party(PartyId{"Seller"})});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(
                  AllOf(Field(&Trade::trade_price, Eq(Price{105})),
                        Field(&Trade::traded_quantity, Eq(Quantity{100})),
                        Field(&Trade::buyer, Eq(BuyerId{"Buyer"})),
                        Field(&Trade::seller, Eq(SellerId{"Seller"})),
                        Field(&Trade::aggressor_side, Eq(std::nullopt)))))))
      .Times(1);

  const auto result = uncross();

  EXPECT_THAT(
      result,
      Optional(AllOf(Field(&AuctionResult::price, Eq(Price{105})),
                     Field(&AuctionResult::quantity, Eq(Quantity{100})))));
  EXPECT_THAT(order_book.buy_page().limit_orders(), IsEmpty());
  EXPECT_THAT(order_book.sell_page().limit_orders(), IsEmpty());
}

TEST_F(MatchingEngineAuctionUncross,
       EmitsExecutionReportForEachCrossedLimitOrder) {
  add_buy_limit(OrderPrice{110}, OrderQuantity{100});
  add_sell_limit(OrderPrice{100}, OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{105},
                                    Quantity{100},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{105},
                                    Quantity{100},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);

  uncross();
}

TEST_F(MatchingEngineAuctionUncross,
       EmitsDepthReductionForEachCrossedLimitOrder) {
  add_buy_limit(OrderPrice{110}, OrderQuantity{100});
  add_sell_limit(OrderPrice{100}, OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(_))))
      .Times(2);

  uncross();
}

TEST_F(MatchingEngineAuctionUncross, LeavesPartiallyFilledLimitOrderResting) {
  add_buy_limit(OrderPrice{110}, OrderQuantity{100});
  add_sell_limit(OrderPrice{100}, OrderQuantity{40});

  const auto result = uncross();

  EXPECT_THAT(result,
              Optional(Field(&AuctionResult::quantity, Eq(Quantity{40}))));
  EXPECT_THAT(order_book.sell_page().limit_orders(), IsEmpty());
  ASSERT_THAT(order_book.buy_page().limit_orders(), SizeIs(1));
  EXPECT_THAT(order_book.buy_page().limit_orders().begin()->leaves_quantity(),
              Eq(LeavesQuantity{60}));
}

TEST_F(MatchingEngineAuctionUncross, CrossesMarketOrdersAgainstEachOther) {
  add_buy_limit(
      OrderPrice{110}, OrderQuantity{100}, {make_party(PartyId{"LimitBuyer"})});
  add_sell_limit(OrderPrice{100},
                 OrderQuantity{100},
                 {make_party(PartyId{"LimitSeller"})});
  add_buy_market(OrderQuantity{30}, {make_party(PartyId{"MarketBuyer"})});
  add_sell_market(OrderQuantity{30}, {make_party(PartyId{"MarketSeller"})});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(
                  AllOf(Field(&Trade::trade_price, Eq(Price{105})),
                        Field(&Trade::traded_quantity, Eq(Quantity{30})),
                        Field(&Trade::buyer, Eq(BuyerId{"MarketBuyer"})),
                        Field(&Trade::seller, Eq(SellerId{"MarketSeller"})))))))
      .Times(1);
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(
                  AllOf(Field(&Trade::trade_price, Eq(Price{105})),
                        Field(&Trade::traded_quantity, Eq(Quantity{100})),
                        Field(&Trade::buyer, Eq(BuyerId{"LimitBuyer"})),
                        Field(&Trade::seller, Eq(SellerId{"LimitSeller"})))))))
      .Times(1);

  uncross();

  EXPECT_THAT(order_book.buy_page().market_orders(), IsEmpty());
  EXPECT_THAT(order_book.sell_page().market_orders(), IsEmpty());
}

TEST_F(MatchingEngineAuctionUncross,
       CrossesLeftoverMarketOrderAgainstOpposingLimit) {
  add_buy_limit(
      OrderPrice{110}, OrderQuantity{100}, {make_party(PartyId{"LimitBuyer"})});
  add_sell_limit(
      OrderPrice{100}, OrderQuantity{100}, {make_party(PartyId{"Seller"})});
  add_buy_market(OrderQuantity{100}, {make_party(PartyId{"MarketBuyer"})});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(
                  AllOf(Field(&Trade::trade_price, Eq(Price{105})),
                        Field(&Trade::traded_quantity, Eq(Quantity{100})),
                        Field(&Trade::buyer, Eq(BuyerId{"MarketBuyer"})),
                        Field(&Trade::seller, Eq(SellerId{"Seller"})))))))
      .Times(1);

  const auto result = uncross();

  EXPECT_THAT(result,
              Optional(Field(&AuctionResult::quantity, Eq(Quantity{100}))));
  EXPECT_THAT(order_book.buy_page().market_orders(), IsEmpty());
  EXPECT_THAT(order_book.sell_page().limit_orders(), IsEmpty());
  ASSERT_THAT(order_book.buy_page().limit_orders(), SizeIs(1));
  EXPECT_THAT(order_book.buy_page().limit_orders().begin()->leaves_quantity(),
              Eq(LeavesQuantity{100}));
}

TEST_F(MatchingEngineAuctionUncross, CancelsLeftoverMarketOrderAfterCrossing) {
  add_buy_limit(OrderPrice{110}, OrderQuantity{100});
  add_sell_limit(
      OrderPrice{100}, OrderQuantity{10}, {make_party(PartyId{"Seller"})});
  add_buy_market(OrderQuantity{100}, {make_party(PartyId{"MarketBuyer"})});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(
                  Field(&Trade::traded_quantity, Eq(Quantity{10}))))))
      .Times(1);
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(_))))
      .Times(1);

  const auto result = uncross();

  EXPECT_THAT(result,
              Optional(Field(&AuctionResult::quantity, Eq(Quantity{10}))));
  EXPECT_THAT(order_book.buy_page().market_orders(), IsEmpty());
}

TEST_F(MatchingEngineAuctionUncross,
       ReportsZeroLeavesQuantityOnLeftoverMarketOrderCancellation) {
  add_buy_limit(OrderPrice{110}, OrderQuantity{100});
  add_sell_limit(OrderPrice{100}, OrderQuantity{10});
  add_buy_market(OrderQuantity{100});

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(
          VariantWith<protocol::OrderCancellationConfirmation>(
              Field(&protocol::OrderCancellationConfirmation::leaving_quantity,
                    Optional(Eq(LeavesQuantity{0})))))))
      .Times(1);

  uncross();
}

TEST_F(MatchingEngineAuctionUncross,
       CancelsAllMarketOrdersWhenNoClearingPrice) {
  add_buy_limit(OrderPrice{100}, OrderQuantity{100});
  add_sell_limit(OrderPrice{101}, OrderQuantity{100});
  add_buy_market(OrderQuantity{50});
  add_sell_market(OrderQuantity{50});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(_))))
      .Times(2);

  const auto result = uncross();

  EXPECT_FALSE(result.has_value());
  EXPECT_THAT(order_book.buy_page().market_orders(), IsEmpty());
  EXPECT_THAT(order_book.sell_page().market_orders(), IsEmpty());
  EXPECT_THAT(order_book.buy_page().limit_orders(), SizeIs(1));
  EXPECT_THAT(order_book.sell_page().limit_orders(), SizeIs(1));
}

TEST_F(MatchingEngineAuctionUncross, StampsTradeWithConfiguredAuctionPhase) {
  add_buy_limit(OrderPrice{110}, OrderQuantity{100});
  add_sell_limit(OrderPrice{100}, OrderQuantity{100});

  const MarketPhase closing_auction{TradingPhase::Option::ClosingAuction,
                                    TradingStatus::Option::Halt};

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(
                  Field(&Trade::market_phase, Eq(closing_auction))))))
      .Times(1);

  AuctionUncross{event_listener, closing_auction, std::nullopt}(order_book);
}

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
