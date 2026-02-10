#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "api/inspectors/listing.hpp"
#include "api/models/listing.hpp"
#include "common/marshaller.hpp"
#include "tests/test_utils/matchers.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace testing;

// NOLINTBEGIN(*magic-numbers*)

class DataLayerInspectorsListingReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<Listing>;
  using ReaderType = ListingReader<MarshallerType>;

  using Attribute = Listing::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  static auto make_default_patch() -> Listing::Patch {
    return Listing::Patch{}.with_venue_id("dummy");
  }

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), uint64(Eq(Attribute::ListingId), _))
        .Times(AnyNumber());

    EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), _))
        .Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerInspectorsListingReader, ReadsListingID) {
  const auto patch = make_default_patch();
  const auto listing = Listing::create(patch, 42);
  ASSERT_EQ(listing.listing_id(), 42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::ListingId), Eq(42))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsSymbol) {
  const auto patch = make_default_patch().with_symbol("AAPL");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Symbol), Eq("AAPL"))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsVenueID) {
  const auto patch = make_default_patch().with_venue_id("NASDAQ");
  const auto listing = Listing::create(patch, 42);
  ASSERT_EQ(listing.venue_id(), "NASDAQ");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("NASDAQ")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsSecurityType) {
  const auto patch = make_default_patch().with_security_type("Equity");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SecurityType), Eq("Equity")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsPriceCurrency) {
  const auto patch = make_default_patch().with_price_currency("USD");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PriceCurrency), Eq("USD")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsFxBaseCurrency) {
  const auto patch = make_default_patch().with_fx_base_currency("USD");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::FxBaseCurrency), Eq("USD")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsInstrSymbol) {
  const auto patch = make_default_patch().with_instr_symbol("AAPL");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::InstrSymbol), Eq("AAPL")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsQtyMinimum) {
  const auto patch = make_default_patch().with_qty_minimum(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), real(Eq(Attribute::QtyMinimum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsQtyMaximum) {
  const auto patch = make_default_patch().with_qty_maximum(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), real(Eq(Attribute::QtyMaximum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsQtyMultiple) {
  const auto patch = make_default_patch().with_qty_multiple(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), real(Eq(Attribute::QtyMultiple), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsPriceTickSize) {
  const auto patch = make_default_patch().with_price_tick_size(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), real(Eq(Attribute::PriceTickSize), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsEnabled) {
  const auto patch = make_default_patch().with_enabled_flag(true);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::Enabled), Eq(true))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomQtyMaximum) {
  const auto patch = make_default_patch().with_random_qty_maximum(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(),
              real(Eq(Attribute::RandomQtyMaximum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomDepthLevels) {
  const auto patch = make_default_patch().with_random_depth_levels(42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), uint32(Eq(Attribute::RandomDepthLevels), Eq(42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomOrdersSpread) {
  const auto patch = make_default_patch().with_random_orders_spread(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(),
              real(Eq(Attribute::RandomOrdersSpread), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomOrdersRate) {
  const auto patch = make_default_patch().with_random_orders_rate(42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), uint32(Eq(Attribute::RandomOrdersRate), Eq(42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomTickRange) {
  const auto patch = make_default_patch().with_random_tick_range(42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), uint32(Eq(Attribute::RandomTickRange), Eq(42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsSecurityExchange) {
  const auto patch = make_default_patch().with_security_exchange("NASDAQ");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(),
              string(Eq(Attribute::SecurityExchange), Eq("NASDAQ")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsPartyID) {
  const auto patch = make_default_patch().with_party_id("GGV");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PartyId), Eq("GGV"))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsPartyRole) {
  const auto patch = make_default_patch().with_party_role("Role");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PartyRole), Eq("Role")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsCusipID) {
  const auto patch = make_default_patch().with_cusip_id("Cusip");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::CusipId), Eq("Cusip")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsSedolID) {
  const auto patch = make_default_patch().with_sedol_id("Sedol");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SedolId), Eq("Sedol")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsIsinID) {
  const auto patch = make_default_patch().with_isin_id("Isin");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::IsinId), Eq("Isin"))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRicID) {
  const auto patch = make_default_patch().with_ric_id("Ric");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::RicId), Eq("Ric"))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsExchangeSymbolID) {
  const auto patch = make_default_patch().with_exchange_symbol_id("EX");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::ExchangeSymbolId), Eq("EX")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsBloombergSymbolID) {
  const auto patch = make_default_patch().with_bloomberg_symbol_id("B");
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::BloombergSymbolId), Eq("B")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomQtyMinimum) {
  const auto patch = make_default_patch().with_random_qty_minimum(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(),
              real(Eq(Attribute::RandomQtyMinimum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomAmtMinimum) {
  const auto patch = make_default_patch().with_random_amt_minimum(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(),
              real(Eq(Attribute::RandomAmtMinimum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomAmtMaximum) {
  const auto patch = make_default_patch().with_random_amt_maximum(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(),
              real(Eq(Attribute::RandomAmtMaximum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomOrdersEnabled) {
  const auto patch = make_default_patch().with_random_orders_enabled_flag(true);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(),
              boolean(Eq(Attribute::RandomOrdersEnabled), Eq(true)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomAggressiveQtyMinimum) {
  const auto patch =
      make_default_patch().with_random_aggressive_qty_minimum(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(),
              real(Eq(Attribute::RandomAggressiveQtyMinimum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomAggressiveQtyMaximum) {
  const auto patch =
      make_default_patch().with_random_aggressive_qty_maximum(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(),
              real(Eq(Attribute::RandomAggressiveQtyMaximum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomAggressiveAmtMinimum) {
  const auto patch =
      make_default_patch().with_random_aggressive_amt_minimum(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(),
              real(Eq(Attribute::RandomAggressiveAmtMinimum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsListingReader, ReadsRandomAggressiveAmtMaximum) {
  const auto patch =
      make_default_patch().with_random_aggressive_amt_maximum(42.42);
  const auto listing = Listing::create(patch, 42);

  EXPECT_CALL(marshaller(),
              real(Eq(Attribute::RandomAggressiveAmtMaximum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

class DataLayerInspectorsListingPatchReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<Listing>;
  using ReaderType = ListingPatchReader<MarshallerType>;

  using Attribute = Listing::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  Listing::Patch patch;

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerInspectorsListingPatchReader, ReadsSymbol) {
  patch.with_symbol("AAPL");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Symbol), Eq("AAPL"))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsVenueID) {
  patch.with_venue_id("NASDAQ");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("NASDAQ")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsSecurityType) {
  patch.with_security_type("CS");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::SecurityType), Eq("CS")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsPriceCurrency) {
  patch.with_price_currency("USD");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::PriceCurrency), Eq("USD")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsFxBaseCurrency) {
  patch.with_fx_base_currency("USD");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::FxBaseCurrency), Eq("USD")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsInstrSymbol) {
  patch.with_instr_symbol("AAPL");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::InstrSymbol), Eq("AAPL")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsQtyMinimum) {
  patch.with_qty_minimum(42.42);

  EXPECT_CALL(
      marshaller(),
      optional_real(Eq(Attribute::QtyMinimum), Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsQtyMaximum) {
  patch.with_qty_maximum(42.42);

  EXPECT_CALL(
      marshaller(),
      optional_real(Eq(Attribute::QtyMaximum), Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsQtyMultiple) {
  patch.with_qty_multiple(42.42);

  EXPECT_CALL(
      marshaller(),
      optional_real(Eq(Attribute::QtyMultiple), Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsPriceTickSize) {
  patch.with_price_tick_size(42.42);

  EXPECT_CALL(
      marshaller(),
      optional_real(Eq(Attribute::PriceTickSize), Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, SetsEnabledFlagToTrueIfItIsNull) {
  patch.with_enabled_flag(std::nullopt);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::Enabled), Eq(true))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsEnabled) {
  patch.with_enabled_flag(false);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::Enabled), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomQtyMaximum) {
  patch.with_random_qty_maximum(42.42);

  EXPECT_CALL(
      marshaller(),
      optional_real(Eq(Attribute::RandomQtyMaximum), Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomDepthLevels) {
  patch.with_random_depth_levels(42);

  EXPECT_CALL(marshaller(),
              optional_uint32(Eq(Attribute::RandomDepthLevels), Eq(42)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomOrdersSpread) {
  patch.with_random_orders_spread(42.42);

  EXPECT_CALL(marshaller(),
              optional_real(Eq(Attribute::RandomOrdersSpread),
                            Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomOrdersRate) {
  patch.with_random_orders_rate(42);

  EXPECT_CALL(marshaller(),
              optional_uint32(Eq(Attribute::RandomOrdersRate), Eq(42)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomTickRange) {
  patch.with_random_tick_range(42);

  EXPECT_CALL(marshaller(),
              optional_uint32(Eq(Attribute::RandomTickRange), Eq(42)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsSecurityExchange) {
  patch.with_security_exchange("NASDAQ");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::SecurityExchange), Eq("NASDAQ")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsPartyID) {
  patch.with_party_id("GGV");

  EXPECT_CALL(marshaller(), optional_string(Eq(Attribute::PartyId), Eq("GGV")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsPartyRole) {
  patch.with_party_role("Role");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::PartyRole), Eq("Role")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsCusipID) {
  patch.with_cusip_id("Cusip");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::CusipId), Eq("Cusip")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsSedolID) {
  patch.with_sedol_id("Sedol");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::SedolId), Eq("Sedol")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsIsinID) {
  patch.with_isin_id("Isin");

  EXPECT_CALL(marshaller(), optional_string(Eq(Attribute::IsinId), Eq("Isin")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRicID) {
  patch.with_ric_id("Ric");

  EXPECT_CALL(marshaller(), optional_string(Eq(Attribute::RicId), Eq("Ric")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsExchangeSymbolID) {
  patch.with_exchange_symbol_id("EX");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::ExchangeSymbolId), Eq("EX")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsBloombergSymbolID) {
  patch.with_bloomberg_symbol_id("B");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::BloombergSymbolId), Eq("B")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomQtyMinimum) {
  patch.with_random_qty_minimum(42.42);

  EXPECT_CALL(
      marshaller(),
      optional_real(Eq(Attribute::RandomQtyMinimum), Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomAmtMinimum) {
  patch.with_random_amt_minimum(42.42);

  EXPECT_CALL(
      marshaller(),
      optional_real(Eq(Attribute::RandomAmtMinimum), Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomAmtMaximum) {
  patch.with_random_amt_maximum(42.42);

  EXPECT_CALL(
      marshaller(),
      optional_real(Eq(Attribute::RandomAmtMaximum), Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader,
       SetsRandomOrdersEnabledToTrueIfItIsNull) {
  patch.with_random_orders_enabled_flag(std::nullopt);

  EXPECT_CALL(marshaller(),
              boolean(Eq(Attribute::RandomOrdersEnabled), Eq(true)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomOrdersEnabled) {
  patch.with_random_orders_enabled_flag(false);

  EXPECT_CALL(marshaller(),
              boolean(Eq(Attribute::RandomOrdersEnabled), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomAggressiveQtyMinimum) {
  patch.with_random_aggressive_qty_minimum(42.42);

  EXPECT_CALL(marshaller(),
              optional_real(Eq(Attribute::RandomAggressiveQtyMinimum),
                            Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomAggressiveQtyMaximum) {
  patch.with_random_aggressive_qty_maximum(42.42);

  EXPECT_CALL(marshaller(),
              optional_real(Eq(Attribute::RandomAggressiveQtyMaximum),
                            Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomAggressiveAmtMinimum) {
  patch.with_random_aggressive_amt_minimum(42.42);

  EXPECT_CALL(marshaller(),
              optional_real(Eq(Attribute::RandomAggressiveAmtMinimum),
                            Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsListingPatchReader, ReadsRandomAggressiveAmtMaximum) {
  patch.with_random_aggressive_amt_maximum(42.42);

  EXPECT_CALL(marshaller(),
              optional_real(Eq(Attribute::RandomAggressiveAmtMaximum),
                            Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

class DataLayerInspectorsListingPatchWriter : public ::testing::Test {
 public:
  using UnmarshallerType = Unmarshaller<Listing>;
  using WriterType = ListingPatchWriter<UnmarshallerType>;

  using Attribute = Listing::Attribute;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

  Listing::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(unmarshaller(), boolean)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), uint32)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), real)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), optional_boolean)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), optional_uint32)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), optional_real)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), optional_string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayerInspectorsListingPatchWriter, WritesSymbol) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::Symbol), _))
      .WillOnce(DoAll(SetArgReferee<1>("AAPL"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.symbol(), Optional(Eq("AAPL")));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesVenueID) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::VenueId), _))
      .WillOnce(DoAll(SetArgReferee<1>("NASDAQ"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.venue_id(), Optional(Eq("NASDAQ")));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesSecurityTypeNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::SecurityType), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.security_type(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesSecurityType) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::SecurityType), _))
      .WillOnce(DoAll(SetArgReferee<1>("CS"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.security_type(), IsPatchFieldWithValue(Optional(Eq("CS"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesPriceCurrencyNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::PriceCurrency), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.price_currency(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesPriceCurrency) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::PriceCurrency), _))
      .WillOnce(DoAll(SetArgReferee<1>("USD"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.price_currency(),
              IsPatchFieldWithValue(Optional(Eq("USD"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesFxBaseCurrencyNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::FxBaseCurrency), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.fx_base_currency(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesFxBaseCurrency) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::FxBaseCurrency), _))
      .WillOnce(DoAll(SetArgReferee<1>("USD"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.fx_base_currency(),
              IsPatchFieldWithValue(Optional(Eq("USD"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesInstrSymbolNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::InstrSymbol), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.instr_symbol(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesInstrSymbol) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::InstrSymbol), _))
      .WillOnce(DoAll(SetArgReferee<1>("AAPL"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.instr_symbol(),
              IsPatchFieldWithValue(Optional(Eq("AAPL"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesQtyMinimumNull) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::QtyMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.qty_minimum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesQtyMinimum) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::QtyMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.qty_minimum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesQtyMaximumNull) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::QtyMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.qty_maximum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesQtyMaximum) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::QtyMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.qty_maximum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesQtyMultipleNull) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::QtyMultiple), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.qty_multiple(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesQtyMultiple) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::QtyMultiple), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.qty_multiple(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesPriceTickSizeNull) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::PriceTickSize), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.price_tick_size(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesPriceTickSize) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::PriceTickSize), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.price_tick_size(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesEnabledNull) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::Enabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.enabled_flag(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesEnabled) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::Enabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.enabled_flag(), IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomQtyMaximumNull) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::RandomQtyMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_qty_maximum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomQtyMaximum) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::RandomQtyMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_qty_maximum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomDepthLevelsNull) {
  EXPECT_CALL(unmarshaller(),
              optional_uint32(Eq(Attribute::RandomDepthLevels), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_depth_levels(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomDepthLevels) {
  EXPECT_CALL(unmarshaller(),
              optional_uint32(Eq(Attribute::RandomDepthLevels), _))
      .WillOnce(DoAll(SetArgReferee<1>(42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_depth_levels(),
              IsPatchFieldWithValue(Optional(Eq(42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomOrdersSpreadNull) {
  EXPECT_CALL(unmarshaller(),
              optional_real(Eq(Attribute::RandomOrdersSpread), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_orders_spread(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomOrdersSpread) {
  EXPECT_CALL(unmarshaller(),
              optional_real(Eq(Attribute::RandomOrdersSpread), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_orders_spread(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomOrdersRateNull) {
  EXPECT_CALL(unmarshaller(),
              optional_uint32(Eq(Attribute::RandomOrdersRate), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_orders_rate(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomOrdersRate) {
  EXPECT_CALL(unmarshaller(),
              optional_uint32(Eq(Attribute::RandomOrdersRate), _))
      .WillOnce(DoAll(SetArgReferee<1>(42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_orders_rate(),
              IsPatchFieldWithValue(Optional(Eq(42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomTickRangeNull) {
  EXPECT_CALL(unmarshaller(),
              optional_uint32(Eq(Attribute::RandomTickRange), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_tick_range(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomTickRange) {
  EXPECT_CALL(unmarshaller(),
              optional_uint32(Eq(Attribute::RandomTickRange), _))
      .WillOnce(DoAll(SetArgReferee<1>(42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_tick_range(),
              IsPatchFieldWithValue(Optional(Eq(42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesSecurityExchangeNull) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::SecurityExchange), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.security_exchange(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesSecurityExchange) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::SecurityExchange), _))
      .WillOnce(DoAll(SetArgReferee<1>("NASDAQ"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.security_exchange(),
              IsPatchFieldWithValue(Optional(Eq("NASDAQ"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesPartyIDNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::PartyId), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.party_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesPartyID) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::PartyId), _))
      .WillOnce(DoAll(SetArgReferee<1>("P1"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.party_id(), IsPatchFieldWithValue(Optional(Eq("P1"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesPartyRoleNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::PartyRole), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.party_role(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesPartyRole) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::PartyRole), _))
      .WillOnce(DoAll(SetArgReferee<1>("Role"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.party_role(), IsPatchFieldWithValue(Optional(Eq("Role"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesCusipIDNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::CusipId), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.cusip_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesCusipID) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::CusipId), _))
      .WillOnce(DoAll(SetArgReferee<1>("Cusip"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.cusip_id(), IsPatchFieldWithValue(Optional(Eq("Cusip"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesSedolIDNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::SedolId), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.sedol_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesSedolID) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::SedolId), _))
      .WillOnce(DoAll(SetArgReferee<1>("Sedol"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.sedol_id(), IsPatchFieldWithValue(Optional(Eq("Sedol"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesIsinIDNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::IsinId), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.isin_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesIsinID) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::IsinId), _))
      .WillOnce(DoAll(SetArgReferee<1>("Isin"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.isin_id(), IsPatchFieldWithValue(Optional(Eq("Isin"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRicIDNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::RicId), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.ric_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRicID) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::RicId), _))
      .WillOnce(DoAll(SetArgReferee<1>("Ric"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.ric_id(), IsPatchFieldWithValue(Optional(Eq("Ric"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesExchangeSymbolIDNull) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::ExchangeSymbolId), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.exchange_symbol_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesExchangeSymbolID) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::ExchangeSymbolId), _))
      .WillOnce(DoAll(SetArgReferee<1>("EXC"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.exchange_symbol_id(),
              IsPatchFieldWithValue(Optional(Eq("EXC"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesBloombergSymbolIDNull) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::BloombergSymbolId), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.bloomberg_symbol_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesBloombergSymbolID) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::BloombergSymbolId), _))
      .WillOnce(DoAll(SetArgReferee<1>("BBG"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.bloomberg_symbol_id(),
              IsPatchFieldWithValue(Optional(Eq("BBG"))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomQtyMinimumNull) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::RandomQtyMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_qty_minimum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomQtyMinimum) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::RandomQtyMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_qty_minimum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomAmtMinimumNull) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::RandomAmtMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_amt_minimum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomAmtMinimum) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::RandomAmtMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_amt_minimum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomAmtMaximumNull) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::RandomAmtMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_amt_maximum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomAmtMaximum) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::RandomAmtMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_amt_maximum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomOrdersEnabledNull) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::RandomOrdersEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_orders_enabled_flag(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter, WritesRandomOrdersEnabled) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::RandomOrdersEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(false), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_orders_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(false))));
}

TEST_F(DataLayerInspectorsListingPatchWriter,
       WritesRandomAggressiveQtyMinimumNull) {
  EXPECT_CALL(unmarshaller(),
              optional_real(Eq(Attribute::RandomAggressiveQtyMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_qty_minimum(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter,
       WritesRandomAggressiveQtyMinimum) {
  EXPECT_CALL(unmarshaller(),
              optional_real(Eq(Attribute::RandomAggressiveQtyMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_qty_minimum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter,
       WritesRandomAggressiveQtyMaximumNull) {
  EXPECT_CALL(unmarshaller(),
              optional_real(Eq(Attribute::RandomAggressiveQtyMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_qty_maximum(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter,
       WritesRandomAggressiveQtyMaximum) {
  EXPECT_CALL(unmarshaller(),
              optional_real(Eq(Attribute::RandomAggressiveQtyMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_qty_maximum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter,
       WritesRandomAggressiveAmtMinimumNull) {
  EXPECT_CALL(unmarshaller(),
              optional_real(Eq(Attribute::RandomAggressiveAmtMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_amt_minimum(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter,
       WritesRandomAggressiveAmtMinimum) {
  EXPECT_CALL(unmarshaller(),
              optional_real(Eq(Attribute::RandomAggressiveAmtMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_amt_minimum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsListingPatchWriter,
       WritesRandomAggressiveAmtMaximumNull) {
  EXPECT_CALL(unmarshaller(),
              optional_real(Eq(Attribute::RandomAggressiveAmtMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_amt_maximum(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsListingPatchWriter,
       WritesRandomAggressiveAmtMaximum) {
  EXPECT_CALL(unmarshaller(),
              optional_real(Eq(Attribute::RandomAggressiveAmtMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_amt_maximum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::test