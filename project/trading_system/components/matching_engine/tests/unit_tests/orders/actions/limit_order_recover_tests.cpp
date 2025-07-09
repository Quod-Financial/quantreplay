#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/market_state/snapshot.hpp"
#include "ih/orders/actions/limit_order_recover.hpp"
#include "ih/orders/book/order_book.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"

namespace simulator::trading_system::matching_engine {
namespace {

using namespace testing;  // NOLINT

struct MatchingEngineLimitOrderRecover : public Test {
  MatchingEngineLimitOrderRecover() {
    market_state_order.side = Side::Option::Buy;
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;

  LimitOrderRecover recover{event_listener, order_book};

  market_state::LimitOrder market_state_order;
};

TEST_F(MatchingEngineLimitOrderRecover, RecoversBuyOrderInBuyOrderBookPage) {
  market_state_order.side = Side::Option::Buy;

  recover(std::move(market_state_order));
  ASSERT_EQ(order_book.buy_page().limit_orders().size(), 1);
  ASSERT_TRUE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversSellOrderInSellOrderBookPage) {
  market_state_order.side = Side::Option::Sell;

  recover(std::move(market_state_order));
  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
  ASSERT_EQ(order_book.sell_page().limit_orders().size(), 1);
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversClientOrderId) {
  market_state_order.client_order_id = ClientOrderId{"client_order_id"};

  recover(std::move(market_state_order));
  ASSERT_THAT(
      order_book.buy_page().limit_orders(),
      ElementsAre(Property(&LimitOrder::client_order_id,
                           Optional(Eq(ClientOrderId{"client_order_id"})))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversTimeInForce) {
  market_state_order.time_in_force = TimeInForce::Option::GoodTillDate;

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::time_in_force,
                                   Eq(TimeInForce::Option::GoodTillDate))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversExpireTime) {
  using namespace std::chrono_literals;
  constexpr ExpireTime expire_time{core::sys_us{core::sys_days{2025y / 1 / 25} +
                                                13h + 14min + 1s + 123456us}};
  market_state_order.expire_time = expire_time;

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::expire_time,
                                   Optional(Eq(expire_time)))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversExpireDate) {
  using namespace std::chrono_literals;
  constexpr ExpireDate expire_date{core::local_days{2025y / 1 / 25}};
  market_state_order.expire_date = expire_date;

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::expire_date,
                                   Optional(Eq(expire_date)))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversShortSaleExemptionReason) {
  constexpr ShortSaleExemptionReason reason{42};
  market_state_order.short_sale_exemption_reason = reason;

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::short_sale_exemption_reason,
                                   Optional(Eq(reason)))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversOrderParties) {
  const Party party{PartyId{"party_id"},
                    PartyIdSource::Option::BIC,
                    PartyRole::Option::ExecutingFirm};

  market_state_order.order_parties = {party};

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::attributes,
                                   Property(&OrderAttributes::order_parties,
                                            ElementsAre(party)))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversOrderId) {
  market_state_order.order_id = OrderId{42};

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::id, Eq(OrderId{42}))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversSide) {
  market_state_order.side = Side::Option::SellShortExempt;

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.sell_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::side,
                                   Eq(Side::Option::SellShortExempt))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversFixSession) {
  const protocol::fix::Session fix_session{
      protocol::fix::BeginString{"FIXT.1.1"},
      protocol::fix::SenderCompId{"SenderCompID"},
      protocol::fix::TargetCompId{"TargetCompID"}};

  market_state_order.client_session = {market_state::SessionType::Fix,
                                       fix_session};

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::client_session,
                                   Eq(protocol::Session{fix_session}))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversGeneratorSession) {
  market_state_order.client_session = {market_state::SessionType::Generator,
                                       std::nullopt};

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(
                  &LimitOrder::client_session,
                  Eq(protocol::Session{protocol::generator::Session{}}))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversInstrumentDescriptor) {
  InstrumentDescriptor descriptor;
  descriptor.currency = std::make_optional(Currency{"USD"});
  market_state_order.client_instrument_descriptor = descriptor;

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::instrument, Eq(descriptor))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversOrderTime) {
  using namespace std::chrono_literals;
  constexpr auto order_time = OrderTime{core::sys_us{
      core::sys_days{2025y / 1 / 25} + 13h + 14min + 1s + 123456us}};
  market_state_order.order_time = order_time;

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::time, Eq(order_time))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversOrderStatus) {
  market_state_order.order_status = OrderStatus::Option::PartiallyFilled;

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::status,
                                   Eq(OrderStatus::Option::PartiallyFilled))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversOrderPrice) {
  market_state_order.order_price = OrderPrice{100.0};

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::price, Eq(OrderPrice{100.0}))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversTotalQuantity) {
  market_state_order.total_quantity = OrderQuantity{1000.0};

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::total_quantity,
                                   Eq(OrderQuantity{1000.0}))));
}

TEST_F(MatchingEngineLimitOrderRecover, RecoversCumExecutedQuantity) {
  market_state_order.cum_executed_quantity = CumExecutedQuantity{500.0};

  recover(std::move(market_state_order));
  ASSERT_THAT(order_book.buy_page().limit_orders(),
              ElementsAre(Property(&LimitOrder::cum_executed_quantity,
                                   Eq(CumExecutedQuantity{500.0}))));
}

TEST_F(MatchingEngineLimitOrderRecover,
       DoesNotChangeOrderStatusWhenCumExecutedQuantityIsZero) {
  market_state_order.order_status = OrderStatus::Option::New;
  market_state_order.cum_executed_quantity = CumExecutedQuantity{0.0};

  recover(std::move(market_state_order));
  ASSERT_THAT(
      order_book.buy_page().limit_orders(),
      ElementsAre(Property(&LimitOrder::status, Eq(OrderStatus::Option::New))));
}

TEST_F(MatchingEngineLimitOrderRecover, EmitsOrderAddedOnRecover) {
  market_state_order.order_id = OrderId{42};

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(
                  Field(&OrderAdded::order_id, Eq(OrderId{42}))))));

  recover(std::move(market_state_order));
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine
