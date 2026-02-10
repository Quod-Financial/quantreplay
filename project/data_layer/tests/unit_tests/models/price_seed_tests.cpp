#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>

#include "api/models/price_seed.hpp"
#include "tests/test_utils/matchers.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

struct DataLayerModelPriceSeedPatch : public ::testing::Test {
  PriceSeed::Patch patch;
};

TEST_F(DataLayerModelPriceSeedPatch, SetsSymbolNull) {
  EXPECT_FALSE(patch.symbol().has_value());

  patch.with_symbol(std::nullopt);
  EXPECT_THAT(patch.symbol(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsSymbol) {
  EXPECT_FALSE(patch.symbol().has_value());

  patch.with_symbol("AAPL");
  EXPECT_THAT(patch.symbol(), IsPatchFieldWithValue(Optional(Eq("AAPL"))));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsSecurityTypeNull) {
  EXPECT_FALSE(patch.security_type().has_value());

  patch.with_security_type(std::nullopt);
  EXPECT_THAT(patch.security_type(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsSecurityType) {
  EXPECT_FALSE(patch.security_type().has_value());

  patch.with_security_type("CS");
  EXPECT_THAT(patch.security_type(), IsPatchFieldWithValue(Optional(Eq("CS"))));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsPriceCurrencyNull) {
  EXPECT_FALSE(patch.price_currency().has_value());

  patch.with_price_currency(std::nullopt);
  EXPECT_THAT(patch.price_currency(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsPriceCurrency) {
  EXPECT_FALSE(patch.price_currency().has_value());

  patch.with_price_currency("USD");
  EXPECT_THAT(patch.price_currency(),
              IsPatchFieldWithValue(Optional(Eq("USD"))));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsSecurityIDNull) {
  EXPECT_FALSE(patch.security_id().has_value());

  patch.with_security_id(std::nullopt);
  EXPECT_THAT(patch.security_id(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsSecurityID) {
  EXPECT_FALSE(patch.security_id().has_value());

  patch.with_security_id("US0378331005");
  EXPECT_THAT(patch.security_id(),
              IsPatchFieldWithValue(Optional(Eq("US0378331005"))));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsSecurityIDSourceNull) {
  EXPECT_FALSE(patch.security_id_source().has_value());

  patch.with_security_id_source(std::nullopt);
  EXPECT_THAT(patch.security_id_source(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsSecurityIDSource) {
  EXPECT_FALSE(patch.security_id_source().has_value());

  patch.with_security_id_source("ISIN");
  EXPECT_THAT(patch.security_id_source(),
              IsPatchFieldWithValue(Optional(Eq("ISIN"))));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsInstrumentSymbolNull) {
  EXPECT_FALSE(patch.instrument_symbol().has_value());

  patch.with_instrument_symbol(std::nullopt);
  EXPECT_THAT(patch.instrument_symbol(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsInstrumentSymbol) {
  EXPECT_FALSE(patch.instrument_symbol().has_value());

  patch.with_instrument_symbol("AAPL");
  EXPECT_THAT(patch.instrument_symbol(),
              IsPatchFieldWithValue(Optional(Eq("AAPL"))));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsMidPriceNull) {
  EXPECT_FALSE(patch.mid_price().has_value());

  patch.with_mid_price(std::nullopt);
  EXPECT_THAT(patch.mid_price(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsMidPrice) {
  EXPECT_FALSE(patch.mid_price().has_value());

  patch.with_mid_price(42.42);  // NOLINT: test value
  EXPECT_THAT(patch.mid_price(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsBidPriceNull) {
  EXPECT_FALSE(patch.bid_price().has_value());

  patch.with_bid_price(std::nullopt);
  EXPECT_THAT(patch.bid_price(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsBidPrice) {
  EXPECT_FALSE(patch.bid_price().has_value());

  patch.with_bid_price(42.42);  // NOLINT: test value
  EXPECT_THAT(patch.bid_price(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsOfferPriceNull) {
  EXPECT_FALSE(patch.offer_price().has_value());

  patch.with_offer_price(std::nullopt);
  EXPECT_THAT(patch.offer_price(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsOfferPrice) {
  EXPECT_FALSE(patch.offer_price().has_value());

  patch.with_offer_price(42.42);  // NOLINT: test value
  EXPECT_THAT(patch.offer_price(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsLastUpdateNull) {
  EXPECT_FALSE(patch.last_update().has_value());

  patch.with_last_update(std::nullopt);
  EXPECT_THAT(patch.last_update(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelPriceSeedPatch, SetsLastUpdate) {
  EXPECT_FALSE(patch.last_update().has_value());

  patch.with_last_update("2023-08-25 12:03:45");
  EXPECT_THAT(patch.last_update(),
              IsPatchFieldWithValue(Optional(Eq("2023-08-25 12:03:45"))));
}

struct DataLayerModelPriceSeed : public ::testing::Test {
  PriceSeed::Patch patch;
};

TEST_F(DataLayerModelPriceSeed, GetsPriceSeedID) {
  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.price_seed_id(), 42);
}

TEST_F(DataLayerModelPriceSeed, GetsSymbolMissing) {
  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.symbol(), std::nullopt);
}

TEST_F(DataLayerModelPriceSeed, GetsSymbolSpecified) {
  patch.with_symbol("AAPL");

  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.symbol(), Optional(Eq("AAPL")));
}

TEST_F(DataLayerModelPriceSeed, GetsSecurityTypeMissing) {
  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.security_type(), std::nullopt);
}

TEST_F(DataLayerModelPriceSeed, GetsSecurityTypeSpecified) {
  patch.with_security_type("CS");

  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.security_type(), Optional(Eq("CS")));
}

TEST_F(DataLayerModelPriceSeed, GetsPriceCurrencyMissing) {
  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.price_currency(), std::nullopt);
}

TEST_F(DataLayerModelPriceSeed, GetsPriceCurrencySpecified) {
  patch.with_price_currency("USD");

  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.price_currency(), Optional(Eq("USD")));
}

TEST_F(DataLayerModelPriceSeed, GetsSecurityIDMissing) {
  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.security_id(), std::nullopt);
}

TEST_F(DataLayerModelPriceSeed, GetsSecurityIDSpecified) {
  patch.with_security_id("US0378331005");

  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.security_id(), Optional(Eq("US0378331005")));
}

TEST_F(DataLayerModelPriceSeed, GetsSecurityIDSourceMissing) {
  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.security_id_source(), std::nullopt);
}

TEST_F(DataLayerModelPriceSeed, GetsSecurityIDSourceSpecified) {
  patch.with_security_id_source("ISIN");

  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.security_id_source(), Optional(Eq("ISIN")));
}

TEST_F(DataLayerModelPriceSeed, GetsMidPriceMissing) {
  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.mid_price(), std::nullopt);
}

TEST_F(DataLayerModelPriceSeed, GetsMidPriceSpecified) {
  patch.with_mid_price(42.42);

  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.mid_price(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerModelPriceSeed, GetsBidPriceMissing) {
  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.bid_price(), std::nullopt);
}

TEST_F(DataLayerModelPriceSeed, GetsBidPriceSpecified) {
  patch.with_bid_price(42.42);

  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.bid_price(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerModelPriceSeed, GetsOfferPriceMissing) {
  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.offer_price(), std::nullopt);
}

TEST_F(DataLayerModelPriceSeed, GetsOfferPriceSpecified) {
  patch.with_offer_price(42.42);

  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.offer_price(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayerModelPriceSeed, GetsLastUpdateMissing) {
  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.last_update(), std::nullopt);
}

TEST_F(DataLayerModelPriceSeed, GetsLastUpdateSpecified) {
  patch.with_last_update("2023-08-25 12:03:45");

  const PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.last_update(), Optional(Eq("2023-08-25 12:03:45")));
}

}  // namespace
}  // namespace simulator::data_layer::test
