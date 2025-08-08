#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "data_layer/api/models/listing.hpp"
#include "instruments/sources.hpp"

namespace simulator::trading_system::instrument::test {
namespace {

using namespace ::testing;

// NOLINTBEGIN(*-magic-numbers)

struct InstrumentsCreateInstrument : public Test {
  using Listing = data_layer::Listing;

  [[nodiscard]] static auto make_listing_patch() -> Listing::Patch {
    return Listing::Patch{}.with_venue_id("VENUE");
  }
};

TEST_F(InstrumentsCreateInstrument, WithDefaultFields) {
  const Listing listing = Listing::create(make_listing_patch(), 42);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.identifier, InstrumentId{0});
  EXPECT_THAT(instrument.database_id, Optional(Eq(DatabaseId{42})));
}

TEST_F(InstrumentsCreateInstrument, WithSymbol) {
  const Listing listing =
      Listing::create(make_listing_patch().with_symbol("AAPL"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.symbol, Optional(Eq(Symbol{"AAPL"})));
}

TEST_F(InstrumentsCreateInstrument, WithoutSymbol) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.symbol, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithSecurityType) {
  const Listing listing =
      Listing::create(make_listing_patch().with_security_type("CS"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.security_type,
              Optional(Eq(SecurityType::Option::CommonStock)));
}

TEST_F(InstrumentsCreateInstrument, WithUndefinedSecurityType) {
  const Listing listing =
      Listing::create(make_listing_patch().with_security_type("bad-value"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.security_type, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithoutSecurityType) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.security_type, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithPriceCurrency) {
  const Listing listing =
      Listing::create(make_listing_patch().with_price_currency("USD"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.price_currency, Optional(Eq(PriceCurrency{"USD"})));
}

TEST_F(InstrumentsCreateInstrument, WithoutPriceCurrency) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.price_currency, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithBaseCurrency) {
  const Listing listing =
      Listing::create(make_listing_patch().with_fx_base_currency("EUR"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.base_currency, Optional(Eq(BaseCurrency{"EUR"})));
}

TEST_F(InstrumentsCreateInstrument, WithoutBaseCurrency) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.base_currency, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithSecurityExchange) {
  const Listing listing =
      Listing::create(make_listing_patch().with_security_exchange("XLON"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.security_exchange,
              Optional(Eq(SecurityExchange{"XLON"})));
}

TEST_F(InstrumentsCreateInstrument, WithoutSecurityExchange) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.security_exchange, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithPartyId) {
  const Listing listing =
      Listing::create(make_listing_patch().with_party_id("P123"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.party_id, Optional(Eq(PartyId{"P123"})));
}

TEST_F(InstrumentsCreateInstrument, WithoutPartyId) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.party_id, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithPartyRole) {
  const Listing listing = Listing::create(
      make_listing_patch().with_party_role("ExecutingFirm"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.party_role,
              Optional(Eq(PartyRole::Option::ExecutingFirm)));
}

TEST_F(InstrumentsCreateInstrument, WithUndefinedPartyRole) {
  const Listing listing =
      Listing::create(make_listing_patch().with_party_role("bad-value"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.party_role, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithoutPartyRole) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.party_role, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithCusipId) {
  const Listing listing =
      Listing::create(make_listing_patch().with_cusip_id("CUSIP"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.cusip, Optional(Eq(CusipId{"CUSIP"})));
}

TEST_F(InstrumentsCreateInstrument, WithoutCusipId) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.cusip, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithSedolId) {
  const Listing listing =
      Listing::create(make_listing_patch().with_sedol_id("SEDOL"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.sedol, Optional(Eq(SedolId{"SEDOL"})));
}

TEST_F(InstrumentsCreateInstrument, WithoutSedolId) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.sedol, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithIsinId) {
  const Listing listing =
      Listing::create(make_listing_patch().with_isin_id("ISIN"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.isin, Optional(Eq(IsinId{"ISIN"})));
}

TEST_F(InstrumentsCreateInstrument, WithoutIsinId) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.isin, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithRicId) {
  const Listing listing =
      Listing::create(make_listing_patch().with_ric_id("RIC"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.ric, Optional(Eq(RicId{"RIC"})));
}

TEST_F(InstrumentsCreateInstrument, WithoutRicId) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.ric, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithExchangeId) {
  const Listing listing = Listing::create(
      make_listing_patch().with_exchange_symbol_id("EXC-ID"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.exchange_id, Optional(Eq(ExchangeId{"EXC-ID"})));
}

TEST_F(InstrumentsCreateInstrument, WithoutExchangeId) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.exchange_id, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithBloombergId) {
  const Listing listing = Listing::create(
      make_listing_patch().with_bloomberg_symbol_id("BBG-ID"), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.bloomberg_id, Optional(Eq(BloombergId{"BBG-ID"})));
}

TEST_F(InstrumentsCreateInstrument, WithoutBloombergId) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.bloomberg_id, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithPriceTick) {
  const Listing listing =
      Listing::create(make_listing_patch().with_price_tick_size(42.42), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.price_tick, Optional(Eq(PriceTick{42.42})));
}

TEST_F(InstrumentsCreateInstrument, WithoutPriceTick) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.price_tick, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithQuantityTick) {
  const Listing listing =
      Listing::create(make_listing_patch().with_qty_multiple(42.42), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.quantity_tick, Optional(Eq(QuantityTick{42.42})));
}

TEST_F(InstrumentsCreateInstrument, WithoutQuantityTick) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.quantity_tick, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithMinimalQuantity) {
  const Listing listing =
      Listing::create(make_listing_patch().with_qty_minimum(42.42), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.min_quantity, Optional(Eq(MinQuantity{42.42})));
}

TEST_F(InstrumentsCreateInstrument, WithoutMinimalQuantity) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.min_quantity, std::nullopt);
}

TEST_F(InstrumentsCreateInstrument, WithMaximalQuantity) {
  const Listing listing =
      Listing::create(make_listing_patch().with_qty_maximum(42.42), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_THAT(instrument.max_quantity, Optional(Eq(MaxQuantity{42.42})));
}

TEST_F(InstrumentsCreateInstrument, WithoutMaximalQuantity) {
  const Listing listing = Listing::create(make_listing_patch(), 43);

  const Instrument instrument = detail::create_instrument(listing);

  EXPECT_EQ(instrument.max_quantity, std::nullopt);
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::trading_system::instrument::test