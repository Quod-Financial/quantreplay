#include <gmock/gmock.h>

#include <chrono>

#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;

[[nodiscard]]
auto sample_descriptor() -> simulator::InstrumentDescriptor {
  return {.security_id = SecurityId{"security_id"},
          .symbol = Symbol{"symbol"},
          .currency = Currency{"currency"},
          .security_exchange = SecurityExchange{"exchange"},
          .parties = {simulator::Party{PartyId{"party_id"},
                                       PartyIdSource::Option::BIC,
                                       PartyRole::Option::ExecutingFirm}},
          .requester_instrument_id = RequesterInstrumentId{42},
          .security_type = SecurityType::Option::Future,
          .security_id_source = SecurityIdSource::Option::Ric};
}

[[nodiscard]]
auto sample_session() -> market_state::Session {
  return {.type = market_state::SessionType::Fix,
          .fix_session =
              protocol::fix::Session{protocol::fix::BeginString{"FIX.4.2"},
                                     protocol::fix::SenderCompId{"sender"},
                                     protocol::fix::TargetCompId{"target"}}};
}

[[nodiscard]]
auto sample_parties() -> std::vector<simulator::Party> {
  return {simulator::Party{PartyId{"PartyId"},
                           PartyIdSource::Option::BIC,
                           PartyRole::Option::ExecutingFirm}};
}

struct TradingSystemJsonLimitOrder : public ::testing::Test {
  const simulator::InstrumentDescriptor default_descriptor{sample_descriptor()};
  const market_state::Session default_session{sample_session()};
  const std::vector<simulator::Party> default_parties{sample_parties()};

  template <typename T>
  [[nodiscard]]
  auto json(const T& object) -> rapidjson::Value {
    rapidjson::Value value;
    json::write(value, doc.GetAllocator(), object);
    return value;
  }

  rapidjson::Document doc;
  rapidjson::Value json_value;
};

TEST_F(TradingSystemJsonLimitOrder, ReadsFromJson) {
  using namespace std::chrono_literals;

  json_value.SetObject();
  json_value.AddMember("ClientInstrumentDescriptor",
                       json(default_descriptor).Move(),
                       doc.GetAllocator());
  json_value.AddMember(
      "ClientSession", json(default_session).Move(), doc.GetAllocator());
  json_value.AddMember("ClientOrderID", "client_order_id", doc.GetAllocator());
  json_value.AddMember(
      "OrderParties", json(default_parties).Move(), doc.GetAllocator());
  json_value.AddMember(
      "ExpireTime", "20231001-12:00:00.123456", doc.GetAllocator());
  json_value.AddMember("ExpireDate", "20231001", doc.GetAllocator());
  json_value.AddMember("ShortSaleExemptionReason", -1, doc.GetAllocator());
  json_value.AddMember("TimeInForce", "GoodTillDate", doc.GetAllocator());
  json_value.AddMember("OrderID", 42, doc.GetAllocator());
  json_value.AddMember(
      "OrderTime", "20231001-13:00:00.123456", doc.GetAllocator());
  json_value.AddMember("Side", "SellShort", doc.GetAllocator());
  json_value.AddMember("OrdStatus", "PartiallyFilled", doc.GetAllocator());
  json_value.AddMember("Price", 100.1, doc.GetAllocator());
  json_value.AddMember("OrderQty", 200.2, doc.GetAllocator());
  json_value.AddMember("CumQty", 50.5, doc.GetAllocator());
  json_value.AddMember("CumPxQty", 99.9, doc.GetAllocator());

  market_state::LimitOrder order;
  ASSERT_TRUE(json::read(json_value, order).has_value());

  ASSERT_EQ(order.client_instrument_descriptor.security_id,
            default_descriptor.security_id);
  ASSERT_EQ(order.client_session.type, default_session.type);
  ASSERT_EQ(order.client_order_id, ClientOrderId{"client_order_id"});
  ASSERT_EQ(order.order_parties, default_parties);
  ASSERT_EQ(order.expire_time, ExpireTime{core::sys_us{1696161600123456us}});
  ASSERT_EQ(order.expire_date, ExpireDate{core::local_days{2023y / 10 / 01}});
  ASSERT_EQ(order.short_sale_exemption_reason, ShortSaleExemptionReason{-1});
  ASSERT_EQ(order.time_in_force, TimeInForce::Option::GoodTillDate);
  ASSERT_EQ(order.order_id, OrderId{42});
  ASSERT_EQ(order.order_time, OrderTime{core::sys_us{1696165200123456us}});
  ASSERT_EQ(order.side, Side::Option::SellShort);
  ASSERT_EQ(order.order_status, OrderStatus::Option::PartiallyFilled);
  ASSERT_EQ(order.order_price, OrderPrice{100.1});
  ASSERT_EQ(order.total_quantity, OrderQuantity{200.2});
  ASSERT_EQ(order.cum_executed_quantity, CumExecutedQuantity{50.5});
  ASSERT_DOUBLE_EQ(order.cum_px_qty, 99.9);
}

TEST_F(TradingSystemJsonLimitOrder,
       DerivesCumPxQtyFromPriceAndCumQtyWhenMissing) {
  using namespace simulator::trading_system::test;

  json_value.SetObject();
  json_value.AddMember(
      "ClientSession", json(default_session).Move(), doc.GetAllocator());
  json_value.AddMember("ClientInstrumentDescriptor",
                       json(default_descriptor).Move(),
                       doc.GetAllocator());
  json_value.AddMember(
      "OrderParties", json(default_parties).Move(), doc.GetAllocator());
  json_value.AddMember("TimeInForce", "Day", doc.GetAllocator());
  json_value.AddMember(
      "OrderID", rapidjson::Value{}.SetUint64(0), doc.GetAllocator());
  json_value.AddMember(
      "OrderTime", "20231001-12:00:00.000000", doc.GetAllocator());
  json_value.AddMember("Side", "Buy", doc.GetAllocator());
  json_value.AddMember("OrdStatus", "New", doc.GetAllocator());
  json_value.AddMember("Price", 10.0, doc.GetAllocator());
  json_value.AddMember("OrderQty", 100.0, doc.GetAllocator());
  json_value.AddMember("CumQty", 50.0, doc.GetAllocator());

  market_state::LimitOrder order;
  ASSERT_TRUE(json::read(json_value, order).has_value());
  ASSERT_DOUBLE_EQ(order.cum_px_qty, 500.0);
}

TEST_F(TradingSystemJsonLimitOrder, WritingSetsJsonValueTypeToObject) {
  using namespace std::chrono_literals;
  using namespace simulator::trading_system::test;

  const market_state::LimitOrder order;

  ASSERT_TRUE(json::write(json_value, doc.GetAllocator(), order).has_value());

  ASSERT_TRUE(json_value.IsObject());
}

TEST_F(TradingSystemJsonLimitOrder, WritesToJson) {
  using namespace std::chrono_literals;
  using namespace simulator::trading_system::test;

  const market_state::LimitOrder order{
      .client_instrument_descriptor = default_descriptor,
      .client_session = default_session,
      .client_order_id = ClientOrderId{"client_order_id"},
      .order_parties = default_parties,
      .expire_time = ExpireTime{core::sys_us{1696161600123456us}},
      .expire_date = ExpireDate{core::local_days{2023y / 10 / 01}},
      .short_sale_exemption_reason = ShortSaleExemptionReason{-1},
      .time_in_force = TimeInForce::Option::GoodTillDate,
      .order_id = OrderId{42},
      .order_time = OrderTime{core::sys_us{1696165200123456us}},
      .side = Side::Option::SellShort,
      .order_status = OrderStatus::Option::PartiallyFilled,
      .order_price = OrderPrice{100.1},
      .total_quantity = OrderQuantity{200.2},
      .cum_executed_quantity = CumExecutedQuantity{50.5},
      .cum_px_qty = 75.3};

  ASSERT_TRUE(json::write(json_value, doc.GetAllocator(), order).has_value());

  ASSERT_THAT(json_value, HasObject("ClientInstrumentDescriptor"));
  ASSERT_THAT(json_value, HasObject("ClientSession"));
  ASSERT_THAT(json_value, HasString("ClientOrderID", "client_order_id"));
  ASSERT_THAT(json_value, HasArraySize("OrderParties", 1));
  ASSERT_THAT(json_value, HasString("ExpireTime", "20231001-12:00:00.123456"));
  ASSERT_THAT(json_value, HasString("ExpireDate", "20231001"));
  ASSERT_THAT(json_value, HasInt("ShortSaleExemptionReason", -1));
  ASSERT_THAT(json_value, HasString("TimeInForce", "GoodTillDate"));
  ASSERT_THAT(json_value, HasUInt64("OrderID", 42));
  ASSERT_THAT(json_value, HasString("OrderTime", "20231001-13:00:00.123456"));
  ASSERT_THAT(json_value, HasString("Side", "SellShort"));
  ASSERT_THAT(json_value, HasString("OrdStatus", "PartiallyFilled"));
  ASSERT_THAT(json_value, HasDouble("Price", 100.1));
  ASSERT_THAT(json_value, HasDouble("OrderQty", 200.2));
  ASSERT_THAT(json_value, HasDouble("CumQty", 50.5));
  ASSERT_THAT(json_value, HasDouble("CumPxQty", 75.3));
}

TEST_F(TradingSystemJsonLimitOrder, WritesZeroCumPxQtyWhenNoFills) {
  using namespace simulator::trading_system::test;

  const market_state::LimitOrder order;

  ASSERT_TRUE(json::write(json_value, doc.GetAllocator(), order).has_value());

  ASSERT_THAT(json_value, HasDouble("CumPxQty", 0.0));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
