#include <gmock/gmock.h>

#include "common/instrument_state.hpp"

namespace simulator::trading_system::market_state::test {
namespace {

using namespace ::testing;

TEST(TradingSystemStatePersistenceSessionTypeFmt, FormatsFixToString) {
  ASSERT_EQ(fmt::to_string(SessionType::Fix), "\"Fix\"");
}

TEST(TradingSystemStatePersistenceSessionTypeFmt, FormatsGeneratorToString) {
  ASSERT_EQ(fmt::to_string(SessionType::Generator), "\"Generator\"");
}

TEST(TradingSystemCommonSessionFmt, FormatsFixToString) {
  const protocol::fix::Session fix_session{
      protocol::fix::BeginString{"begin"},
      protocol::fix::SenderCompId{"sender"},
      protocol::fix::TargetCompId{"target"}};
  const market_state::Session session{market_state::SessionType::Fix,
                                      fix_session};
  ASSERT_EQ(fmt::to_string(session),
            "{ \"type\": \"Fix\", \"fix_session\": { "
            "BeginString=\"begin\", "
            "SenderCompID=\"sender\", TargetCompID=\"target\", "
            "SessionQualifier=none, ClientSubID=none } }");
}

TEST(TradingSystemCommonSessionFmt, FormatsGeneratedToString) {
  const market_state::Session session{market_state::SessionType::Generator,
                                      std::nullopt};
  ASSERT_EQ(fmt::to_string(session),
            R"({ "type": "Generator", "fix_session": none })");
}

TEST(TradingSystemMarketStateInstrumentInfoFmt, FormatsToString) {
  const InstrumentInfo instrument_info{.low_price = Price{100.1},
                                       .high_price = Price{300.3}};
  ASSERT_EQ(fmt::to_string(instrument_info),
            R"({ "low_price": 100.1, "high_price": 300.3 })");
}

struct TradingSystemCommonLimitOrderFmt : public ::testing::Test {
  [[nodiscard]]
  static auto sample_descriptor() -> InstrumentDescriptor {
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
  static auto sample_session() -> Session {
    return {.type = SessionType::Fix,
            .fix_session =
                protocol::fix::Session{protocol::fix::BeginString{"FIX.4.2"},
                                       protocol::fix::SenderCompId{"sender"},
                                       protocol::fix::TargetCompId{"target"}}};
  }

  [[nodiscard]]
  static auto sample_parties() -> std::vector<Party> {
    return {Party{PartyId{"PartyId"},
                  PartyIdSource::Option::BIC,
                  PartyRole::Option::ExecutingFirm}};
  }
};

TEST_F(TradingSystemCommonLimitOrderFmt, FormatsToString) {
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
      .cum_executed_quantity = CumExecutedQuantity{50.5},
      .cum_px_qty = 5065.05};

  ASSERT_EQ(
      fmt::to_string(order),
      "{ \"client_instrument_descriptor\": { Symbol=\"symbol\", "
      "SecurityID=\"security_id\", SecurityIDSource=RIC, SecurityType=Future, "
      "SecurityExchange=\"exchange\", Currency=\"currency\", "
      "RequesterInstrumentID=42, Parties=[ { PartyIdentifier={ "
      "PartyID=\"party_id\", PartyIDSource=BIC }, PartyRole=ExecutingFirm } ] "
      "}, \"client_session\": { \"type\": \"Fix\", \"fix_session\": { "
      "BeginString=\"FIX.4.2\", SenderCompID=\"sender\", "
      "TargetCompID=\"target\", SessionQualifier=none, ClientSubID=none } }, "
      "\"client_order_id\": \"ClientOrderId\", \"order_parties\": [ { PartyIdentifier={ "
      "PartyID=\"PartyId\", PartyIDSource=BIC }, PartyRole=ExecutingFirm } ], "
      "\"expire_time\": 2023-Oct-01 12:00:00.123456, \"expire_date\": "
      "2023-Oct-01, \"short_sale_exemption_reason\": -1, "
      "\"time_in_force\": GoodTillDate, \"order_id\": 42, \"order_time\": "
      "2023-Oct-01 13:00:00.123456, \"side\": SellShort, \"order_status\": "
      "PartiallyFilled, \"order_price\": 100.1, \"total_quantity\": 200.2, "
      "\"cum_executed_quantity\": 50.5, \"cum_px_qty\": 5065.05 }\"");
}

}  // namespace
}  // namespace simulator::trading_system::market_state::test
