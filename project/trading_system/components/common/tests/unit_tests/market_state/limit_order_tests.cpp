#include <gmock/gmock.h>

#include <chrono>

#include "common/market_state/json/limit_order.hpp"
#include "common/market_state/snapshot.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::market_state::test {
namespace {
using namespace ::testing;

[[nodiscard]]
auto sample_descriptor() -> InstrumentDescriptor {
  return {.security_id = SecurityId{"security_id"},
          .symbol = Symbol{"symbol"},
          .currency = Currency{"currency"},
          .security_exchange = SecurityExchange{"exchange"},
          .parties = {Party{PartyId{"party_id"},
                            PartyIdSource::Option::BIC,
                            PartyRole::Option::ExecutingFirm}},
          .requester_instrument_id = RequesterInstrumentId{42},
          .security_type = SecurityType::Option::Future,
          .security_id_source = SecurityIdSource::Option::Ric};
}

[[nodiscard]]
auto sample_session() -> Session {
  return {.type = SessionType::Fix,
          .fix_session =
              protocol::fix::Session{protocol::fix::BeginString{"FIX.4.2"},
                                     protocol::fix::SenderCompId{"sender"},
                                     protocol::fix::TargetCompId{"target"}}};
}

[[nodiscard]]
auto sample_parties() -> std::vector<Party> {
  return {Party{PartyId{"PartyId"},
                PartyIdSource::Option::BIC,
                PartyRole::Option::ExecutingFirm}};
}

TEST(TradingSystemCommonLimitOrderFmt, FormatsToString) {
  using namespace std::chrono_literals;
  const LimitOrder order{
      .client_instrument_descriptor = sample_descriptor(),
      .client_session = sample_session(),
      .client_order_id = ClientOrderId{"ClientOrderId"},
      .order_parties = sample_parties(),
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
      .cum_executed_quantity = CumExecutedQuantity{50.5}};

  ASSERT_EQ(
      fmt::to_string(order),
      "{ \"client_instrument_descriptor\": { Symbol=\"symbol\", "
      "SecurityID=\"security_id\", SecurityIDSource=RIC, SecurityType=Future, "
      "SecurityExchange=\"exchange\", Currency=\"currency\", "
      "RequesterInstrumentID=42, Parties=[ { PartyIdentifier={ "
      "PartyID=\"party_id\", PartyIDSource=BIC }, PartyRole=ExecutingFirm } ] "
      "}, \"client_session\": { \"type\": \"Fix\", \"fix_session\": { "
      "BeginString=\"FIX.4.2\", SenderCompID=\"sender\", "
      "TargetCompID=\"target\", ClientSubID=none } }, \"client_order_id\": "
      "\"ClientOrderId\", \"order_parties\": [ { PartyIdentifier={ "
      "PartyID=\"PartyId\", PartyIDSource=BIC }, PartyRole=ExecutingFirm } ], "
      "\"expire_time\": 2023-Oct-01 12:00:00.123456, \"expire_date\": "
      "2023-Oct-01, \"short_sale_exemption_reason\": -1, \"time_in_force\": "
      "GoodTillDate, \"order_id\": 42, \"order_time\": 2023-Oct-01 "
      "13:00:00.123456, \"side\": SellShort, \"order_status\": "
      "PartiallyFilled, \"order_price\": 100.1, \"total_quantity\": 200.2, "
      "\"cum_executed_quantity\": 50.5 }\"");
}

struct TradingSystemCommonLimitOrder : public ::testing::Test {
  const InstrumentDescriptor default_descriptor{sample_descriptor()};
  const Session default_session{sample_session()};
  const std::vector<Party> default_parties{sample_parties()};

  template <typename T>
  [[nodiscard]]
  auto json(const T& descriptor) -> rapidjson::Value {
    rapidjson::Value json_value{rapidjson::Type::kObjectType};
    core::json::Type<T>::write_json_value(
        json_value, doc.GetAllocator(), descriptor);
    return json_value;
  }

  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(TradingSystemCommonLimitOrder, ReadsFromJson) {
  using namespace std::chrono_literals;

  value.SetObject();
  value.AddMember("client_instrument_descriptor",
                  json(default_descriptor).Move(),
                  doc.GetAllocator());
  value.AddMember(
      "client_session", json(default_session).Move(), doc.GetAllocator());
  value.AddMember("client_order_id", "ClientOrderId", doc.GetAllocator());
  value.AddMember(
      "order_parties", json(default_parties).Move(), doc.GetAllocator());
  value.AddMember(
      "expire_time", "2023-10-01 12:00:00.123456", doc.GetAllocator());
  value.AddMember("expire_date", "2023-10-01", doc.GetAllocator());
  value.AddMember("short_sale_exemption_reason", -1, doc.GetAllocator());
  value.AddMember("time_in_force", "GoodTillDate", doc.GetAllocator());
  value.AddMember("order_id", 42, doc.GetAllocator());
  value.AddMember(
      "order_time", "2023-10-01 13:00:00.123456", doc.GetAllocator());
  value.AddMember("side", "SellShort", doc.GetAllocator());
  value.AddMember("order_status", "PartiallyFilled", doc.GetAllocator());
  value.AddMember("order_price", 100.1, doc.GetAllocator());
  value.AddMember("total_quantity", 200.2, doc.GetAllocator());
  value.AddMember("cum_executed_quantity", 50.5, doc.GetAllocator());

  const auto order = core::json::Type<LimitOrder>::read_json_value(value);

  ASSERT_EQ(order.client_instrument_descriptor.security_id,
            default_descriptor.security_id);
  ASSERT_EQ(order.client_session.type, default_session.type);
  ASSERT_EQ(order.client_order_id, ClientOrderId{"ClientOrderId"});
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
}

TEST_F(TradingSystemCommonLimitOrder, WritesToJson) {
  using namespace std::chrono_literals;
  using namespace simulator::trading_system::test;

  const LimitOrder order{
      .client_instrument_descriptor = default_descriptor,
      .client_session = default_session,
      .client_order_id = ClientOrderId{"ClientOrderId"},
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
      .cum_executed_quantity = CumExecutedQuantity{50.5}};

  core::json::Type<LimitOrder>::write_json_value(
      value, doc.GetAllocator(), order);

  ASSERT_TRUE(value.IsObject());
  ASSERT_THAT(value, HasObject("client_instrument_descriptor"));
  ASSERT_THAT(value, HasObject("client_session"));
  ASSERT_THAT(value, HasString("client_order_id", "ClientOrderId"));
  ASSERT_THAT(value, HasArraySize("order_parties", 1));
  ASSERT_THAT(value, HasString("expire_time", "2023-10-01 12:00:00.123456"));
  ASSERT_THAT(value, HasString("expire_date", "2023-10-01"));
  ASSERT_THAT(value, HasInt("short_sale_exemption_reason", -1));
  ASSERT_THAT(value, HasString("time_in_force", "GoodTillDate"));
  ASSERT_THAT(value, HasUInt64("order_id", 42));
  ASSERT_THAT(value, HasString("order_time", "2023-10-01 13:00:00.123456"));
  ASSERT_THAT(value, HasString("side", "SellShort"));
  ASSERT_THAT(value, HasString("order_status", "PartiallyFilled"));
  ASSERT_THAT(value, HasDouble("order_price", 100.1));
  ASSERT_THAT(value, HasDouble("total_quantity", 200.2));
  ASSERT_THAT(value, HasDouble("cum_executed_quantity", 50.5));
}

}  // namespace
}  // namespace simulator::trading_system::market_state::test