#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "api/inspectors/listing.hpp"
#include "api/models/listing.hpp"
#include "common/marshaller.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace testing;

// NOLINTBEGIN(*magic-numbers*)

class DataLayer_Inspectors_ListingReader : public ::testing::Test {
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

class DataLayer_Inspectors_ListingPatchReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<Listing>;
  using ReaderType = ListingPatchReader<MarshallerType>;

  using Attribute = Listing::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

 private:
  MarshallerType marshaller_;
};

class DataLayer_Inspectors_ListingPatchWriter : public ::testing::Test {
 public:
  using UnmarshallerType = Unmarshaller<Listing>;
  using WriterType = ListingPatchWriter<UnmarshallerType>;

  using Attribute = Listing::Attribute;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

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
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayer_Inspectors_ListingReader, Read_ListingID) {
  const auto patch = make_default_patch();
  const auto listing = Listing::create(patch, 42);
  ASSERT_EQ(listing.listing_id(), 42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::ListingId), Eq(42))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_Symbol) {
  const auto patch = make_default_patch().with_symbol("AAPL");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.symbol(), Optional(Eq("AAPL")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Symbol), Eq("AAPL"))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_VenueID) {
  const auto patch = make_default_patch().with_venue_id("NASDAQ");
  const auto listing = Listing::create(patch, 42);
  ASSERT_EQ(listing.venue_id(), "NASDAQ");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("NASDAQ")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_SecurityType) {
  const auto patch = make_default_patch().with_security_type("Equity");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.security_type(), Optional(Eq("Equity")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SecurityType), Eq("Equity")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_PriceCurrency) {
  const auto patch = make_default_patch().with_price_currency("USD");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.price_currency(), Optional(Eq("USD")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PriceCurrency), Eq("USD")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_FxBaseCurrency) {
  const auto patch = make_default_patch().with_fx_base_currency("USD");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.fx_base_currency(), Optional(Eq("USD")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::FxBaseCurrency), Eq("USD")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_InstrSymbol) {
  const auto patch = make_default_patch().with_instr_symbol("AAPL");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.instr_symbol(), Optional(Eq("AAPL")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::InstrSymbol), Eq("AAPL")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_QtyMinimum) {
  const auto patch = make_default_patch().with_qty_minimum(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.qty_minimum(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::QtyMinimum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_QtyMaximum) {
  const auto patch = make_default_patch().with_qty_maximum(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.qty_maximum(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::QtyMaximum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_QtyMultiple) {
  const auto patch = make_default_patch().with_qty_multiple(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.qty_multiple(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::QtyMultiple), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_PriceTickSize) {
  const auto patch = make_default_patch().with_price_tick_size(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.price_tick_size(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::PriceTickSize), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_Enabled) {
  const auto patch = make_default_patch().with_enabled_flag(true);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.enabled_flag(), Optional(Eq(true)));

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::Enabled), Eq(true))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomQtyMaximum) {
  const auto patch = make_default_patch().with_random_qty_maximum(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_qty_maximum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::RandomQtyMaximum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomDepthLevels) {
  const auto patch = make_default_patch().with_random_depth_levels(42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_depth_levels(), Optional(Eq(42)));

  auto& expect = EXPECT_CALL(marshaller(),
                             uint32(Eq(Attribute::RandomDepthLevels), Eq(42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomOrdersSpread) {
  const auto patch = make_default_patch().with_random_orders_spread(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_orders_spread(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::RandomOrdersSpread), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomOrdersRate) {
  const auto patch = make_default_patch().with_random_orders_rate(42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_orders_rate(), Optional(Eq(42)));

  auto& expect = EXPECT_CALL(marshaller(),
                             uint32(Eq(Attribute::RandomOrdersRate), Eq(42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomTickRange) {
  const auto patch = make_default_patch().with_random_tick_range(42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_tick_range(), Optional(Eq(42)));

  auto& expect =
      EXPECT_CALL(marshaller(), uint32(Eq(Attribute::RandomTickRange), Eq(42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_SecurityExchange) {
  const auto patch = make_default_patch().with_security_exchange("NASDAQ");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.security_exchange(), Optional(Eq("NASDAQ")));

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::SecurityExchange), Eq("NASDAQ")));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_PartyID) {
  const auto patch = make_default_patch().with_party_id("GGV");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.party_id(), Optional(Eq("GGV")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PartyId), Eq("GGV"))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_PartyRole) {
  const auto patch = make_default_patch().with_party_role("Role");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.party_role(), Optional(Eq("Role")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PartyRole), Eq("Role")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_CusipID) {
  const auto patch = make_default_patch().with_cusip_id("Cusip");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.cusip_id(), Optional(Eq("Cusip")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::CusipId), Eq("Cusip")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_SedolID) {
  const auto patch = make_default_patch().with_sedol_id("Sedol");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.sedol_id(), Optional(Eq("Sedol")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SedolId), Eq("Sedol")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_IsinID) {
  const auto patch = make_default_patch().with_isin_id("Isin");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.isin_id(), Optional(Eq("Isin")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::IsinId), Eq("Isin"))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RicID) {
  const auto patch = make_default_patch().with_ric_id("Ric");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.ric_id(), Optional(Eq("Ric")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::RicId), Eq("Ric"))).Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_ExchangeSymbolID) {
  const auto patch = make_default_patch().with_exchange_symbol_id("EX");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.exchange_symbol_id(), Optional(Eq("EX")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::ExchangeSymbolId), Eq("EX")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_BloombergSymbolID) {
  const auto patch = make_default_patch().with_bloomberg_symbol_id("B");
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.bloomberg_symbol_id(), Optional(Eq("B")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::BloombergSymbolId), Eq("B")))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomQtyMinimum) {
  const auto patch = make_default_patch().with_random_qty_minimum(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_qty_minimum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::RandomQtyMinimum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomAmtMinimum) {
  const auto patch = make_default_patch().with_random_amt_minimum(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_amt_minimum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::RandomAmtMinimum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomAmtMaximum) {
  const auto patch = make_default_patch().with_random_amt_maximum(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_amt_maximum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::RandomAmtMaximum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomOrdersEnabled) {
  const auto patch = make_default_patch().with_random_orders_enabled_flag(true);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_orders_enabled_flag(), Optional(Eq(true)));

  auto& expect = EXPECT_CALL(
      marshaller(), boolean(Eq(Attribute::RandomOrdersEnabled), Eq(true)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomAggressiveQtyMinimum) {
  const auto patch =
      make_default_patch().with_random_aggressive_qty_minimum(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_aggressive_qty_minimum(),
              Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(),
      real(Eq(Attribute::RandomAggressiveQtyMinimum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomAggressiveQtyMaximum) {
  const auto patch =
      make_default_patch().with_random_aggressive_qty_maximum(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_aggressive_qty_maximum(),
              Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(),
      real(Eq(Attribute::RandomAggressiveQtyMaximum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomAggressiveAmtMinimum) {
  const auto patch =
      make_default_patch().with_random_aggressive_amt_minimum(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_aggressive_amt_minimum(),
              Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(),
      real(Eq(Attribute::RandomAggressiveAmtMinimum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingReader, Read_RandomAggressiveAmtMaximum) {
  const auto patch =
      make_default_patch().with_random_aggressive_amt_maximum(42.42);
  const auto listing = Listing::create(patch, 42);
  ASSERT_THAT(listing.random_aggressive_amt_maximum(),
              Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(),
      real(Eq(Attribute::RandomAggressiveAmtMaximum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_Symbol) {
  Listing::Patch patch{};
  patch.with_symbol("AAPL");
  ASSERT_THAT(patch.symbol(), Optional(Eq("AAPL")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Symbol), Eq("AAPL"))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_VenueID) {
  Listing::Patch patch{};
  patch.with_venue_id("NASDAQ");
  ASSERT_THAT(patch.venue_id(), Optional(Eq("NASDAQ")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("NASDAQ")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_SecurityType) {
  Listing::Patch patch{};
  patch.with_security_type("CS");
  ASSERT_THAT(patch.security_type(), Optional(Eq("CS")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SecurityType), Eq("CS")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_PriceCurrency) {
  Listing::Patch patch{};
  patch.with_price_currency("USD");
  ASSERT_THAT(patch.price_currency(), Optional(Eq("USD")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PriceCurrency), Eq("USD")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_FxBaseCurrency) {
  Listing::Patch patch{};
  patch.with_fx_base_currency("USD");
  ASSERT_THAT(patch.fx_base_currency(), Optional(Eq("USD")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::FxBaseCurrency), Eq("USD")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_InstrSymbol) {
  Listing::Patch patch{};
  patch.with_instr_symbol("AAPL");
  ASSERT_THAT(patch.instr_symbol(), Optional(Eq("AAPL")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::InstrSymbol), Eq("AAPL")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_QtyMinimum) {
  Listing::Patch patch{};
  patch.with_qty_minimum(42.42);
  ASSERT_THAT(patch.qty_minimum(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::QtyMinimum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_QtyMaximum) {
  Listing::Patch patch{};
  patch.with_qty_maximum(42.42);
  ASSERT_THAT(patch.qty_maximum(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::QtyMaximum), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_QtyMultiple) {
  Listing::Patch patch{};
  patch.with_qty_multiple(42.42);
  ASSERT_THAT(patch.qty_multiple(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::QtyMultiple), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_PriceTickSize) {
  Listing::Patch patch{};
  patch.with_price_tick_size(42.42);
  ASSERT_THAT(patch.price_tick_size(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::PriceTickSize), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_Enabled) {
  Listing::Patch patch{};
  patch.with_enabled_flag(true);
  ASSERT_THAT(patch.enabled_flag(), Optional(Eq(true)));

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::Enabled), Eq(true))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_RandomQtyMaximum) {
  Listing::Patch patch{};
  patch.with_random_qty_maximum(42.42);
  ASSERT_THAT(patch.random_qty_maximum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::RandomQtyMaximum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_RandomDepthLevels) {
  Listing::Patch patch{};
  patch.with_random_depth_levels(42);
  ASSERT_THAT(patch.random_depth_levels(), Optional(Eq(42)));

  auto& expect = EXPECT_CALL(marshaller(),
                             uint32(Eq(Attribute::RandomDepthLevels), Eq(42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_RandomOrdersSpread) {
  Listing::Patch patch{};
  patch.with_random_orders_spread(42.42);
  ASSERT_THAT(patch.random_orders_spread(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::RandomOrdersSpread), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_RandomOrdersRate) {
  Listing::Patch patch{};
  patch.with_random_orders_rate(42);
  ASSERT_THAT(patch.random_orders_rate(), Optional(Eq(42)));

  auto& expect = EXPECT_CALL(marshaller(),
                             uint32(Eq(Attribute::RandomOrdersRate), Eq(42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_RandomTickRange) {
  Listing::Patch patch{};
  patch.with_random_tick_range(42);
  ASSERT_THAT(patch.random_tick_range(), Optional(Eq(42)));

  auto& expect =
      EXPECT_CALL(marshaller(), uint32(Eq(Attribute::RandomTickRange), Eq(42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_SecurityExchange) {
  Listing::Patch patch{};
  patch.with_security_exchange("NASDAQ");
  ASSERT_THAT(patch.security_exchange(), Optional(Eq("NASDAQ")));

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::SecurityExchange), Eq("NASDAQ")));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_PartyID) {
  Listing::Patch patch{};
  patch.with_party_id("GGV");
  ASSERT_THAT(patch.party_id(), Optional(Eq("GGV")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PartyId), Eq("GGV"))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_PartyRole) {
  Listing::Patch patch{};
  patch.with_party_role("Role");
  ASSERT_THAT(patch.party_role(), Optional(Eq("Role")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PartyRole), Eq("Role")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_CusipID) {
  Listing::Patch patch{};
  patch.with_cusip_id("Cusip");
  ASSERT_THAT(patch.cusip_id(), Optional(Eq("Cusip")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::CusipId), Eq("Cusip")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_SedolID) {
  Listing::Patch patch{};
  patch.with_sedol_id("Sedol");
  ASSERT_THAT(patch.sedol_id(), Optional(Eq("Sedol")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SedolId), Eq("Sedol")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_IsinID) {
  Listing::Patch patch{};
  patch.with_isin_id("Isin");
  ASSERT_THAT(patch.isin_id(), Optional(Eq("Isin")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::IsinId), Eq("Isin"))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_RicID) {
  Listing::Patch patch{};
  patch.with_ric_id("Ric");
  ASSERT_THAT(patch.ric_id(), Optional(Eq("Ric")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::RicId), Eq("Ric"))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_ExchangeSymbolID) {
  Listing::Patch patch{};
  patch.with_exchange_symbol_id("EX");
  ASSERT_THAT(patch.exchange_symbol_id(), Optional(Eq("EX")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::ExchangeSymbolId), Eq("EX")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_BloombergSymbolID) {
  Listing::Patch patch{};
  patch.with_bloomberg_symbol_id("B");
  ASSERT_THAT(patch.bloomberg_symbol_id(), Optional(Eq("B")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::BloombergSymbolId), Eq("B")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_RandomQtyMinimum) {
  Listing::Patch patch{};
  patch.with_random_qty_minimum(42.42);
  ASSERT_THAT(patch.random_qty_minimum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::RandomQtyMinimum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_RandomAmtMinimum) {
  Listing::Patch patch{};
  patch.with_random_amt_minimum(42.42);
  ASSERT_THAT(patch.random_amt_minimum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::RandomAmtMinimum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_RandomAmtMaximum) {
  Listing::Patch patch{};
  patch.with_random_amt_maximum(42.42);
  ASSERT_THAT(patch.random_amt_maximum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(), real(Eq(Attribute::RandomAmtMaximum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader, Read_RandomOrdersEnabled) {
  Listing::Patch patch{};
  patch.with_random_orders_enabled_flag(false);
  ASSERT_THAT(patch.random_orders_enabled_flag(), Optional(Eq(false)));

  auto& expect = EXPECT_CALL(
      marshaller(), boolean(Eq(Attribute::RandomOrdersEnabled), Eq(false)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader,
       Read_RandomAggressiveQtyMinimum) {
  Listing::Patch patch;
  patch.with_random_aggressive_qty_minimum(42.42);
  ASSERT_THAT(patch.random_aggressive_qty_minimum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(),
      real(Eq(Attribute::RandomAggressiveQtyMinimum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader,
       Read_RandomAggressiveQtyMaximum) {
  Listing::Patch patch;
  patch.with_random_aggressive_qty_maximum(42.42);
  ASSERT_THAT(patch.random_aggressive_qty_maximum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(),
      real(Eq(Attribute::RandomAggressiveQtyMaximum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader,
       Read_RandomAggressiveAmtMinimum) {
  Listing::Patch patch;
  patch.with_random_aggressive_amt_minimum(42.42);
  ASSERT_THAT(patch.random_aggressive_amt_minimum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(),
      real(Eq(Attribute::RandomAggressiveAmtMinimum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchReader,
       Read_RandomAggressiveAmtMaximum) {
  Listing::Patch patch;
  patch.with_random_aggressive_amt_maximum(42.42);
  ASSERT_THAT(patch.random_aggressive_amt_maximum(), Optional(DoubleEq(42.42)));

  auto& expect = EXPECT_CALL(
      marshaller(),
      real(Eq(Attribute::RandomAggressiveAmtMaximum), DoubleEq(42.42)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_Symbol) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::Symbol), _))
      .WillOnce(DoAll(SetArgReferee<1>("AAPL"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.symbol(), Optional(Eq("AAPL")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_VenueID) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::VenueId), _))
      .WillOnce(DoAll(SetArgReferee<1>("NASDAQ"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.venue_id(), Optional(Eq("NASDAQ")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_SecurityType) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::SecurityType), _))
      .WillOnce(DoAll(SetArgReferee<1>("CS"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.security_type(), Optional(Eq("CS")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_PriceCurrency) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::PriceCurrency), _))
      .WillOnce(DoAll(SetArgReferee<1>("USD"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.price_currency(), Optional(Eq("USD")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_FxBaseCurrency) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::FxBaseCurrency), _))
      .WillOnce(DoAll(SetArgReferee<1>("USD"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.fx_base_currency(), Optional(Eq("USD")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_InstrSymbol) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::InstrSymbol), _))
      .WillOnce(DoAll(SetArgReferee<1>("AAPL"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.instr_symbol(), Optional(Eq("AAPL")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_QtyMinimum) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), real(Eq(Attribute::QtyMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.qty_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_QtyMaximum) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), real(Eq(Attribute::QtyMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.qty_maximum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_PriceTickSize) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), real(Eq(Attribute::PriceTickSize), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.price_tick_size(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_Enabled) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::Enabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.enabled_flag(), Optional(Eq(true)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_RandomQtyMaximum) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), real(Eq(Attribute::RandomQtyMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_qty_maximum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_RandomDepthLevels) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), uint32(Eq(Attribute::RandomDepthLevels), _))
      .WillOnce(DoAll(SetArgReferee<1>(42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_depth_levels(), Optional(Eq(42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_RandomOrdersSpread) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), real(Eq(Attribute::RandomOrdersSpread), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_orders_spread(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_RandomOrdersRate) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), uint32(Eq(Attribute::RandomOrdersRate), _))
      .WillOnce(DoAll(SetArgReferee<1>(42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_orders_rate(), Optional(Eq(42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_RandomTickRange) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), uint32(Eq(Attribute::RandomTickRange), _))
      .WillOnce(DoAll(SetArgReferee<1>(42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_tick_range(), Optional(Eq(42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_SecurityExchange) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::SecurityExchange), _))
      .WillOnce(DoAll(SetArgReferee<1>("NASDAQ"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.security_exchange(), Optional(Eq("NASDAQ")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_PartyID) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::PartyId), _))
      .WillOnce(DoAll(SetArgReferee<1>("P1"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.party_id(), Optional(Eq("P1")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_PartyRole) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::PartyRole), _))
      .WillOnce(DoAll(SetArgReferee<1>("Role"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.party_role(), Optional(Eq("Role")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_CusipID) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::CusipId), _))
      .WillOnce(DoAll(SetArgReferee<1>("Cusip"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.cusip_id(), Optional(Eq("Cusip")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_SedolID) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::SedolId), _))
      .WillOnce(DoAll(SetArgReferee<1>("Sedol"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.sedol_id(), Optional(Eq("Sedol")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_IsinID) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::IsinId), _))
      .WillOnce(DoAll(SetArgReferee<1>("Isin"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.isin_id(), Optional(Eq("Isin")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_RicID) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::RicId), _))
      .WillOnce(DoAll(SetArgReferee<1>("Ric"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.ric_id(), Optional(Eq("Ric")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_ExchangeSymbolID) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::ExchangeSymbolId), _))
      .WillOnce(DoAll(SetArgReferee<1>("EXC"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.exchange_symbol_id(), Optional(Eq("EXC")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_BloombergSymbolID) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::BloombergSymbolId), _))
      .WillOnce(DoAll(SetArgReferee<1>("BBG"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.bloomberg_symbol_id(), Optional(Eq("BBG")));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_RandomQtyMinimum) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), real(Eq(Attribute::RandomQtyMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_qty_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_RandomAmtMinimum) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), real(Eq(Attribute::RandomAmtMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_amt_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_RandomAmtMaximum) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), real(Eq(Attribute::RandomAmtMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_amt_maximum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter, Write_RandomOrdersEnabled) {
  Listing::Patch patch{};
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::RandomOrdersEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(false), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_orders_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter,
       Write_RandomAggressiveQtyMinimum) {
  Listing::Patch patch;
  EXPECT_CALL(unmarshaller(),
              real(Eq(Attribute::RandomAggressiveQtyMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_qty_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter,
       Write_RandomAggressiveQtyMaximum) {
  Listing::Patch patch;
  EXPECT_CALL(unmarshaller(),
              real(Eq(Attribute::RandomAggressiveQtyMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_qty_maximum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter,
       Write_RandomAggressiveAmtMinimum) {
  Listing::Patch patch;
  EXPECT_CALL(unmarshaller(),
              real(Eq(Attribute::RandomAggressiveAmtMinimum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_amt_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_ListingPatchWriter,
       Write_RandomAggressiveAmtMaximum) {
  Listing::Patch patch;
  EXPECT_CALL(unmarshaller(),
              real(Eq(Attribute::RandomAggressiveAmtMaximum), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.random_aggressive_amt_maximum(), Optional(DoubleEq(42.42)));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::test