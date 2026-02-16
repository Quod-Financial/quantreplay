#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>

#include "common/instrument.hpp"
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

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::trading_system::test
