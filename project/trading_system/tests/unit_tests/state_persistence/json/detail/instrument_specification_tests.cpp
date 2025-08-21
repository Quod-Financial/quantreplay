#include <gtest/gtest.h>

#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;
using namespace simulator::trading_system::test;

struct TradingSystemJsonInstrumentSpecification : public Test {
  [[nodiscard]]
  static auto specification_sample()
      -> instrument::Cache::InstrumentSpecification {
    return instrument::Cache::InstrumentSpecification{
        .symbol = Symbol{"AAPL-XLON"},
        .price_currency = PriceCurrency{"USD"},
        .base_currency = BaseCurrency{"EUR"},
        .security_exchange = SecurityExchange{"XLON"},
        .party_id = PartyId{"ContraFirm"},
        .cusip = CusipId{"cusip"},
        .sedol = SedolId{"sedol"},
        .isin = IsinId{"isin"},
        .ric = RicId{"ric"},
        .exchange_id = ExchangeId{"ExchangeID"},
        .bloomberg_id = BloombergId{"BloombergID"},
        .party_role = PartyRole::Option::ContraFirm,
        .security_type = SecurityType::Option::CommonStock};
  }

  instrument::Cache::InstrumentSpecification specification;
};

TEST_F(TradingSystemJsonInstrumentSpecification, ReadsDefaultFromJson) {
  rapidjson::Document doc;
  doc.Parse(R"({
    "Symbol": null,
    "PriceCurrency": null,
    "BaseCurrency": null,
    "SecurityExchange": null,
    "PartyID": null,
    "CUSIP": null,
    "SEDOL": null,
    "ISIN": null,
    "RIC": null,
    "ExchangeSymbol": null,
    "BloombergSymbol": null,
    "PartyRole": null,
    "SecurityType": null
  })");

  ASSERT_TRUE(json::read(doc, specification).has_value());

  ASSERT_EQ(specification.symbol, std::nullopt);
  ASSERT_EQ(specification.price_currency, std::nullopt);
  ASSERT_EQ(specification.base_currency, std::nullopt);
  ASSERT_EQ(specification.security_exchange, std::nullopt);
  ASSERT_EQ(specification.party_id, std::nullopt);
  ASSERT_EQ(specification.cusip, std::nullopt);
  ASSERT_EQ(specification.sedol, std::nullopt);
  ASSERT_EQ(specification.isin, std::nullopt);
  ASSERT_EQ(specification.ric, std::nullopt);
  ASSERT_EQ(specification.exchange_id, std::nullopt);
  ASSERT_EQ(specification.bloomberg_id, std::nullopt);
  ASSERT_EQ(specification.party_role, std::nullopt);
  ASSERT_EQ(specification.security_type, std::nullopt);
}

TEST_F(TradingSystemJsonInstrumentSpecification, ReadsFromJson) {
  rapidjson::Document doc;
  doc.Parse(R"({
    "Symbol": "AAPL-XLON",
    "PriceCurrency": "USD",
    "BaseCurrency": "EUR",
    "SecurityExchange": "XLON",
    "PartyID": "ContraFirm",
    "CUSIP": "cusip",
    "SEDOL": "sedol",
    "ISIN": "isin",
    "RIC": "ric",
    "ExchangeSymbol": "ExchangeID",
    "BloombergSymbol": "BloombergID",
    "PartyRole": "ContraFirm",
    "SecurityType": "CommonStock"
  })");

  ASSERT_TRUE(json::read(doc, specification).has_value());

  ASSERT_THAT(specification.symbol, Optional(Eq(Symbol{"AAPL-XLON"})));
  ASSERT_THAT(specification.security_type,
              Optional(Eq(SecurityType::Option::CommonStock)));
  ASSERT_THAT(specification.price_currency, Optional(Eq(Currency{"USD"})));
  ASSERT_THAT(specification.base_currency, Optional(Eq(Currency{"EUR"})));
  ASSERT_THAT(specification.security_exchange,
              Optional(Eq(SecurityExchange{"XLON"})));
  ASSERT_THAT(specification.party_id, Optional(Eq(PartyId{"ContraFirm"})));
  ASSERT_THAT(specification.party_role,
              Optional(Eq(PartyRole::Option::ContraFirm)));
  ASSERT_THAT(specification.cusip, Optional(Eq(SecurityId{"cusip"})));
  ASSERT_THAT(specification.sedol, Optional(Eq(SecurityId{"sedol"})));
  ASSERT_THAT(specification.isin, Optional(Eq(SecurityId{"isin"})));
  ASSERT_THAT(specification.ric, Optional(Eq(SecurityId{"ric"})));
  ASSERT_THAT(specification.exchange_id,
              Optional(Eq(SecurityId{"ExchangeID"})));
  ASSERT_THAT(specification.bloomberg_id,
              Optional(Eq(SecurityId{"BloombergID"})));
}

TEST_F(TradingSystemJsonInstrumentSpecification,
       WritingSetsJsonValueTypeToObject) {
  rapidjson::Document doc;
  rapidjson::Value json_value;

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), specification).has_value());

  ASSERT_TRUE(json_value.IsObject());
}

TEST_F(TradingSystemJsonInstrumentSpecification, WritesDefaultToJson) {
  rapidjson::Document doc;
  rapidjson::Value json_value;

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), specification).has_value());

  ASSERT_THAT(json_value, HasNull("Symbol"));
  ASSERT_THAT(json_value, HasNull("PriceCurrency"));
  ASSERT_THAT(json_value, HasNull("BaseCurrency"));
  ASSERT_THAT(json_value, HasNull("SecurityExchange"));
  ASSERT_THAT(json_value, HasNull("PartyID"));
  ASSERT_THAT(json_value, HasNull("CUSIP"));
  ASSERT_THAT(json_value, HasNull("SEDOL"));
  ASSERT_THAT(json_value, HasNull("ISIN"));
  ASSERT_THAT(json_value, HasNull("RIC"));
  ASSERT_THAT(json_value, HasNull("ExchangeSymbol"));
  ASSERT_THAT(json_value, HasNull("BloombergSymbol"));
  ASSERT_THAT(json_value, HasNull("PartyRole"));
  ASSERT_THAT(json_value, HasNull("SecurityType"));
}

TEST_F(TradingSystemJsonInstrumentSpecification, WritesToJson) {
  specification = specification_sample();

  rapidjson::Document doc;
  rapidjson::Value json_value;
  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), specification).has_value());

  ASSERT_THAT(json_value, HasString("Symbol", "AAPL-XLON"));
  ASSERT_THAT(json_value, HasString("PriceCurrency", "USD"));
  ASSERT_THAT(json_value, HasString("BaseCurrency", "EUR"));
  ASSERT_THAT(json_value, HasString("SecurityExchange", "XLON"));
  ASSERT_THAT(json_value, HasString("PartyID", "ContraFirm"));
  ASSERT_THAT(json_value, HasString("CUSIP", "cusip"));
  ASSERT_THAT(json_value, HasString("SEDOL", "sedol"));
  ASSERT_THAT(json_value, HasString("ISIN", "isin"));
  ASSERT_THAT(json_value, HasString("RIC", "ric"));
  ASSERT_THAT(json_value, HasString("ExchangeSymbol", "ExchangeID"));
  ASSERT_THAT(json_value, HasString("BloombergSymbol", "BloombergID"));
  ASSERT_THAT(json_value, HasString("PartyRole", "ContraFirm"));
  ASSERT_THAT(json_value, HasString("SecurityType", "CommonStock"));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
