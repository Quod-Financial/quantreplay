#include "common/instrument.hpp"

#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>

#include "common/json/instrument.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace testing;  // NOLINT

struct TradingSystemInstrument : public Test {
  [[nodiscard]]
  static auto instrument_sample() -> Instrument {
    Instrument instrument;
    instrument.identifier = InstrumentId{42};
    instrument.symbol = Symbol{"AAPL-XLON"};
    instrument.security_type = SecurityType::Option::CommonStock;
    instrument.price_currency = PriceCurrency{"USD"};
    instrument.base_currency = BaseCurrency{"EUR"};
    instrument.security_exchange = SecurityExchange{"XLON"};
    instrument.party_id = PartyId{"ContraFirm"};
    instrument.party_role = PartyRole::Option::ContraFirm;
    instrument.cusip = CusipId{"CUSIP"};
    instrument.sedol = SedolId{"SEDOL"};
    instrument.isin = IsinId{"ISIN"};
    instrument.ric = RicId{"RIC"};
    instrument.exchange_id = ExchangeId{"ExchangeID"};
    instrument.bloomberg_id = BloombergId{"BloombergID"};
    instrument.database_id = DatabaseId{42};
    instrument.price_tick = PriceTick{42.42};
    instrument.quantity_tick = QuantityTick{42.42};
    instrument.min_quantity = MinQuantity{42.42};
    instrument.max_quantity = MaxQuantity{42.42};
    return instrument;
  }

  Instrument instrument;
};

// NOLINTBEGIN(*-magic-numbers)

TEST_F(TradingSystemInstrument, CreateInstrument) {
  EXPECT_EQ(instrument.identifier, InstrumentId{0});
  EXPECT_EQ(instrument.symbol, std::nullopt);
  EXPECT_EQ(instrument.security_type, std::nullopt);
  EXPECT_EQ(instrument.price_currency, std::nullopt);
  EXPECT_EQ(instrument.base_currency, std::nullopt);
  EXPECT_EQ(instrument.security_exchange, std::nullopt);
  EXPECT_EQ(instrument.party_id, std::nullopt);
  EXPECT_EQ(instrument.party_role, std::nullopt);
  EXPECT_EQ(instrument.cusip, std::nullopt);
  EXPECT_EQ(instrument.sedol, std::nullopt);
  EXPECT_EQ(instrument.isin, std::nullopt);
  EXPECT_EQ(instrument.ric, std::nullopt);
  EXPECT_EQ(instrument.exchange_id, std::nullopt);
  EXPECT_EQ(instrument.bloomberg_id, std::nullopt);
  EXPECT_EQ(instrument.database_id, std::nullopt);
  EXPECT_EQ(instrument.price_tick, std::nullopt);
  EXPECT_EQ(instrument.quantity_tick, std::nullopt);
  EXPECT_EQ(instrument.min_quantity, std::nullopt);
  EXPECT_EQ(instrument.max_quantity, std::nullopt);
}

TEST_F(TradingSystemInstrument, SpecifyAttributes) {
  instrument = instrument_sample();

  EXPECT_EQ(instrument.identifier, InstrumentId{42});
  EXPECT_THAT(instrument.symbol, Optional(Eq(Symbol{"AAPL-XLON"})));
  EXPECT_THAT(instrument.price_currency, Optional(Eq(Currency{"USD"})));
  EXPECT_THAT(instrument.base_currency, Optional(Eq(Currency{"EUR"})));
  EXPECT_THAT(instrument.party_id, Optional(Eq(PartyId{"ContraFirm"})));
  EXPECT_THAT(instrument.cusip, Optional(Eq(SecurityId{"CUSIP"})));
  EXPECT_THAT(instrument.sedol, Optional(Eq(SecurityId{"SEDOL"})));
  EXPECT_THAT(instrument.isin, Optional(Eq(SecurityId{"ISIN"})));
  EXPECT_THAT(instrument.ric, Optional(Eq(SecurityId{"RIC"})));
  EXPECT_THAT(instrument.exchange_id, Optional(Eq(SecurityId{"ExchangeID"})));
  EXPECT_THAT(instrument.bloomberg_id, Optional(Eq(SecurityId{"BloombergID"})));
  EXPECT_THAT(instrument.database_id, Optional(Eq(DatabaseId{42})));
  EXPECT_THAT(instrument.security_type,
              Optional(Eq(SecurityType::Option::CommonStock)));
  EXPECT_THAT(instrument.security_exchange,
              Optional(Eq(SecurityExchange{"XLON"})));
  EXPECT_THAT(instrument.party_role,
              Optional(Eq(PartyRole::Option::ContraFirm)));
  EXPECT_THAT(instrument.price_tick, Optional(Eq(PriceTick{42.42})));
  EXPECT_THAT(instrument.quantity_tick, Optional(Eq(QuantityTick{42.42})));
  EXPECT_THAT(instrument.min_quantity, Optional(Eq(MinQuantity{42.42})));
  EXPECT_THAT(instrument.max_quantity, Optional(Eq(MaxQuantity{42.42})));
}

TEST_F(TradingSystemInstrument, FormatDefault) {
  constexpr std::string_view string_representation =
      "Instrument={ InstrumentID=0, DatabaseID=none, Symbol=none, "
      "SecurityType=none, PriceCurrency=none, BaseCurrency=none, "
      "SecurityExchange=none, CUSIP=none, SEDOL=none, ISIN=none, RIC=none, "
      "ExchangeID=none, BloombergID=none, PartyID=none, PartyRole=none, "
      "PriceTick=none, QuantityTick=none, MinQuantity=none, MaxQuantity=none }";

  EXPECT_EQ(fmt::to_string(instrument), string_representation);
}

TEST_F(TradingSystemInstrument, FormatWithSpecifiedAttributes) {
  constexpr std::string_view string_representation =
      "Instrument={ InstrumentID=42, DatabaseID=42, "
      "Symbol=\"AAPL-XLON\", SecurityType=CommonStock, PriceCurrency=\"USD\", "
      "BaseCurrency=\"EUR\", SecurityExchange=\"XLON\", CUSIP=\"CUSIP\", "
      "SEDOL=\"SEDOL\", ISIN=\"ISIN\", RIC=\"RIC\", ExchangeID=\"ExchangeID\", "
      "BloombergID=\"BloombergID\", PartyID=\"ContraFirm\", "
      "PartyRole=ContraFirm, PriceTick=42.42, QuantityTick=42.42, "
      "MinQuantity=42.42, MaxQuantity=42.42 }";

  instrument.identifier = InstrumentId{42};
  instrument.symbol = Symbol{"AAPL-XLON"};
  instrument.security_type = SecurityType::Option::CommonStock;
  instrument.price_currency = PriceCurrency{"USD"};
  instrument.base_currency = BaseCurrency{"EUR"};
  instrument.security_exchange = SecurityExchange{"XLON"};
  instrument.party_id = PartyId{"ContraFirm"};
  instrument.party_role = PartyRole::Option::ContraFirm;
  instrument.cusip = CusipId{"CUSIP"};
  instrument.sedol = SedolId{"SEDOL"};
  instrument.isin = IsinId{"ISIN"};
  instrument.ric = RicId{"RIC"};
  instrument.exchange_id = ExchangeId{"ExchangeID"};
  instrument.bloomberg_id = BloombergId{"BloombergID"};
  instrument.database_id = DatabaseId{42};
  instrument.price_tick = PriceTick{42.42};
  instrument.quantity_tick = QuantityTick{42.42};
  instrument.min_quantity = MinQuantity{42.42};
  instrument.max_quantity = MaxQuantity{42.42};

  EXPECT_EQ(fmt::to_string(instrument), string_representation);
}

TEST_F(TradingSystemInstrument, WritesDefaultToJson) {
  rapidjson::Document doc;
  rapidjson::Value json_value;
  simulator::core::json::Type<Instrument>::write_json_value(
      json_value, doc.GetAllocator(), instrument);

  ASSERT_THAT(json_value, HasNull("symbol"));
  ASSERT_THAT(json_value, HasNull("security_type"));
  ASSERT_THAT(json_value, HasNull("price_currency"));
  ASSERT_THAT(json_value, HasNull("base_currency"));
  ASSERT_THAT(json_value, HasNull("security_exchange"));
  ASSERT_THAT(json_value, HasNull("party_id"));
  ASSERT_THAT(json_value, HasNull("party_role"));
  ASSERT_THAT(json_value, HasNull("cusip"));
  ASSERT_THAT(json_value, HasNull("sedol"));
  ASSERT_THAT(json_value, HasNull("isin"));
  ASSERT_THAT(json_value, HasNull("ric"));
  ASSERT_THAT(json_value, HasNull("exchange_id"));
  ASSERT_THAT(json_value, HasNull("bloomberg_id"));
  ASSERT_THAT(json_value, HasNull("price_tick"));
  ASSERT_THAT(json_value, HasNull("quantity_tick"));
  ASSERT_THAT(json_value, HasNull("min_quantity"));
  ASSERT_THAT(json_value, HasNull("max_quantity"));
}

TEST_F(TradingSystemInstrument, WritesToJson) {
  instrument = instrument_sample();

  rapidjson::Document doc;
  rapidjson::Value json_value;
  simulator::core::json::Type<Instrument>::write_json_value(
      json_value, doc.GetAllocator(), instrument);

  ASSERT_TRUE(json_value.IsObject());
  ASSERT_THAT(json_value, HasString("symbol", "AAPL-XLON"));
  ASSERT_THAT(json_value, HasString("security_type", "CommonStock"));
  ASSERT_THAT(json_value, HasString("price_currency", "USD"));
  ASSERT_THAT(json_value, HasString("base_currency", "EUR"));
  ASSERT_THAT(json_value, HasString("security_exchange", "XLON"));
  ASSERT_THAT(json_value, HasString("party_id", "ContraFirm"));
  ASSERT_THAT(json_value, HasString("party_role", "ContraFirm"));
  ASSERT_THAT(json_value, HasString("cusip", "CUSIP"));
  ASSERT_THAT(json_value, HasString("sedol", "SEDOL"));
  ASSERT_THAT(json_value, HasString("isin", "ISIN"));
  ASSERT_THAT(json_value, HasString("ric", "RIC"));
  ASSERT_THAT(json_value, HasString("exchange_id", "ExchangeID"));
  ASSERT_THAT(json_value, HasString("bloomberg_id", "BloombergID"));
  ASSERT_THAT(json_value, HasDouble("price_tick", 42.42));
  ASSERT_THAT(json_value, HasDouble("quantity_tick", 42.42));
  ASSERT_THAT(json_value, HasDouble("min_quantity", 42.42));
  ASSERT_THAT(json_value, HasDouble("max_quantity", 42.42));
}

TEST_F(TradingSystemInstrument, ReadsDefaultFromJson) {
  rapidjson::Document doc;
  doc.Parse(R"({
    "symbol": null,
    "security_type": null,
    "price_currency": null,
    "base_currency": null,
    "security_exchange": null,
    "party_id": null,
    "party_role": null,
    "cusip": null,
    "sedol": null,
    "isin": null,
    "ric": null,
    "exchange_id": null,
    "bloomberg_id": null,
    "price_tick": null,
    "quantity_tick": null,
    "min_quantity": null,
    "max_quantity": null
  })");

  instrument = core::json::Type<Instrument>::read_json_value(doc);

  ASSERT_EQ(instrument.symbol, std::nullopt);
  ASSERT_EQ(instrument.security_type, std::nullopt);
  ASSERT_EQ(instrument.price_currency, std::nullopt);
  ASSERT_EQ(instrument.base_currency, std::nullopt);
  ASSERT_EQ(instrument.security_exchange, std::nullopt);
  ASSERT_EQ(instrument.party_id, std::nullopt);
  ASSERT_EQ(instrument.party_role, std::nullopt);
  ASSERT_EQ(instrument.cusip, std::nullopt);
  ASSERT_EQ(instrument.sedol, std::nullopt);
  ASSERT_EQ(instrument.isin, std::nullopt);
  ASSERT_EQ(instrument.ric, std::nullopt);
  ASSERT_EQ(instrument.exchange_id, std::nullopt);
  ASSERT_EQ(instrument.bloomberg_id, std::nullopt);
  ASSERT_EQ(instrument.database_id, std::nullopt);
  ASSERT_EQ(instrument.price_tick, std::nullopt);
  ASSERT_EQ(instrument.quantity_tick, std::nullopt);
  ASSERT_EQ(instrument.min_quantity, std::nullopt);
  ASSERT_EQ(instrument.max_quantity, std::nullopt);
}

TEST_F(TradingSystemInstrument, ReadsFromJson) {
  rapidjson::Document doc;
  doc.Parse(R"({
    "symbol": "AAPL-XLON",
    "security_type": "CommonStock",
    "price_currency": "USD",
    "base_currency": "EUR",
    "security_exchange": "XLON",
    "party_id": "ContraFirm",
    "party_role": "ContraFirm",
    "cusip": "CUSIP",
    "sedol": "SEDOL",
    "isin": "ISIN",
    "ric": "RIC",
    "exchange_id": "ExchangeID",
    "bloomberg_id": "BloombergID",
    "price_tick": 42.42,
    "quantity_tick": 42.42,
    "min_quantity": 42.42,
    "max_quantity": 42.42
  })");

  instrument = core::json::Type<Instrument>::read_json_value(doc);

  ASSERT_THAT(instrument.symbol, Optional(Eq(Symbol{"AAPL-XLON"})));
  ASSERT_THAT(instrument.security_type,
              Optional(Eq(SecurityType::Option::CommonStock)));
  ASSERT_THAT(instrument.price_currency, Optional(Eq(Currency{"USD"})));
  ASSERT_THAT(instrument.base_currency, Optional(Eq(Currency{"EUR"})));
  ASSERT_THAT(instrument.security_exchange,
              Optional(Eq(SecurityExchange{"XLON"})));
  ASSERT_THAT(instrument.party_id, Optional(Eq(PartyId{"ContraFirm"})));
  ASSERT_THAT(instrument.party_role,
              Optional(Eq(PartyRole::Option::ContraFirm)));
  ASSERT_THAT(instrument.cusip, Optional(Eq(SecurityId{"CUSIP"})));
  ASSERT_THAT(instrument.sedol, Optional(Eq(SecurityId{"SEDOL"})));
  ASSERT_THAT(instrument.isin, Optional(Eq(SecurityId{"ISIN"})));
  ASSERT_THAT(instrument.ric, Optional(Eq(SecurityId{"RIC"})));
  ASSERT_THAT(instrument.exchange_id, Optional(Eq(SecurityId{"ExchangeID"})));
  ASSERT_THAT(instrument.bloomberg_id, Optional(Eq(SecurityId{"BloombergID"})));
  ASSERT_THAT(instrument.price_tick, Optional(Eq(PriceTick{42.42})));
  ASSERT_THAT(instrument.quantity_tick, Optional(Eq(QuantityTick{42.42})));
  ASSERT_THAT(instrument.min_quantity, Optional(Eq(MinQuantity{42.42})));
  ASSERT_THAT(instrument.max_quantity, Optional(Eq(MaxQuantity{42.42})));
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::trading_system::test
