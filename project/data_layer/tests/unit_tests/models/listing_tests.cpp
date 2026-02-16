#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "api/models/listing.hpp"
#include "ih/common/exceptions.hpp"
#include "tests/test_utils/matchers.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct DataLayerListingPatch : public Test {
  Listing::Patch patch;
};

TEST_F(DataLayerListingPatch, SetsSymbol) {
  ASSERT_FALSE(patch.symbol().has_value());

  patch.with_symbol("AAPL");
  EXPECT_THAT(patch.symbol(), Optional(Eq("AAPL")));
}

TEST_F(DataLayerListingPatch, SetsVenueID) {
  ASSERT_FALSE(patch.venue_id().has_value());

  patch.with_venue_id("NASDAQ");
  EXPECT_THAT(patch.venue_id(), Optional(Eq("NASDAQ")));
}

TEST_F(DataLayerListingPatch, SetsSecurityTypeNull) {
  ASSERT_FALSE(patch.security_type().has_value());

  patch.with_security_type(std::nullopt);
  EXPECT_THAT(patch.security_type(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsSecurityType) {
  ASSERT_FALSE(patch.security_type().has_value());

  patch.with_security_type("CS");
  EXPECT_THAT(patch.security_type(), IsPatchFieldWithValue(Optional(Eq("CS"))));
}

TEST_F(DataLayerListingPatch, SetsPriceCurrencyNull) {
  ASSERT_FALSE(patch.price_currency().has_value());

  patch.with_price_currency(std::nullopt);
  EXPECT_THAT(patch.price_currency(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsPriceCurrency) {
  ASSERT_FALSE(patch.price_currency().has_value());

  patch.with_price_currency("USD");
  EXPECT_THAT(patch.price_currency(),
              IsPatchFieldWithValue(Optional(Eq("USD"))));
}

TEST_F(DataLayerListingPatch, SetsFxBaseCurrencyNull) {
  ASSERT_FALSE(patch.fx_base_currency().has_value());

  patch.with_fx_base_currency(std::nullopt);
  EXPECT_THAT(patch.fx_base_currency(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsFxBaseCurrency) {
  ASSERT_FALSE(patch.fx_base_currency().has_value());

  patch.with_fx_base_currency("USD");
  EXPECT_THAT(patch.fx_base_currency(),
              IsPatchFieldWithValue(Optional(Eq("USD"))));
}

TEST_F(DataLayerListingPatch, SetsInstrSymbolNull) {
  ASSERT_FALSE(patch.instr_symbol().has_value());

  patch.with_instr_symbol(std::nullopt);
  EXPECT_THAT(patch.instr_symbol(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsInstrSymbol) {
  ASSERT_FALSE(patch.instr_symbol().has_value());

  patch.with_instr_symbol("AAPL");
  EXPECT_THAT(patch.instr_symbol(),
              IsPatchFieldWithValue(Optional(Eq("AAPL"))));
}

TEST_F(DataLayerListingPatch, SetsQtyMinimumNull) {
  ASSERT_FALSE(patch.qty_minimum().has_value());

  patch.with_qty_minimum(std::nullopt);
  EXPECT_THAT(patch.qty_minimum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsQtyMinimum) {
  ASSERT_FALSE(patch.qty_minimum().has_value());

  patch.with_qty_minimum(42.42);
  EXPECT_THAT(patch.qty_minimum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsQtyMaximumNull) {
  ASSERT_FALSE(patch.qty_maximum().has_value());

  patch.with_qty_maximum(std::nullopt);
  EXPECT_THAT(patch.qty_maximum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsQtyMaximum) {
  ASSERT_FALSE(patch.qty_maximum().has_value());

  patch.with_qty_maximum(42.42);
  EXPECT_THAT(patch.qty_maximum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsQtyMultipleNull) {
  ASSERT_FALSE(patch.qty_multiple().has_value());

  patch.with_qty_multiple(std::nullopt);
  EXPECT_THAT(patch.qty_multiple(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsQtyMultiple) {
  ASSERT_FALSE(patch.qty_multiple().has_value());

  patch.with_qty_multiple(42.42);
  EXPECT_THAT(patch.qty_multiple(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsPriceTickSizeNull) {
  ASSERT_FALSE(patch.price_tick_size().has_value());

  patch.with_price_tick_size(std::nullopt);
  EXPECT_THAT(patch.price_tick_size(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsPriceTickSize) {
  ASSERT_FALSE(patch.price_tick_size().has_value());

  patch.with_price_tick_size(42.42);
  EXPECT_THAT(patch.price_tick_size(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsEnabledNull) {
  ASSERT_FALSE(patch.enabled_flag().has_value());

  patch.with_enabled_flag(std::nullopt);
  EXPECT_THAT(patch.enabled_flag(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsEnabled) {
  ASSERT_FALSE(patch.enabled_flag().has_value());

  patch.with_enabled_flag(true);
  EXPECT_THAT(patch.enabled_flag(), IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerListingPatch, SetsRandomQtyMaximumNull) {
  ASSERT_FALSE(patch.random_qty_maximum().has_value());

  patch.with_random_qty_maximum(std::nullopt);
  EXPECT_THAT(patch.random_qty_maximum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomQtyMaximum) {
  ASSERT_FALSE(patch.random_qty_maximum().has_value());

  patch.with_random_qty_maximum(42.42);
  EXPECT_THAT(patch.random_qty_maximum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsRandomDepthLevelsNull) {
  ASSERT_FALSE(patch.random_depth_levels().has_value());

  patch.with_random_depth_levels(std::nullopt);
  EXPECT_THAT(patch.random_depth_levels(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomDepthLevels) {
  ASSERT_FALSE(patch.random_depth_levels().has_value());

  patch.with_random_depth_levels(42);
  EXPECT_THAT(patch.random_depth_levels(),
              IsPatchFieldWithValue(Optional(Eq(42))));
}

TEST_F(DataLayerListingPatch, SetsRandomOrderSpreadNull) {
  ASSERT_FALSE(patch.random_orders_spread().has_value());

  patch.with_random_orders_spread(std::nullopt);
  EXPECT_THAT(patch.random_orders_spread(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomOrderSpread) {
  ASSERT_FALSE(patch.random_orders_spread().has_value());

  patch.with_random_orders_spread(42.42);
  EXPECT_THAT(patch.random_orders_spread(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsRandomOrdersRateNull) {
  ASSERT_FALSE(patch.random_orders_rate().has_value());

  patch.with_random_orders_rate(std::nullopt);
  EXPECT_THAT(patch.random_orders_rate(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomOrdersRate) {
  ASSERT_FALSE(patch.random_orders_rate().has_value());

  patch.with_random_orders_rate(42);
  EXPECT_THAT(patch.random_orders_rate(),
              IsPatchFieldWithValue(Optional(Eq(42))));
}

TEST_F(DataLayerListingPatch, SetsRandomTickRangeNull) {
  ASSERT_FALSE(patch.random_tick_range().has_value());

  patch.with_random_tick_range(std::nullopt);
  EXPECT_THAT(patch.random_tick_range(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomTickRange) {
  ASSERT_FALSE(patch.random_tick_range().has_value());

  patch.with_random_tick_range(42);
  EXPECT_THAT(patch.random_tick_range(),
              IsPatchFieldWithValue(Optional(Eq(42))));
}

TEST_F(DataLayerListingPatch, SetsSecurityExchageNull) {
  ASSERT_FALSE(patch.security_exchange().has_value());

  patch.with_security_exchange(std::nullopt);
  EXPECT_THAT(patch.security_exchange(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsSecurityExchage) {
  ASSERT_FALSE(patch.security_exchange().has_value());

  patch.with_security_exchange("NASDAQ");
  EXPECT_THAT(patch.security_exchange(),
              IsPatchFieldWithValue(Optional(Eq("NASDAQ"))));
}

TEST_F(DataLayerListingPatch, SetsPartyIDNull) {
  ASSERT_FALSE(patch.party_id().has_value());

  patch.with_party_id(std::nullopt);
  EXPECT_THAT(patch.party_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsPartyID) {
  ASSERT_FALSE(patch.party_id().has_value());

  patch.with_party_id("GGV");
  EXPECT_THAT(patch.party_id(), IsPatchFieldWithValue(Optional(Eq("GGV"))));
}

TEST_F(DataLayerListingPatch, SetsPartyRoleNull) {
  ASSERT_FALSE(patch.party_role().has_value());

  patch.with_party_role(std::nullopt);
  EXPECT_THAT(patch.party_role(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsPartyRole) {
  ASSERT_FALSE(patch.party_role().has_value());

  patch.with_party_role("ExecutingFirm");
  EXPECT_THAT(patch.party_role(),
              IsPatchFieldWithValue(Optional(Eq("ExecutingFirm"))));
}

TEST_F(DataLayerListingPatch, SetsCusipIDNull) {
  ASSERT_FALSE(patch.cusip_id().has_value());

  patch.with_cusip_id(std::nullopt);
  EXPECT_THAT(patch.cusip_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsCusipID) {
  ASSERT_FALSE(patch.cusip_id().has_value());

  patch.with_cusip_id("037833100");
  EXPECT_THAT(patch.cusip_id(),
              IsPatchFieldWithValue(Optional(Eq("037833100"))));
}

TEST_F(DataLayerListingPatch, SetsSedolIDNull) {
  ASSERT_FALSE(patch.sedol_id().has_value());

  patch.with_sedol_id(std::nullopt);
  EXPECT_THAT(patch.sedol_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsSedolID) {
  ASSERT_FALSE(patch.sedol_id().has_value());

  patch.with_sedol_id("2046251");
  EXPECT_THAT(patch.sedol_id(), IsPatchFieldWithValue(Optional(Eq("2046251"))));
}

TEST_F(DataLayerListingPatch, SetsIsinIDNull) {
  ASSERT_FALSE(patch.isin_id().has_value());

  patch.with_isin_id(std::nullopt);
  EXPECT_THAT(patch.isin_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsIsinID) {
  ASSERT_FALSE(patch.isin_id().has_value());

  patch.with_isin_id("US0378331005");
  EXPECT_THAT(patch.isin_id(),
              IsPatchFieldWithValue(Optional(Eq("US0378331005"))));
}

TEST_F(DataLayerListingPatch, SetsRicIDNull) {
  ASSERT_FALSE(patch.ric_id().has_value());

  patch.with_ric_id(std::nullopt);
  EXPECT_THAT(patch.ric_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRicID) {
  ASSERT_FALSE(patch.ric_id().has_value());

  patch.with_ric_id("AAPL.OQ");
  EXPECT_THAT(patch.ric_id(), IsPatchFieldWithValue(Optional(Eq("AAPL.OQ"))));
}

TEST_F(DataLayerListingPatch, SetsExchangeSymbolIDNull) {
  ASSERT_FALSE(patch.exchange_symbol_id().has_value());

  patch.with_exchange_symbol_id(std::nullopt);
  EXPECT_THAT(patch.exchange_symbol_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsExchangeSymbolID) {
  ASSERT_FALSE(patch.exchange_symbol_id().has_value());

  patch.with_exchange_symbol_id("NASDAQ:AAPL");
  EXPECT_THAT(patch.exchange_symbol_id(),
              IsPatchFieldWithValue(Optional(Eq("NASDAQ:AAPL"))));
}

TEST_F(DataLayerListingPatch, SetsBloombergSymbolIDNull) {
  ASSERT_FALSE(patch.bloomberg_symbol_id().has_value());

  patch.with_bloomberg_symbol_id(std::nullopt);
  EXPECT_THAT(patch.bloomberg_symbol_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsBloombergSymbolID) {
  ASSERT_FALSE(patch.bloomberg_symbol_id().has_value());

  patch.with_bloomberg_symbol_id("AAPL:US");
  EXPECT_THAT(patch.bloomberg_symbol_id(),
              IsPatchFieldWithValue(Optional(Eq("AAPL:US"))));
}

TEST_F(DataLayerListingPatch, SetsRandomQtyMinimumNull) {
  ASSERT_FALSE(patch.random_qty_minimum().has_value());

  patch.with_random_qty_minimum(std::nullopt);
  EXPECT_THAT(patch.random_qty_minimum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomQtyMinimum) {
  ASSERT_FALSE(patch.random_qty_minimum().has_value());

  patch.with_random_qty_minimum(42.42);
  EXPECT_THAT(patch.random_qty_minimum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsRandomAmtMinimumNull) {
  ASSERT_FALSE(patch.random_amt_minimum().has_value());

  patch.with_random_amt_minimum(std::nullopt);
  EXPECT_THAT(patch.random_amt_minimum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomAmtMinimum) {
  ASSERT_FALSE(patch.random_amt_minimum().has_value());

  patch.with_random_amt_minimum(42.42);
  EXPECT_THAT(patch.random_amt_minimum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsRandomAmtMaximumNull) {
  ASSERT_FALSE(patch.random_amt_maximum().has_value());

  patch.with_random_amt_maximum(std::nullopt);
  EXPECT_THAT(patch.random_amt_maximum(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomAmtMaximum) {
  ASSERT_FALSE(patch.random_amt_minimum().has_value());

  patch.with_random_amt_maximum(42.42);
  EXPECT_THAT(patch.random_amt_maximum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsRandomOrdersEnabledFlagNull) {
  ASSERT_FALSE(patch.random_orders_enabled_flag().has_value());

  patch.with_random_orders_enabled_flag(std::nullopt);
  EXPECT_THAT(patch.random_orders_enabled_flag(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomOrdersEnabledFlag) {
  ASSERT_FALSE(patch.random_orders_enabled_flag().has_value());

  patch.with_random_orders_enabled_flag(false);
  EXPECT_THAT(patch.random_orders_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(false))));
}

TEST_F(DataLayerListingPatch, SetsRandomAggressiveQtyMinimumNull) {
  ASSERT_FALSE(patch.random_aggressive_qty_minimum().has_value());

  patch.with_random_aggressive_qty_minimum(std::nullopt);
  EXPECT_THAT(patch.random_aggressive_qty_minimum(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomAggressiveQtyMinimum) {
  ASSERT_FALSE(patch.random_aggressive_qty_minimum().has_value());

  patch.with_random_aggressive_qty_minimum(42.42);
  EXPECT_THAT(patch.random_aggressive_qty_minimum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsRandomAggressiveQtyMaximumNull) {
  ASSERT_FALSE(patch.random_aggressive_qty_maximum().has_value());

  patch.with_random_aggressive_qty_maximum(std::nullopt);
  EXPECT_THAT(patch.random_aggressive_qty_maximum(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomAggressiveQtyMaximum) {
  ASSERT_FALSE(patch.random_aggressive_qty_maximum().has_value());

  patch.with_random_aggressive_qty_maximum(42.42);
  EXPECT_THAT(patch.random_aggressive_qty_maximum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsRandomAggressiveAmtMinimumNull) {
  ASSERT_FALSE(patch.random_aggressive_amt_minimum().has_value());

  patch.with_random_aggressive_amt_minimum(std::nullopt);
  EXPECT_THAT(patch.random_aggressive_amt_minimum(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomAggressiveAmtMinimum) {
  ASSERT_FALSE(patch.random_aggressive_amt_minimum().has_value());

  patch.with_random_aggressive_amt_minimum(42.42);
  EXPECT_THAT(patch.random_aggressive_amt_minimum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerListingPatch, SetsRandomAggressiveAmtMaximumNull) {
  ASSERT_FALSE(patch.random_aggressive_amt_maximum().has_value());

  patch.with_random_aggressive_amt_maximum(std::nullopt);
  EXPECT_THAT(patch.random_aggressive_amt_maximum(),
              IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerListingPatch, SetsRandomAggressiveAmtMaximum) {
  ASSERT_FALSE(patch.random_aggressive_amt_maximum().has_value());

  patch.with_random_aggressive_amt_maximum(42.42);
  EXPECT_THAT(patch.random_aggressive_amt_maximum(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

struct DataLayerListing : public Test {
  Listing::Patch patch;
};

TEST_F(DataLayerListing, CreatedWithoutVenueID) {
  ASSERT_FALSE(patch.venue_id().has_value());

  EXPECT_THROW((void)Listing::create(patch, 42), RequiredAttributeMissing);
}

TEST_F(DataLayerListing, GetsListingID) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.listing_id(), 42);
}

TEST_F(DataLayerListing, GetsVenueID) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.venue_id(), "NASDAQ");
}

TEST_F(DataLayerListing, GetsSymbolMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.symbol(), std::nullopt);
}

TEST_F(DataLayerListing, GetsSymbolSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_symbol("AAPL");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.symbol(), Optional(Eq("AAPL")));
}

TEST_F(DataLayerListing, GetsSecurityTypeMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.security_type(), std::nullopt);
}

TEST_F(DataLayerListing, GetsSecurityTypeSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_security_type("CS");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.security_type(), Optional(Eq("CS")));
}

TEST_F(DataLayerListing, GetsPriceCurrencyMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.price_currency(), std::nullopt);
}

TEST_F(DataLayerListing, GetsPriceCurrencySpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_price_currency("USD");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.price_currency(), Optional(Eq("USD")));
}

TEST_F(DataLayerListing, GetsFxBaseCurrencyMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.fx_base_currency(), std::nullopt);
}

TEST_F(DataLayerListing, GetsFxBaseCurrencySpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_fx_base_currency("USD");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.fx_base_currency(), Optional(Eq("USD")));
}

TEST_F(DataLayerListing, GetsInstrSymbolMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.instr_symbol(), std::nullopt);
}

TEST_F(DataLayerListing, GetsInstrSymbolSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_instr_symbol("AAPL");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.instr_symbol(), Optional(Eq("AAPL")));
}

TEST_F(DataLayerListing, GetsQtyMinimumMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.qty_minimum(), std::nullopt);
}

TEST_F(DataLayerListing, GetsQtyMinimumSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_qty_minimum(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.qty_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsQtyMaximumMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.qty_maximum(), std::nullopt);
}

TEST_F(DataLayerListing, GetsQtyMaximumSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_qty_maximum(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.qty_maximum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsQtyMultipleMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.qty_multiple(), std::nullopt);
}

TEST_F(DataLayerListing, GetsQtyMultipleSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_qty_multiple(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.qty_multiple(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsQtyPriceTickSizeMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.price_tick_size(), std::nullopt);
}

TEST_F(DataLayerListing, GetsPriceTickSizeSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_price_tick_size(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.price_tick_size(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsEnabledMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.enabled_flag(), std::nullopt);
}

TEST_F(DataLayerListing, GetsEnabledSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_enabled_flag(true);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.enabled_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerListing, GetsRandomQtyMaximumMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_qty_maximum(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomQtyMaximumSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_qty_maximum(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_qty_maximum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsRandomDepthLevelsMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_depth_levels(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomDepthLevelsSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_depth_levels(42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_depth_levels(), Optional(Eq(42)));
}

TEST_F(DataLayerListing, GetsRandomOrdersSpreadMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_orders_spread(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomOrdersSpreadSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_orders_spread(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_orders_spread(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsRandomOrdersRateMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_orders_rate(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomOrdersRateSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_orders_rate(42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_orders_rate(), Optional(Eq(42)));
}

TEST_F(DataLayerListing, GetsRandomTickRangeMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_tick_range(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomTickRangeSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_tick_range(42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_tick_range(), Optional(Eq(42)));
}

TEST_F(DataLayerListing, GetsSecurityExchangeMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.security_exchange(), std::nullopt);
}

TEST_F(DataLayerListing, GetsSecurityExchangeSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_security_exchange("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.security_exchange(), Optional(Eq("NASDAQ")));
}

TEST_F(DataLayerListing, GetsPartyIDMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.party_id(), std::nullopt);
}

TEST_F(DataLayerListing, GetsPartyIDSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_party_id("GGV");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.party_id(), Optional(Eq("GGV")));
}

TEST_F(DataLayerListing, GetsPartyRoleMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.party_role(), std::nullopt);
}

TEST_F(DataLayerListing, GetsPartyRoleSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_party_role("ExecutingFirm");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.party_role(), Optional(Eq("ExecutingFirm")));
}

TEST_F(DataLayerListing, GetsCusipIDMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.cusip_id(), std::nullopt);
}

TEST_F(DataLayerListing, GetsCusipIDSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_cusip_id("037833100");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.cusip_id(), Optional(Eq("037833100")));
}

TEST_F(DataLayerListing, GetsSedolIDMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.sedol_id(), std::nullopt);
}

TEST_F(DataLayerListing, GetsSedolIDSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_sedol_id("2046251");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.sedol_id(), Optional(Eq("2046251")));
}

TEST_F(DataLayerListing, GetsIsinIDMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.isin_id(), std::nullopt);
}

TEST_F(DataLayerListing, GetsIsinIDSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_isin_id("US0378331005");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.isin_id(), Optional(Eq("US0378331005")));
}

TEST_F(DataLayerListing, GetsRicIDMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.ric_id(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRicIDSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_ric_id("AAPL.OQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.ric_id(), Optional(Eq("AAPL.OQ")));
}

TEST_F(DataLayerListing, GetsExchangeSymbolIDMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.exchange_symbol_id(), std::nullopt);
}

TEST_F(DataLayerListing, GetsExchangeSymbolIDSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_exchange_symbol_id("NASDAQ:AAPL");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.exchange_symbol_id(), Optional(Eq("NASDAQ:AAPL")));
}

TEST_F(DataLayerListing, GetsBloombergSymbolIDMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.bloomberg_symbol_id(), std::nullopt);
}

TEST_F(DataLayerListing, GetsBloombergSymbolIDSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_bloomberg_symbol_id("AAPL:US");

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.bloomberg_symbol_id(), Optional(Eq("AAPL:US")));
}

TEST_F(DataLayerListing, GetsRandomQtyMinimumMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_qty_minimum(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomQtyMinimumSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_qty_minimum(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_qty_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsRandomAmtMinimumMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_amt_minimum(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomAmtMinimumSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_amt_minimum(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_amt_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsRandomAmtMaximumMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_amt_maximum(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomAmtMaximumSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_amt_maximum(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_amt_maximum(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsRandomOrdersEnabledMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_orders_enabled_flag(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomOrdersEnabledSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_orders_enabled_flag(false);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_orders_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerListing, GetsRandomAggressiveQtyMinimumMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_aggressive_qty_minimum(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomAggressiveQtyMinimumSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_aggressive_qty_minimum(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_aggressive_qty_minimum(),
              Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsRandomAggressiveQtyMaximumMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_aggressive_qty_maximum(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomAggressiveQtyMaximumSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_aggressive_qty_maximum(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_aggressive_qty_maximum(),
              Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsRandomAggressiveAmtMinimumMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_aggressive_amt_minimum(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomAggressiveAmtMinimumSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_aggressive_amt_minimum(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_aggressive_amt_minimum(),
              Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerListing, GetsRandomAggressiveAmtMaximumMissing) {
  patch.with_venue_id("NASDAQ");

  Listing listing = Listing::create(patch, 42);
  EXPECT_EQ(listing.random_aggressive_amt_maximum(), std::nullopt);
}

TEST_F(DataLayerListing, GetsRandomAggressiveAmtMaximumSpecified) {
  patch.with_venue_id("NASDAQ");
  patch.with_random_aggressive_amt_maximum(42.42);

  Listing listing = Listing::create(patch, 42);
  EXPECT_THAT(listing.random_aggressive_amt_maximum(),
              Optional(DoubleEq(42.42)));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::test