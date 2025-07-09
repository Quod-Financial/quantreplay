#include "ih/instruments_cache.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/instrument.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "data_layer/api/models/listing.hpp"
#include "test_utils/utils.hpp"

namespace simulator::trading_system::instrument::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*-magic-numbers)

struct InstrumentsInstrumentsCache : public Test {
  Cache::Implementation cache;
};

TEST_F(InstrumentsInstrumentsCache, AddsInstrument) {
  cache.add_instrument(make_instrument(InstrumentId{9}));

  EXPECT_THAT(cache.container(),
              ElementsAre(Field(&Instrument::identifier, Eq(InstrumentId{1}))));
}

TEST_F(InstrumentsInstrumentsCache, AddsSeveralInstruments) {
  cache.add_instrument(make_instrument(InstrumentId{120}));
  cache.add_instrument(make_instrument(InstrumentId{120}));

  EXPECT_THAT(cache.container(),
              ElementsAre(Field(&Instrument::identifier, Eq(InstrumentId{1})),
                          Field(&Instrument::identifier, Eq(InstrumentId{2}))));
}

TEST_F(InstrumentsInstrumentsCache, RetrieveEmptyCache) {
  const std::vector<Instrument> instruments = cache.retrieve_instruments();

  ASSERT_THAT(instruments, IsEmpty());
}

TEST_F(InstrumentsInstrumentsCache, RetrieveSeveralInstruments) {
  cache.add_instrument(make_instrument(InstrumentId{120}));
  cache.add_instrument(make_instrument(InstrumentId{120}));

  const std::vector<Instrument> instruments = cache.retrieve_instruments();

  ASSERT_THAT(instruments,
              ElementsAre(Field(&Instrument::identifier, Eq(InstrumentId{1})),
                          Field(&Instrument::identifier, Eq(InstrumentId{2}))));
}

struct InstrumentsInstrumentsCacheFind : InstrumentsInstrumentsCache {
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
};

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentBySymbol) {
  Instrument instrument1 = instrument_sample();
  instrument1.symbol = Symbol{"first"};
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.symbol = Symbol{"second"};
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.symbol = Symbol{"second"};
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().symbol, Symbol{"second"});
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentByPriceCurrency) {
  Instrument instrument1 = instrument_sample();
  instrument1.price_currency = PriceCurrency{"first"};
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.price_currency = PriceCurrency{"second"};
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.price_currency = PriceCurrency{"second"};
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().price_currency, PriceCurrency{"second"});
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentByBaseCurrency) {
  Instrument instrument1 = instrument_sample();
  instrument1.base_currency = BaseCurrency{"first"};
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.base_currency = BaseCurrency{"second"};
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.base_currency = BaseCurrency{"second"};
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().base_currency, BaseCurrency{"second"});
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentBySecurityExchange) {
  Instrument instrument1 = instrument_sample();
  instrument1.security_exchange = SecurityExchange{"first"};
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.security_exchange = SecurityExchange{"second"};
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.security_exchange = SecurityExchange{"second"};
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().security_exchange, SecurityExchange{"second"});
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentByPartyId) {
  Instrument instrument1 = instrument_sample();
  instrument1.party_id = PartyId{"first"};
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.party_id = PartyId{"second"};
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.party_id = PartyId{"second"};
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().party_id, PartyId{"second"});
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentByCusip) {
  Instrument instrument1 = instrument_sample();
  instrument1.cusip = CusipId{"first"};
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.cusip = CusipId{"second"};
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.cusip = CusipId{"second"};
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().cusip, CusipId{"second"});
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentBySedol) {
  Instrument instrument1 = instrument_sample();
  instrument1.sedol = SedolId{"first"};
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.sedol = SedolId{"second"};
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.sedol = SedolId{"second"};
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().sedol, SedolId{"second"});
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentByIsin) {
  Instrument instrument1 = instrument_sample();
  instrument1.isin = IsinId{"first"};
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.isin = IsinId{"second"};
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.isin = IsinId{"second"};
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().isin, IsinId{"second"});
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentByRic) {
  Instrument instrument1 = instrument_sample();
  instrument1.ric = RicId{"first"};
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.ric = RicId{"second"};
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.ric = RicId{"second"};
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().ric, RicId{"second"});
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentByExchangeId) {
  Instrument instrument1 = instrument_sample();
  instrument1.exchange_id = ExchangeId{"first"};
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.exchange_id = ExchangeId{"second"};
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.exchange_id = ExchangeId{"second"};
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().exchange_id, ExchangeId{"second"});
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentByBloombergId) {
  Instrument instrument1 = instrument_sample();
  instrument1.bloomberg_id = BloombergId{"first"};
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.bloomberg_id = BloombergId{"second"};
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.bloomberg_id = BloombergId{"second"};
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().bloomberg_id, BloombergId{"second"});
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentByPartyRole) {
  Instrument instrument1 = instrument_sample();
  instrument1.party_role = PartyRole::Option::ContraFirm;
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.party_role = PartyRole::Option::Agent;
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.party_role = PartyRole::Option::Agent;
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().party_role, PartyRole::Option::Agent);
}

TEST_F(InstrumentsInstrumentsCacheFind, FindsInstrumentBySecurityType) {
  Instrument instrument1 = instrument_sample();
  instrument1.security_type = SecurityType::Option::CommonStock;
  cache.add_instrument(instrument1);

  Instrument instrument2 = instrument_sample();
  instrument2.security_type = SecurityType::Option::Future;
  cache.add_instrument(instrument2);

  Instrument instrument3 = instrument_sample();
  instrument3.security_type = SecurityType::Option::Future;
  const auto result = cache.find_instrument(instrument3);

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->instrument().security_type, SecurityType::Option::Future);
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::trading_system::instrument::test