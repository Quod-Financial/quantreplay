#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/orders/tools/order_book_state_converter.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {
using namespace ::testing;

struct MatchingEngineOrderBookStateConverterOrderAttributes
    : public ::testing::Test {
  [[nodiscard]]
  static auto create_limit_order(const OrderAttributes& attributes)
      -> LimitOrder {
    OrderRecord order_record{OrderId{42},
                             Side::Option::Buy,
                             protocol::Session{protocol::generator::Session{}},
                             {},
                             attributes};
    return LimitOrder{
        OrderPrice{3.14}, OrderQuantity{2.74}, std::move(order_record)};
  }

  OrderAttributes order_attributes;
  OrderBook order_book;
  market_state::OrderBook order_book_state;
};

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresEmptyOrderBook) {
  store_order_book_state(order_book, order_book_state);

  ASSERT_TRUE(order_book_state.buy_orders.empty());
  ASSERT_TRUE(order_book_state.sell_orders.empty());
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresBuyOrdersInBuyPage) {
  order_book.buy_page().limit_orders().emplace(
      create_limit_order(order_attributes));
  order_book.buy_page().limit_orders().emplace(
      create_limit_order(order_attributes));
  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders.size(), 2);
  ASSERT_TRUE(order_book_state.sell_orders.empty());
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresSellOrdersInSellPage) {
  order_book.sell_page().limit_orders().emplace(
      create_limit_order(order_attributes));
  order_book.sell_page().limit_orders().emplace(
      create_limit_order(order_attributes));
  store_order_book_state(order_book, order_book_state);

  ASSERT_TRUE(order_book_state.buy_orders.empty());
  ASSERT_EQ(order_book_state.sell_orders.size(), 2);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresClientOrderIdInBuyOrder) {
  order_attributes.set_client_order_id(ClientOrderId{"ClientOrderId"});

  order_book.buy_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].client_order_id,
            ClientOrderId{"ClientOrderId"});
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresClientOrderIdInSellOrder) {
  order_attributes.set_client_order_id(ClientOrderId{"ClientOrderId"});

  order_book.sell_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].client_order_id,
            ClientOrderId{"ClientOrderId"});
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresOrderPartiesInBuyOrder) {
  const std::vector<Party> parties{
      {PartyId{"Party1"}, PartyIdSource::Option::BIC, PartyRole::Option::Agent},
      {PartyId{"Party2"},
       PartyIdSource::Option::FDID,
       PartyRole::Option::Buyer}};
  order_attributes.set_order_parties(parties);
  order_book.buy_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].order_parties, parties);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresOrderPartiesInSellOrder) {
  const std::vector<Party> parties{
      {PartyId{"Party1"}, PartyIdSource::Option::BIC, PartyRole::Option::Agent},
      {PartyId{"Party2"},
       PartyIdSource::Option::FDID,
       PartyRole::Option::Buyer}};
  order_attributes.set_order_parties(parties);
  order_book.sell_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].order_parties, parties);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresExpireTimeInBuyOrder) {
  using namespace std::chrono_literals;

  constexpr ExpireTime expire_time{core::sys_us{core::sys_days{2025y / 1 / 25} +
                                                13h + 14min + 1s + 123456us}};
  order_attributes.set_expire_time(expire_time);
  order_book.buy_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].expire_time, expire_time);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresExpireTimeInSellOrder) {
  using namespace std::chrono_literals;

  constexpr ExpireTime expire_time{core::sys_us{core::sys_days{2025y / 1 / 25} +
                                                13h + 14min + 1s + 123456us}};
  order_attributes.set_expire_time(expire_time);
  order_book.sell_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].expire_time, expire_time);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresExpireDateInBuyOrder) {
  using namespace std::chrono_literals;

  constexpr ExpireDate expire_date{core::sys_days{2025y / 1 / 25}};
  order_attributes.set_expire_date(expire_date);
  order_book.buy_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].expire_date, expire_date);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresExpireDateInSellOrder) {
  using namespace std::chrono_literals;

  constexpr ExpireDate expire_date{core::sys_days{2025y / 1 / 25}};
  order_attributes.set_expire_date(expire_date);
  order_book.sell_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].expire_date, expire_date);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresShortSaleExemptionReasonInBuyOrder) {
  order_attributes.set_short_sale_exemption_reason(
      ShortSaleExemptionReason{-1});
  order_book.buy_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].short_sale_exemption_reason,
            ShortSaleExemptionReason{-1});
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresShortSaleExemptionReasonInSellOrder) {
  order_attributes.set_short_sale_exemption_reason(
      ShortSaleExemptionReason{-1});
  order_book.sell_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].short_sale_exemption_reason,
            ShortSaleExemptionReason{-1});
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresTimeInForceInBuyOrder) {
  order_attributes.set_time_in_force(TimeInForce::Option::GoodTillDate);
  order_book.buy_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].time_in_force,
            TimeInForce::Option::GoodTillDate);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderAttributes,
       StoresTimeInForceInSellOrder) {
  order_attributes.set_time_in_force(TimeInForce::Option::GoodTillDate);
  order_book.sell_page().limit_orders().emplace(
      create_limit_order(order_attributes));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].time_in_force,
            TimeInForce::Option::GoodTillDate);
}

struct MatchingEngineOrderBookStateConverterOrderRecord
    : public ::testing::Test {
  [[nodiscard]]
  static auto create_limit_order(OrderRecord&& order_record) -> LimitOrder {
    return LimitOrder{
        OrderPrice{3.14}, OrderQuantity{2.74}, std::move(order_record)};
  }

  OrderBook order_book;
  market_state::OrderBook order_book_state;
};

MATCHER(IsGeneratorSession, "") {
  return ExplainMatchResult(Eq(market_state::SessionType::Generator),
                            arg.type,
                            result_listener) &&
         ExplainMatchResult(Eq(std::nullopt), arg.fix_session, result_listener);
}

MATCHER_P(IsFixSession, expected, "") {
  return ExplainMatchResult(
             Eq(market_state::SessionType::Fix), arg.type, result_listener) &&
         ExplainMatchResult(Eq(expected), arg.fix_session, result_listener) &&
         ExplainMatchResult(Eq(expected.client_sub_id),
                            arg.fix_session->client_sub_id,
                            result_listener);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresClientInstrumentDescriptorInBuyOrder) {
  InstrumentDescriptor descriptor;
  descriptor.symbol = std::make_optional(Symbol{"Symbol"});
  OrderRecord order_record{OrderId{42},
                           Side::Option::Buy,
                           protocol::Session{protocol::generator::Session{}},
                           descriptor,
                           {}};

  order_book.buy_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].client_instrument_descriptor.symbol,
            Symbol{"Symbol"});
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresClientInstrumentDescriptorInSellOrder) {
  InstrumentDescriptor descriptor;
  descriptor.symbol = std::make_optional(Symbol{"Symbol"});
  OrderRecord order_record{OrderId{42},
                           Side::Option::Sell,
                           protocol::Session{protocol::generator::Session{}},
                           descriptor,
                           {}};

  order_book.sell_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].client_instrument_descriptor.symbol,
            Symbol{"Symbol"});
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresOrderIdInBuyOrder) {
  OrderRecord order_record{OrderId{42},
                           Side::Option::Buy,
                           protocol::Session{protocol::generator::Session{}},
                           {},
                           {}};

  order_book.buy_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].order_id, OrderId{42});
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresOrderIdInSellOrder) {
  OrderRecord order_record{OrderId{42},
                           Side::Option::Sell,
                           protocol::Session{protocol::generator::Session{}},
                           {},
                           {}};

  order_book.sell_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].order_id, OrderId{42});
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresOrderTimeInBuyOrder) {
  using namespace std::chrono_literals;

  OrderRecord order_record{OrderId{42},
                           Side::Option::Buy,
                           protocol::Session{protocol::generator::Session{}},
                           {},
                           {}};
  order_record.set_order_time(OrderTime{core::sys_us{
      core::sys_days{2025y / 1 / 25} + 13h + 14min + 1s + 123456us}});

  order_book.buy_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].order_time,
            OrderTime{core::sys_us{core::sys_days{2025y / 1 / 25} + 13h +
                                   14min + 1s + 123456us}});
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresOrderTimeInSellOrder) {
  using namespace std::chrono_literals;

  OrderRecord order_record{OrderId{42},
                           Side::Option::Sell,
                           protocol::Session{protocol::generator::Session{}},
                           {},
                           {}};
  order_record.set_order_time(OrderTime{core::sys_us{
      core::sys_days{2025y / 1 / 25} + 13h + 14min + 1s + 123456us}});

  order_book.sell_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].order_time,
            OrderTime{core::sys_us{core::sys_days{2025y / 1 / 25} + 13h +
                                   14min + 1s + 123456us}});
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord, StoresSideInBuyOrder) {
  OrderRecord order_record{OrderId{42},
                           Side::Option::Buy,
                           protocol::Session{protocol::generator::Session{}},
                           {},
                           {}};

  order_book.buy_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].side, Side::Option::Buy);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresSideInSellOrder) {
  OrderRecord order_record{OrderId{42},
                           Side::Option::Sell,
                           protocol::Session{protocol::generator::Session{}},
                           {},
                           {}};

  order_book.sell_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].side, Side::Option::Sell);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresOrderStatusInBuyOrder) {
  OrderRecord order_record{OrderId{42},
                           Side::Option::Buy,
                           protocol::Session{protocol::generator::Session{}},
                           {},
                           {}};
  order_record.set_order_status(OrderStatus::Option::PartiallyFilled);

  order_book.buy_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].order_status,
            OrderStatus::Option::PartiallyFilled);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresOrderStatusInSellOrder) {
  OrderRecord order_record{OrderId{42},
                           Side::Option::Sell,
                           protocol::Session{protocol::generator::Session{}},
                           {},
                           {}};
  order_record.set_order_status(OrderStatus::Option::PartiallyFilled);

  order_book.sell_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].order_status,
            OrderStatus::Option::PartiallyFilled);
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresGeneratorSessionInBuyOrder) {
  OrderRecord order_record{OrderId{42},
                           Side::Option::Buy,
                           protocol::Session{protocol::generator::Session{}},
                           {},
                           {}};

  order_book.buy_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_THAT(order_book_state.buy_orders[0].client_session,
              IsGeneratorSession());
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresGeneratorSessionInSellOrder) {
  OrderRecord order_record{OrderId{42},
                           Side::Option::Sell,
                           protocol::Session{protocol::generator::Session{}},
                           {},
                           {}};

  order_book.sell_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_THAT(order_book_state.sell_orders[0].client_session,
              IsGeneratorSession());
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresFixSessionInBuyOrder) {
  const protocol::fix::Session fix_session{
      protocol::fix::BeginString{"begin"},
      protocol::fix::SenderCompId{"sender"},
      protocol::fix::TargetCompId{"target"},
      protocol::fix::ClientSubId{"sub_id"}};
  OrderRecord order_record{
      OrderId{42}, Side::Option::Buy, protocol::Session{fix_session}, {}, {}};

  order_book.buy_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_THAT(order_book_state.buy_orders[0].client_session,
              IsFixSession(fix_session));
}

TEST_F(MatchingEngineOrderBookStateConverterOrderRecord,
       StoresFixSessionInSellOrder) {
  const protocol::fix::Session fix_session{
      protocol::fix::BeginString{"begin"},
      protocol::fix::SenderCompId{"sender"},
      protocol::fix::TargetCompId{"target"},
      protocol::fix::ClientSubId{"sub_id"}};
  OrderRecord order_record{
      OrderId{42}, Side::Option::Sell, protocol::Session{fix_session}, {}, {}};

  order_book.sell_page().limit_orders().emplace(
      create_limit_order(std::move(order_record)));

  store_order_book_state(order_book, order_book_state);

  ASSERT_THAT(order_book_state.sell_orders[0].client_session,
              IsFixSession(fix_session));
}

struct MatchingEngineOrderBookStateConverterLimitOrder
    : public ::testing::Test {
  OrderRecord order_record{OrderId{42},
                           Side::Option::Buy,
                           protocol::Session{protocol::generator::Session{}},
                           {},
                           {}};
  OrderBook order_book;
  market_state::OrderBook order_book_state;
};

TEST_F(MatchingEngineOrderBookStateConverterLimitOrder,
       StoresOrderPriceInBuyOrder) {
  const LimitOrder order{
      OrderPrice{3.14}, OrderQuantity{2.74}, std::move(order_record)};
  order_book.buy_page().limit_orders().emplace(order);

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].order_price, OrderPrice{3.14});
}

TEST_F(MatchingEngineOrderBookStateConverterLimitOrder,
       StoresOrderPriceInSellOrder) {
  const LimitOrder order{
      OrderPrice{3.14}, OrderQuantity{2.74}, std::move(order_record)};
  order_book.sell_page().limit_orders().emplace(order);

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].order_price, OrderPrice{3.14});
}

TEST_F(MatchingEngineOrderBookStateConverterLimitOrder,
       StoresTotalQuantityInBuyOrder) {
  const LimitOrder order{
      OrderPrice{3.14}, OrderQuantity{2.74}, std::move(order_record)};
  order_book.buy_page().limit_orders().emplace(order);

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].total_quantity, OrderQuantity{2.74});
}

TEST_F(MatchingEngineOrderBookStateConverterLimitOrder,
       StoresTotalQuantityInSellOrder) {
  const LimitOrder order{
      OrderPrice{3.14}, OrderQuantity{2.74}, std::move(order_record)};
  order_book.sell_page().limit_orders().emplace(order);

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].total_quantity,
            OrderQuantity{2.74});
}

TEST_F(MatchingEngineOrderBookStateConverterLimitOrder,
       StoresCumExecutedQuantityInBuyOrder) {
  const LimitOrder order{
      OrderPrice{3.14}, OrderQuantity{2.74}, std::move(order_record)};
  order_book.buy_page().limit_orders().emplace(order);

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.buy_orders[0].cum_executed_quantity,
            CumExecutedQuantity{0.0});
}

TEST_F(MatchingEngineOrderBookStateConverterLimitOrder,
       StoresCumExecutedQuantityInSellOrder) {
  const LimitOrder order{
      OrderPrice{3.14}, OrderQuantity{2.74}, std::move(order_record)};
  order_book.sell_page().limit_orders().emplace(order);

  store_order_book_state(order_book, order_book_state);

  ASSERT_EQ(order_book_state.sell_orders[0].cum_executed_quantity,
            CumExecutedQuantity{0.0});
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
