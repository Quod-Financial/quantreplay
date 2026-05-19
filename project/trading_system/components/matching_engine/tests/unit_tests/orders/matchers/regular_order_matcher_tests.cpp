#include <gmock/gmock.h>

#include "ih/orders/book/order_book.hpp"
#include "ih/orders/matchers/regular_order_matcher.hpp"
#include "mocks/event_listener_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
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

MATCHER_P5(IsTradeNotification,
           buyer,
           seller,
           trade_price,
           traded_quantity,
           aggressor_side,
           "") {
  using namespace ::testing;

  return ExplainMatchResult(
      VariantWith<Trade>(AllOf(Field(&Trade::buyer, buyer),
                               Field(&Trade::seller, seller),
                               Field(&Trade::trade_price, trade_price),
                               Field(&Trade::traded_quantity, traded_quantity),
                               Field(&Trade::aggressor_side, aggressor_side))),
      arg,
      result_listener);
}

MATCHER_P4(
    IsOrderCancellationConfirmation, price, quantity, side, order_status, "") {
  using namespace ::testing;

  return ExplainMatchResult(
      VariantWith<protocol::OrderCancellationConfirmation>(AllOf(
          Field(&protocol::OrderCancellationConfirmation::order_price,
                Optional(price)),
          Field(&protocol::OrderCancellationConfirmation::leaving_quantity,
                Optional(quantity)),
          Field(&protocol::OrderCancellationConfirmation::side, Optional(side)),
          Field(&protocol::OrderCancellationConfirmation::order_status,
                Optional(order_status)))),
      arg,
      result_listener);
}

struct BuyLimitOrderMatching : public Test {
  auto make_aggressor(OrderId order_id,
                      Price price,
                      Quantity quantity,
                      std::vector<Party> parties = {}) -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_order_price(static_cast<OrderPrice>(price))
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_order_parties(std::move(parties))
        .with_side(Side::Option::Buy)
        .with_time_in_force(TimeInForce::Option::Day)
        .build_limit_order();
  }

  auto make_ioc_aggressor(OrderId order_id,
                          Price price,
                          Quantity quantity,
                          std::vector<Party> parties = {}) -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_order_price(static_cast<OrderPrice>(price))
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_order_parties(std::move(parties))
        .with_side(Side::Option::Buy)
        .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
        .build_limit_order();
  }

  auto make_ioc_aggressor(OrderId order_id,
                          Price price,
                          Quantity quantity,
                          ClientOrderId client_order_id) -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_order_price(static_cast<OrderPrice>(price))
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_side(Side::Option::Buy)
        .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
        .with_client_order_id(std::move(client_order_id))
        .build_limit_order();
  }

  auto add_resting_order(OrderId order_id,
                         Price price,
                         Quantity quantity,
                         std::vector<Party> parties = {}) -> void {
    resting_orders().emplace(
        OrderBuilder{}
            .with_order_id(order_id)
            .with_order_price(static_cast<OrderPrice>(price))
            .with_order_quantity(static_cast<OrderQuantity>(quantity))
            .with_order_parties(std::move(parties))
            .with_side(Side::Option::Sell)
            .build_limit_order());
  }

  auto make_party(PartyId party_id) -> Party {
    return Party{std::move(party_id),
                 PartyIdSource::Option::BIC,
                 PartyRole::Option::ExecutingFirm};
  }

  auto resting_orders() -> LimitOrdersContainer& {
    return book_.sell_page().limit_orders();
  }

 private:
  OrderBook book_;

 public:
  NiceMock<EventListenerMock> listener;
  RegularOrderMatcher matcher{listener, book_, std::nullopt};
};

TEST_F(BuyLimitOrderMatching, DoesNotDetectFacingOrdersInEmptyPage) {
  ASSERT_THAT(resting_orders(), IsEmpty());

  LimitOrder order = make_aggressor(OrderId{1}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(BuyLimitOrderMatching, DoesNotDetectFacingOrderWhenAllHaveWorsePrice) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});
  add_resting_order(OrderId{2}, Price{102}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(BuyLimitOrderMatching, DetectsFacingOrders) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});
  add_resting_order(OrderId{2}, Price{102}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsTrue());
}

TEST_F(BuyLimitOrderMatching, DetectsTakerCanNotBeFullyTraded) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{99});
  add_resting_order(OrderId{2}, Price{101}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.can_fully_trade(order), IsFalse());
}

TEST_F(BuyLimitOrderMatching, DetectsTakerCanBeFullyTraded) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{55});
  add_resting_order(OrderId{2}, Price{100}, Quantity{55});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.can_fully_trade(order), IsTrue());
}

TEST_F(BuyLimitOrderMatching, SkipsMatchingIfSellSideIsEmpty) {
  ASSERT_THAT(resting_orders(), IsEmpty());

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(_)).Times(0);
  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, SkipsMatchingIfSellSideHasWorsePrices) {
  add_resting_order(OrderId{1}, Price{99.0001}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{99.0000}, Quantity{100});

  EXPECT_CALL(listener, on(_)).Times(0);
  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, ReducesTakerQuantityOnMatch) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{70});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_EQ(order.leaves_quantity(), Quantity{30});
}

TEST_F(BuyLimitOrderMatching, RemovesFilledRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{2}, Price{100}, Quantity{50});
  ASSERT_THAT(resting_orders(), SizeIs(2));

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(resting_orders(), IsEmpty());
}

TEST_F(BuyLimitOrderMatching, DoesNotRemoveNonFilledRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{51});
  ASSERT_THAT(resting_orders(), SizeIs(1));

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{50});
  matcher.match(order);

  ASSERT_THAT(resting_orders(), SizeIs(1));
  ASSERT_EQ(resting_orders().begin()->leaves_quantity(), LeavesQuantity{1});
}

TEST_F(BuyLimitOrderMatching,
       EmitsExecutionReportsWithExecutionIdForBothOrdersOnMatch) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
          Field(&protocol::ExecutionReport::execution_id, Ne(std::nullopt))))))
      .Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, EmitsExecutionReportsWithPartiesOnMatch) {
  Party maker_party(PartyId{"Maker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  add_resting_order(OrderId{1}, Price{99}, Quantity{100}, {maker_party});

  Party taker_party(PartyId{"Taker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  LimitOrder order =
      make_aggressor(OrderId{2}, Price{100}, Quantity{100}, {taker_party});

  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(taker_party,
                                    Party{maker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(maker_party,
                                    Party{taker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, EmitsOrderReducedOfMakerOnMatch) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{99}, Quantity{0}, OrderId{1}, Side::Option::Sell}))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, EmitsTradeOnMatch) {
  add_resting_order(
      OrderId{1}, Price{99}, Quantity{100}, {make_party(PartyId{"Maker"})});

  LimitOrder order = make_aggressor(
      OrderId{2}, Price{100}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(_))))
      .Times(1);

  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker"},
                                                     Price{99},
                                                     Quantity{100},
                                                     Side::Option::Buy))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, MatchesAggressorAtSamePrice) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, MatchesAggressorWithBetterPrice) {
  add_resting_order(OrderId{1}, Price{99.9999}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{99.9999},
                                    Quantity{100},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{99.9999},
                                    Quantity{100},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, MatchesByRestingOrderPrice) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);
  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(Field(
          &protocol::ExecutionReport::execution_price, Optional(Price{99}))))))
      .Times(2);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, SetsAggressorAveragePriceFromRestingOrderPrice) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{99.0})));
}

TEST_F(BuyLimitOrderMatching, SetsRestingOrderAveragePriceToItsOwnPrice) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{50});
  matcher.match(order);

  ASSERT_THAT(resting_orders(),
              ElementsAre(Property(&LimitOrder::average_price,
                                   Optional(Eq(AveragePrice{99.0})))));
}

TEST_F(BuyLimitOrderMatching, SetsAveragePriceOnIocAggressor) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{50});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});
  matcher.match(order);

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{99.0})));
}

TEST_F(BuyLimitOrderMatching, SetsAveragePriceOnIocRestingOrder) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});
  matcher.match(order);

  ASSERT_THAT(resting_orders(),
              ElementsAre(Property(&LimitOrder::average_price,
                                   Optional(Eq(AveragePrice{99.0})))));
}

TEST_F(BuyLimitOrderMatching, PopulatesAveragePriceInExecutionReports) {
  add_resting_order(OrderId{1}, Price{98}, Quantity{100});
  add_resting_order(OrderId{2}, Price{99}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{200});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(4);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{98.0}))))))
      .Times(2);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{99.0}))))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{98.5}))))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, MatchesMinimalLeavesQuantity) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::executed_quantity,
                        Optional(Quantity{50}))))))
      .Times(2);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, MatchesAggressorWithMultipleRestingOrders) {
  add_resting_order(
      OrderId{1}, Price{98}, Quantity{40}, {make_party(PartyId{"Maker1"})});
  add_resting_order(
      OrderId{2}, Price{99}, Quantity{75}, {make_party(PartyId{"Maker2"})});

  LimitOrder order = make_aggressor(
      OrderId{3}, Price{100}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{98},
                                    Quantity{40},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{98},
                                    Quantity{40},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{98}, Quantity{0}, OrderId{1}, Side::Option::Sell}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker1"},
                                                     Price{98},
                                                     Quantity{40},
                                                     Side::Option::Buy))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{99},
                                    Quantity{60},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{99},
                                    Quantity{60},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{99}, Quantity{15}, OrderId{2}, Side::Option::Sell}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker2"},
                                                     Price{99},
                                                     Quantity{60},
                                                     Side::Option::Buy))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, ThrowsExceptionWhenIocOrderHasNoFacingOrders) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{100});

  ASSERT_THROW(matcher.match(order), std::logic_error);
}

TEST_F(BuyLimitOrderMatching, FullyMatchesIocOrderWithRestingOrder) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});
  matcher.match(order);

  EXPECT_THAT(order.executed(), IsTrue());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(BuyLimitOrderMatching,
       EmitsExecutionReportsWithExecutionIdForBothOrdersOnMatchIoc) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});

  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
          Field(&protocol::ExecutionReport::execution_id, Ne(std::nullopt))))))
      .Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, EmitsExecutionReportsWithPartiesOnMatchIoc) {
  Party maker_party(PartyId{"Maker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  add_resting_order(OrderId{1}, Price{100}, Quantity{50}, {maker_party});

  Party taker_party(PartyId{"Taker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  LimitOrder order =
      make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50}, {taker_party});

  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(taker_party,
                                    Party{maker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(maker_party,
                                    Party{taker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, EmitsOrderReducedOfMakerOnMatchIoc) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{50});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{99}, Quantity{0}, OrderId{1}, Side::Option::Sell}))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, EmitsTradeOnMatchIoc) {
  add_resting_order(
      OrderId{1}, Price{99}, Quantity{50}, {make_party(PartyId{"Maker"})});

  LimitOrder order = make_ioc_aggressor(
      OrderId{2}, Price{100}, Quantity{50}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(_))))
      .Times(1);

  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker"},
                                                     Price{99},
                                                     Quantity{50},
                                                     Side::Option::Buy))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching,
       EmitsOrderCancellationConfirmationWithExecutionIdOnPartialMatchIoc) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{2}, Price{101}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(OrderId{3}, Price{100}, Quantity{75});

  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(_))))
      .Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(_))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::execution_id,
                      Ne(std::nullopt))))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching,
       EmitsOrderCancellationConfirmationWithClientOrderIdOnPartialMatchIoc) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{2}, Price{101}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(
      OrderId{3}, Price{100}, Quantity{75}, ClientOrderId{"ClientOrderId"});

  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(_))))
      .Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(_))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::client_order_id,
                      ClientOrderId{"ClientOrderId"})))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyLimitOrderMatching, FullyMatchesIocOrderWithMultipleRestingOrders) {
  add_resting_order(
      OrderId{1}, Price{98}, Quantity{50}, {make_party(PartyId{"Maker1"})});
  add_resting_order(
      OrderId{2}, Price{99}, Quantity{70}, {make_party(PartyId{"Maker2"})});

  LimitOrder order = make_ioc_aggressor(
      OrderId{4}, Price{100}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{98},
                                    Quantity{50},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{98},
                                    Quantity{50},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{98}, Quantity{0}, OrderId{1}, Side::Option::Sell}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker1"},
                                                     Price{98},
                                                     Quantity{50},
                                                     Side::Option::Buy))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{99},
                                    Quantity{50},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{99},
                                    Quantity{50},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{99}, Quantity{20}, OrderId{2}, Side::Option::Sell}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker2"},
                                                     Price{99},
                                                     Quantity{50},
                                                     Side::Option::Buy))))
      .Times(1);

  matcher.match(order);

  EXPECT_THAT(order.executed(), IsTrue());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(BuyLimitOrderMatching, PartiallyMatchesIocOrderWithRestingOrder) {
  add_resting_order(
      OrderId{1}, Price{100}, Quantity{50}, {make_party(PartyId{"Maker1"})});
  add_resting_order(
      OrderId{2}, Price{101}, Quantity{1000}, {make_party(PartyId{"Maker2"})});

  LimitOrder order = make_ioc_aggressor(
      OrderId{3}, Price{100}, Quantity{75}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{50},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{50},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{0}, OrderId{1}, Side::Option::Sell}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker1"},
                                                     Price{100},
                                                     Quantity{50},
                                                     Side::Option::Buy))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  Price{100},
                  Quantity{25},
                  Side::Option::Buy,
                  OrderStatus::Option::Cancelled))))
      .Times(1);

  matcher.match(order);

  EXPECT_THAT(order.executed(), IsFalse());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Cancelled));
  EXPECT_THAT(order.leaves_quantity(), Eq(Quantity{25}));
}

TEST_F(BuyLimitOrderMatching,
       PartiallyMatchesIocOrderWithMultipleRestingOrders) {
  add_resting_order(
      OrderId{1}, Price{100}, Quantity{25}, {make_party(PartyId{"Maker1"})});
  add_resting_order(
      OrderId{2}, Price{100}, Quantity{30}, {make_party(PartyId{"Maker2"})});
  add_resting_order(OrderId{3}, Price{101}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(
      OrderId{4}, Price{100}, Quantity{75}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{25},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{25},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{0}, OrderId{1}, Side::Option::Sell}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker1"},
                                                     Price{100},
                                                     Quantity{25},
                                                     Side::Option::Buy))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{30},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{30},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{0}, OrderId{2}, Side::Option::Sell}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker2"},
                                                     Price{100},
                                                     Quantity{30},
                                                     Side::Option::Buy))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  Price{100},
                  Quantity{20},
                  Side::Option::Buy,
                  OrderStatus::Option::Cancelled))))
      .Times(1);

  matcher.match(order);

  EXPECT_THAT(order.executed(), IsFalse());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Cancelled));
  EXPECT_THAT(order.leaves_quantity(), Eq(Quantity{20}));
}

struct SellLimitOrderMatching : public Test {
  auto make_aggressor(OrderId order_id,
                      Price price,
                      Quantity quantity,
                      std::vector<Party> parties = {}) -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_order_price(static_cast<OrderPrice>(price))
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_order_parties(std::move(parties))
        .with_side(Side::Option::Sell)
        .with_time_in_force(TimeInForce::Option::Day)
        .build_limit_order();
  }

  auto make_ioc_aggressor(OrderId order_id,
                          Price price,
                          Quantity quantity,
                          std::vector<Party> parties = {}) -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_order_price(static_cast<OrderPrice>(price))
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_order_parties(std::move(parties))
        .with_side(Side::Option::Sell)
        .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
        .build_limit_order();
  }

  auto make_ioc_aggressor(OrderId order_id,
                          Price price,
                          Quantity quantity,
                          ClientOrderId client_order_id) -> LimitOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_order_price(static_cast<OrderPrice>(price))
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_side(Side::Option::Sell)
        .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
        .with_client_order_id(std::move(client_order_id))
        .build_limit_order();
  }

  auto add_resting_order(OrderId order_id,
                         Price price,
                         Quantity quantity,
                         std::vector<Party> parties = {}) -> void {
    resting_orders().emplace(
        OrderBuilder{}
            .with_order_id(order_id)
            .with_order_price(static_cast<OrderPrice>(price))
            .with_order_quantity(static_cast<OrderQuantity>(quantity))
            .with_order_parties(std::move(parties))
            .with_side(Side::Option::Buy)
            .build_limit_order());
  }

  auto make_party(PartyId party_id) -> Party {
    return Party{std::move(party_id),
                 PartyIdSource::Option::BIC,
                 PartyRole::Option::ExecutingFirm};
  }

  auto resting_orders() -> LimitOrdersContainer& {
    return book_.buy_page().limit_orders();
  }

 private:
  OrderBook book_;

 public:
  NiceMock<EventListenerMock> listener;
  RegularOrderMatcher matcher{listener, book_, std::nullopt};
};

TEST_F(SellLimitOrderMatching, DoesNotDetectFacingOrdersInEmptyPage) {
  ASSERT_THAT(resting_orders(), IsEmpty());

  LimitOrder order = make_aggressor(OrderId{1}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(SellLimitOrderMatching, DoesNotDetectFacingOrderWhenAllHaveWorsePrice) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});
  add_resting_order(OrderId{2}, Price{98}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(SellLimitOrderMatching, DetectsFacingOrders) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});
  add_resting_order(OrderId{2}, Price{99}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsTrue());
}

TEST_F(SellLimitOrderMatching, DetectsTakerCanNotBeFullyTraded) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{99});
  add_resting_order(OrderId{2}, Price{99}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.can_fully_trade(order), IsFalse());
}

TEST_F(SellLimitOrderMatching, DetectsTakerCanBeFullyTraded) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{55});
  add_resting_order(OrderId{2}, Price{100}, Quantity{55});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});

  ASSERT_THAT(matcher.can_fully_trade(order), IsTrue());
}

TEST_F(SellLimitOrderMatching, SkipsMatchingIfBuySideIsEmpty) {
  ASSERT_THAT(resting_orders(), IsEmpty());

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(_)).Times(0);
  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, SkipsMatchingIfBuySideHasWorsePrices) {
  add_resting_order(OrderId{1}, Price{99.9999}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100.0000}, Quantity{100});

  EXPECT_CALL(listener, on(_)).Times(0);
  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, ReducesTakerQuantityOnMatch) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{70});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_EQ(order.leaves_quantity(), Quantity{30});
}

TEST_F(SellLimitOrderMatching, RemovesFilledRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{2}, Price{100}, Quantity{50});
  ASSERT_THAT(resting_orders(), SizeIs(2));

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(resting_orders(), IsEmpty());
}

TEST_F(SellLimitOrderMatching, DoesNotRemoveNonFilledRestingOrders) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{51});
  ASSERT_THAT(resting_orders(), SizeIs(1));

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{50});
  matcher.match(order);

  ASSERT_THAT(resting_orders(), SizeIs(1));
  ASSERT_EQ(resting_orders().begin()->leaves_quantity(), LeavesQuantity{1});
}

TEST_F(SellLimitOrderMatching,
       EmitsExecutionReportsWithExecutionIdForBothOrdersOnMatch) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
          Field(&protocol::ExecutionReport::execution_id, Ne(std::nullopt))))))
      .Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, EmitsExecutionReportsWithPartiesOnMatch) {
  Party maker_party(PartyId{"Maker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  add_resting_order(OrderId{1}, Price{101}, Quantity{100}, {maker_party});

  Party taker_party(PartyId{"Taker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  LimitOrder order =
      make_aggressor(OrderId{2}, Price{100}, Quantity{100}, {taker_party});

  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(taker_party,
                                    Party{maker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(maker_party,
                                    Party{taker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, EmitsOrderReducedOfMakerOnMatch) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{101}, Quantity{0}, OrderId{1}, Side::Option::Buy}))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, EmitsTradeOnMatch) {
  add_resting_order(
      OrderId{1}, Price{101}, Quantity{100}, {make_party(PartyId{"Maker"})});

  LimitOrder order = make_aggressor(
      OrderId{2}, Price{100}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(_))))
      .Times(1);

  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker"},
                                                     SellerId{"Taker"},
                                                     Price{101},
                                                     Quantity{100},
                                                     Side::Option::Sell))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, MatchesAggressorAtSamePrice) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, MatchesAggressorWithBetterPrice) {
  add_resting_order(OrderId{1}, Price{100.0001}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100.0001},
                                    Quantity{100},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100.0001},
                                    Quantity{100},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, MatchesByRestingOrderPrice) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);
  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(Field(
          &protocol::ExecutionReport::execution_price, Optional(Price{101}))))))
      .Times(2);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, SetsAggressorAveragePriceFromRestingOrderPrice) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{101.0})));
}

TEST_F(SellLimitOrderMatching, SetsRestingOrderAveragePriceToItsOwnPrice) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{50});
  matcher.match(order);

  ASSERT_THAT(resting_orders(),
              ElementsAre(Property(&LimitOrder::average_price,
                                   Optional(Eq(AveragePrice{101.0})))));
}

TEST_F(SellLimitOrderMatching, SetsAveragePriceOnIocAggressor) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{50});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});
  matcher.match(order);

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{101.0})));
}

TEST_F(SellLimitOrderMatching, SetsAveragePriceOnIocRestingOrder) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});
  matcher.match(order);

  ASSERT_THAT(resting_orders(),
              ElementsAre(Property(&LimitOrder::average_price,
                                   Optional(Eq(AveragePrice{101.0})))));
}

TEST_F(SellLimitOrderMatching, PopulatesAveragePriceInExecutionReports) {
  add_resting_order(OrderId{1}, Price{102}, Quantity{100});
  add_resting_order(OrderId{2}, Price{101}, Quantity{100});

  LimitOrder order = make_aggressor(OrderId{3}, Price{100}, Quantity{200});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(4);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{102.0}))))))
      .Times(2);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{101.0}))))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{101.5}))))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, MatchesMinimalLeavesQuantity) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});

  LimitOrder order = make_aggressor(OrderId{2}, Price{100}, Quantity{100});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::executed_quantity,
                        Optional(Quantity{50}))))))
      .Times(2);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, MatchesAggressorWithMultipleRestingOrders) {
  add_resting_order(
      OrderId{1}, Price{102}, Quantity{40}, {make_party(PartyId{"Maker1"})});
  add_resting_order(
      OrderId{2}, Price{101}, Quantity{75}, {make_party(PartyId{"Maker2"})});

  LimitOrder order = make_aggressor(
      OrderId{3}, Price{100}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{102},
                                    Quantity{40},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{102},
                                    Quantity{40},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{102}, Quantity{0}, OrderId{1}, Side::Option::Buy}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker1"},
                                                     SellerId{"Taker"},
                                                     Price{102},
                                                     Quantity{40},
                                                     Side::Option::Sell))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{101},
                                    Quantity{60},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{101},
                                    Quantity{60},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{101}, Quantity{15}, OrderId{2}, Side::Option::Buy}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker2"},
                                                     SellerId{"Taker"},
                                                     Price{101},
                                                     Quantity{60},
                                                     Side::Option::Sell))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, ThrowsExceptionWhenIocOrderHasNoFacingOrders) {
  add_resting_order(OrderId{1}, Price{99}, Quantity{100});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{100});

  ASSERT_THROW(matcher.match(order), std::logic_error);
}

TEST_F(SellLimitOrderMatching, FullyMatchesIocOrderWithRestingOrder) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});
  matcher.match(order);

  EXPECT_THAT(order.executed(), IsTrue());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(SellLimitOrderMatching,
       EmitsExecutionReportsWithExecutionIdForBothOrdersOnMatchIoc) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});

  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
          Field(&protocol::ExecutionReport::execution_id, Ne(std::nullopt))))))
      .Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, EmitsExecutionReportsWithPartiesOnMatchIoc) {
  Party maker_party(PartyId{"Maker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  add_resting_order(OrderId{1}, Price{100}, Quantity{50}, {maker_party});

  Party taker_party(PartyId{"Taker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  LimitOrder order =
      make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50}, {taker_party});

  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(taker_party,
                                    Party{maker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(maker_party,
                                    Party{taker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, EmitsOrderReducedOfMakerOnMatchIoc) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{50});

  LimitOrder order = make_ioc_aggressor(OrderId{2}, Price{100}, Quantity{50});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{101}, Quantity{0}, OrderId{1}, Side::Option::Buy}))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, EmitsTradeOnMatchIoc) {
  add_resting_order(
      OrderId{1}, Price{101}, Quantity{50}, {make_party(PartyId{"Maker"})});

  LimitOrder order = make_ioc_aggressor(
      OrderId{2}, Price{100}, Quantity{50}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(_))))
      .Times(1);

  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker"},
                                                     SellerId{"Taker"},
                                                     Price{101},
                                                     Quantity{50},
                                                     Side::Option::Sell))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching,
       EmitsOrderCancellationConfirmationWithExecutionIdOnPartialMatchIoc) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{2}, Price{99}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(OrderId{3}, Price{100}, Quantity{75});

  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(_))))
      .Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(_))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::execution_id,
                      Ne(std::nullopt))))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching,
       EmitsOrderCancellationConfirmationWithClientOrderIdOnPartialMatchIoc) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{50});
  add_resting_order(OrderId{2}, Price{99}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(
      OrderId{3}, Price{100}, Quantity{75}, ClientOrderId{"ClientOrderId"});

  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(_))))
      .Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(_))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::client_order_id,
                      ClientOrderId{"ClientOrderId"})))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellLimitOrderMatching, FullyMatchesIocOrderWithMultipleRestingOrders) {
  add_resting_order(
      OrderId{1}, Price{102}, Quantity{50}, {make_party(PartyId{"Maker1"})});
  add_resting_order(
      OrderId{2}, Price{101}, Quantity{70}, {make_party(PartyId{"Maker2"})});

  LimitOrder order = make_ioc_aggressor(
      OrderId{4}, Price{100}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{102},
                                    Quantity{50},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{102},
                                    Quantity{50},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{102}, Quantity{0}, OrderId{1}, Side::Option::Buy}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker1"},
                                                     SellerId{"Taker"},
                                                     Price{102},
                                                     Quantity{50},
                                                     Side::Option::Sell))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{101},
                                    Quantity{50},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{101},
                                    Quantity{50},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{101}, Quantity{20}, OrderId{2}, Side::Option::Buy}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker2"},
                                                     SellerId{"Taker"},
                                                     Price{101},
                                                     Quantity{50},
                                                     Side::Option::Sell))))
      .Times(1);

  matcher.match(order);

  EXPECT_THAT(order.executed(), IsTrue());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(SellLimitOrderMatching, PartiallyMatchesIocOrderWithRestingOrder) {
  add_resting_order(
      OrderId{1}, Price{100}, Quantity{50}, {make_party(PartyId{"Maker1"})});
  add_resting_order(
      OrderId{2}, Price{99}, Quantity{1000}, {make_party(PartyId{"Maker2"})});

  LimitOrder order = make_ioc_aggressor(
      OrderId{3}, Price{100}, Quantity{75}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{50},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{50},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{0}, OrderId{1}, Side::Option::Buy}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker1"},
                                                     SellerId{"Taker"},
                                                     Price{100},
                                                     Quantity{50},
                                                     Side::Option::Sell))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  Price{100},
                  Quantity{25},
                  Side::Option::Sell,
                  OrderStatus::Option::Cancelled))))
      .Times(1);

  matcher.match(order);

  EXPECT_THAT(order.executed(), IsFalse());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Cancelled));
  EXPECT_THAT(order.leaves_quantity(), Eq(Quantity{25}));
}

TEST_F(SellLimitOrderMatching,
       PartiallyMatchesIocOrderWithMultipleRestingOrders) {
  add_resting_order(
      OrderId{1}, Price{100}, Quantity{25}, {make_party(PartyId{"Maker1"})});
  add_resting_order(
      OrderId{2}, Price{100}, Quantity{30}, {make_party(PartyId{"Maker2"})});
  add_resting_order(OrderId{3}, Price{99}, Quantity{1000});

  LimitOrder order = make_ioc_aggressor(
      OrderId{4}, Price{100}, Quantity{75}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{25},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{25},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{0}, OrderId{1}, Side::Option::Buy}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker1"},
                                                     SellerId{"Taker"},
                                                     Price{100},
                                                     Quantity{25},
                                                     Side::Option::Sell))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{30},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{30},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{0}, OrderId{2}, Side::Option::Buy}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker2"},
                                                     SellerId{"Taker"},
                                                     Price{100},
                                                     Quantity{30},
                                                     Side::Option::Sell))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  Price{100},
                  Quantity{20},
                  Side::Option::Sell,
                  OrderStatus::Option::Cancelled))))
      .Times(1);

  matcher.match(order);

  EXPECT_THAT(order.executed(), IsFalse());
  EXPECT_THAT(order.status(), Eq(OrderStatus::Option::Cancelled));
  EXPECT_THAT(order.leaves_quantity(), Eq(Quantity{20}));
}

struct BuyMarketOrderMatching : public Test {
  auto make_aggressor(OrderId order_id,
                      Quantity quantity,
                      std::vector<Party> parties = {}) -> MarketOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_order_parties(std::move(parties))
        .with_side(Side::Option::Buy)
        .build_market_order();
  }

  auto add_resting_order(OrderId order_id,
                         Price price,
                         Quantity quantity,
                         std::vector<Party> parties = {}) -> void {
    resting_orders().emplace(
        OrderBuilder{}
            .with_order_id(order_id)
            .with_order_price(static_cast<OrderPrice>(price))
            .with_order_quantity(static_cast<OrderQuantity>(quantity))
            .with_order_parties(std::move(parties))
            .with_side(Side::Option::Sell)
            .build_limit_order());
  }

  auto make_party(PartyId party_id) -> Party {
    return Party{std::move(party_id),
                 PartyIdSource::Option::BIC,
                 PartyRole::Option::ExecutingFirm};
  }

  auto resting_orders() -> LimitOrdersContainer& {
    return book_.sell_page().limit_orders();
  }

 private:
  OrderBook book_;

 public:
  NiceMock<EventListenerMock> listener;
  RegularOrderMatcher matcher{listener, book_, std::nullopt};
};

TEST_F(BuyMarketOrderMatching, DoesNotDetectFacingOrdersInEmptyPage) {
  const MarketOrder order = make_aggressor(OrderId{1}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(BuyMarketOrderMatching, DetectsOppositeFacingOrders) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  const MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsTrue());
}

TEST_F(BuyMarketOrderMatching, ThrowsExceptionWhenMatchedWithoutFacingOrders) {
  MarketOrder order = make_aggressor(OrderId{1}, Quantity{100});

  ASSERT_THROW(matcher.match(order), std::logic_error);
}

TEST_F(BuyMarketOrderMatching,
       EmitsExecutionReportsWithExecutionIdForBothOrdersOnMatch) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});

  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});

  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
          Field(&protocol::ExecutionReport::execution_id, Ne(std::nullopt))))))
      .Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(BuyMarketOrderMatching, EmitsExecutionReportsWithPartiesOnMatch) {
  Party maker_party(PartyId{"Maker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  add_resting_order(OrderId{1}, Price{100}, Quantity{100}, {maker_party});

  Party taker_party(PartyId{"Taker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100}, {taker_party});

  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(taker_party,
                                    Party{maker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(maker_party,
                                    Party{taker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(BuyMarketOrderMatching, EmitsOrderReducedOfMakerOnMatch) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});

  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{0}, OrderId{1}, Side::Option::Sell}))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyMarketOrderMatching, EmitsTradeOnMatch) {
  add_resting_order(
      OrderId{1}, Price{100}, Quantity{100}, {make_party(PartyId{"Maker"})});

  MarketOrder order =
      make_aggressor(OrderId{2}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(_))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker"},
                                                     Price{100},
                                                     Quantity{100},
                                                     Side::Option::Buy))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyMarketOrderMatching, PopulatesAveragePriceInExecutionReports) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});
  add_resting_order(OrderId{2}, Price{101}, Quantity{100});

  MarketOrder order = make_aggressor(OrderId{3}, Quantity{200});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(4);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{100.0}))))))
      .Times(2);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{101.0}))))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{100.5}))))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyMarketOrderMatching, FullyMatchesWithFacingOrder) {
  add_resting_order(
      OrderId{1}, Price{100}, Quantity{101}, {make_party(PartyId{"Maker"})});

  MarketOrder order =
      make_aggressor(OrderId{2}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{1}, OrderId{1}, Side::Option::Sell}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker"},
                                                     Price{100},
                                                     Quantity{100},
                                                     Side::Option::Buy))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyMarketOrderMatching, PartiallyMatchesWithFacingOrder) {
  add_resting_order(
      OrderId{1}, Price{100}, Quantity{99}, {make_party(PartyId{"Maker"})});

  MarketOrder order =
      make_aggressor(OrderId{2}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{99},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Cancelled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{99},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{0}, OrderId{1}, Side::Option::Sell}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker"},
                                                     Price{100},
                                                     Quantity{99},
                                                     Side::Option::Buy))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyMarketOrderMatching, MatchesWithSeveralFacingOrders) {
  add_resting_order(
      OrderId{1}, Price{100}, Quantity{100}, {make_party(PartyId{"Maker1"})});
  add_resting_order(
      OrderId{2}, Price{101}, Quantity{150}, {make_party(PartyId{"Maker2"})});

  MarketOrder order =
      make_aggressor(OrderId{3}, Quantity{200}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{0}, OrderId{1}, Side::Option::Sell}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker1"},
                                                     Price{100},
                                                     Quantity{100},
                                                     Side::Option::Buy))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{101},
                                    Quantity{100},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{101},
                                    Quantity{100},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{101}, Quantity{50}, OrderId{2}, Side::Option::Sell}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Taker"},
                                                     SellerId{"Maker2"},
                                                     Price{101},
                                                     Quantity{100},
                                                     Side::Option::Buy))))
      .Times(1);

  matcher.match(order);
}

TEST_F(BuyMarketOrderMatching, SetsAggressorAveragePriceFromRestingOrderPrice) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});

  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{100.0})));
}

TEST_F(BuyMarketOrderMatching, SetsWeightedAveragePriceAcrossMultipleFills) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});
  add_resting_order(OrderId{2}, Price{101}, Quantity{100});

  MarketOrder order = make_aggressor(OrderId{3}, Quantity{200});
  matcher.match(order);

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{100.5})));
}

struct SellMarketOrderMatching : public Test {
  auto make_aggressor(OrderId order_id,
                      Quantity quantity,
                      std::vector<Party> parties = {}) -> MarketOrder {
    return OrderBuilder{}
        .with_order_id(order_id)
        .with_order_quantity(static_cast<OrderQuantity>(quantity))
        .with_order_parties(std::move(parties))
        .with_side(Side::Option::Sell)
        .build_market_order();
  }

  auto add_resting_order(OrderId order_id,
                         Price price,
                         Quantity quantity,
                         std::vector<Party> parties = {}) -> void {
    resting_orders().emplace(
        OrderBuilder{}
            .with_order_id(order_id)
            .with_order_price(static_cast<OrderPrice>(price))
            .with_order_quantity(static_cast<OrderQuantity>(quantity))
            .with_order_parties(std::move(parties))
            .with_side(Side::Option::Buy)
            .build_limit_order());
  }

  auto make_party(PartyId party_id) -> Party {
    return Party{std::move(party_id),
                 PartyIdSource::Option::BIC,
                 PartyRole::Option::ExecutingFirm};
  }

  auto resting_orders() -> LimitOrdersContainer& {
    return book_.buy_page().limit_orders();
  }

 private:
  OrderBook book_;

 public:
  NiceMock<EventListenerMock> listener;
  RegularOrderMatcher matcher{listener, book_, std::nullopt};
};

TEST_F(SellMarketOrderMatching, DoesNotDetectFacingOrdersInEmptyPage) {
  const MarketOrder order = make_aggressor(OrderId{1}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsFalse());
}

TEST_F(SellMarketOrderMatching, DetectsOppositeFacingOrders) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});

  const MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});

  ASSERT_THAT(matcher.has_facing_orders(order), IsTrue());
}

TEST_F(SellMarketOrderMatching, ThrowsExceptionWhenMatchedWithoutFacingOrders) {
  MarketOrder order = make_aggressor(OrderId{1}, Quantity{100});

  ASSERT_THROW(matcher.match(order), std::logic_error);
}

TEST_F(SellMarketOrderMatching,
       EmitsExecutionReportsWithExecutionIdForBothOrdersOnMatch) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});

  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});

  EXPECT_CALL(
      listener,
      on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
          Field(&protocol::ExecutionReport::execution_id, Ne(std::nullopt))))))
      .Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(SellMarketOrderMatching, EmitsExecutionReportsWithPartiesOnMatch) {
  Party maker_party(PartyId{"Maker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  add_resting_order(OrderId{1}, Price{100}, Quantity{100}, {maker_party});

  Party taker_party(PartyId{"Taker"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm);
  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100}, {taker_party});

  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(taker_party,
                                    Party{maker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::parties,
                        ElementsAre(maker_party,
                                    Party{taker_party.identifier(),
                                          PartyRole::Option::ContraFirm}))))))
      .Times(1);
  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(2);

  matcher.match(order);
}

TEST_F(SellMarketOrderMatching, EmitsOrderReducedOfMakerOnMatch) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});

  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener, on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{0}, OrderId{1}, Side::Option::Buy}))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellMarketOrderMatching, EmitsTradeOnMatch) {
  add_resting_order(
      OrderId{1}, Price{100}, Quantity{100}, {make_party(PartyId{"Maker"})});

  MarketOrder order =
      make_aggressor(OrderId{2}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener, on(IsClientNotification(_))).Times(2);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(_))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker"},
                                                     SellerId{"Taker"},
                                                     Price{100},
                                                     Quantity{100},
                                                     Side::Option::Sell))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellMarketOrderMatching, PopulatesAveragePriceInExecutionReports) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});
  add_resting_order(OrderId{2}, Price{100}, Quantity{100});

  MarketOrder order = make_aggressor(OrderId{3}, Quantity{200});

  EXPECT_CALL(listener, on(IsOrderBookNotification(_))).Times(4);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{101.0}))))))
      .Times(2);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{100.0}))))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(VariantWith<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::average_price,
                        Optional(AveragePrice{100.5}))))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellMarketOrderMatching, FullyMatchesWithFacingOrder) {
  add_resting_order(
      OrderId{1}, Price{100}, Quantity{101}, {make_party(PartyId{"Maker"})});

  MarketOrder order =
      make_aggressor(OrderId{2}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{1}, OrderId{1}, Side::Option::Buy}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker"},
                                                     SellerId{"Taker"},
                                                     Price{100},
                                                     Quantity{100},
                                                     Side::Option::Sell))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellMarketOrderMatching, PartiallyMatchesWithFacingOrder) {
  add_resting_order(
      OrderId{1}, Price{100}, Quantity{99}, {make_party(PartyId{"Maker"})});

  MarketOrder order =
      make_aggressor(OrderId{2}, Quantity{100}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{99},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Cancelled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{99},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{0}, OrderId{1}, Side::Option::Buy}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker"},
                                                     SellerId{"Taker"},
                                                     Price{100},
                                                     Quantity{99},
                                                     Side::Option::Sell))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellMarketOrderMatching, MatchesWithSeveralFacingOrders) {
  add_resting_order(
      OrderId{1}, Price{101}, Quantity{100}, {make_party(PartyId{"Maker1"})});
  add_resting_order(
      OrderId{2}, Price{100}, Quantity{150}, {make_party(PartyId{"Maker2"})});

  MarketOrder order =
      make_aggressor(OrderId{3}, Quantity{200}, {make_party(PartyId{"Taker"})});

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{101},
                                    Quantity{100},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{101},
                                    Quantity{100},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{101}, Quantity{0}, OrderId{1}, Side::Option::Buy}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker1"},
                                                     SellerId{"Taker"},
                                                     Price{101},
                                                     Quantity{100},
                                                     Side::Option::Sell))))
      .Times(1);

  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Sell,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::Filled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsClientNotification(
                  IsExecutionReport(Price{100},
                                    Quantity{100},
                                    Side::Option::Buy,
                                    ExecutionType::Option::OrderTraded,
                                    OrderStatus::Option::PartiallyFilled))))
      .Times(1);
  EXPECT_CALL(listener,
              on(IsOrderBookNotification(VariantWith<OrderReduced>(OrderReduced{
                  Price{100}, Quantity{50}, OrderId{2}, Side::Option::Buy}))))
      .Times(1);
  EXPECT_CALL(
      listener,
      on(IsOrderBookNotification(IsTradeNotification(BuyerId{"Maker2"},
                                                     SellerId{"Taker"},
                                                     Price{100},
                                                     Quantity{100},
                                                     Side::Option::Sell))))
      .Times(1);

  matcher.match(order);
}

TEST_F(SellMarketOrderMatching,
       SetsAggressorAveragePriceFromRestingOrderPrice) {
  add_resting_order(OrderId{1}, Price{100}, Quantity{100});

  MarketOrder order = make_aggressor(OrderId{2}, Quantity{100});
  matcher.match(order);

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{100.0})));
}

TEST_F(SellMarketOrderMatching, SetsWeightedAveragePriceAcrossMultipleFills) {
  add_resting_order(OrderId{1}, Price{101}, Quantity{100});
  add_resting_order(OrderId{2}, Price{100}, Quantity{100});

  MarketOrder order = make_aggressor(OrderId{3}, Quantity{200});
  matcher.match(order);

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{100.5})));
}

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test