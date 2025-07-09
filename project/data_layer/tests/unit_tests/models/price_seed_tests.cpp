#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>

#include "api/models/price_seed.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

TEST(DataLayer_Model_PriceSeed, Patch_Set_Symbol) {
  PriceSeed::Patch patch{};
  EXPECT_FALSE(patch.symbol().has_value());

  patch.with_symbol("AAPL");
  EXPECT_THAT(patch.symbol(), Optional(Eq("AAPL")));
}

TEST(DataLayer_Model_PriceSeed, Patch_Set_SecurityType) {
  PriceSeed::Patch patch{};
  EXPECT_FALSE(patch.security_type().has_value());

  patch.with_security_type("CS");
  EXPECT_THAT(patch.security_type(), Optional(Eq("CS")));
}

TEST(DataLayer_Model_PriceSeed, Patch_Set_PriceCurrency) {
  PriceSeed::Patch patch{};
  EXPECT_FALSE(patch.price_currency().has_value());

  patch.with_price_currency("USD");
  EXPECT_THAT(patch.price_currency(), Optional(Eq("USD")));
}

TEST(DataLayer_Model_PriceSeed, Patch_Set_SecurityID) {
  PriceSeed::Patch patch{};
  EXPECT_FALSE(patch.security_id().has_value());

  patch.with_security_id("US0378331005");
  EXPECT_THAT(patch.security_id(), Optional(Eq("US0378331005")));
}

TEST(DataLayer_Model_PriceSeed, Patch_Set_SecurityIDSource) {
  PriceSeed::Patch patch{};
  EXPECT_FALSE(patch.security_id_source().has_value());

  patch.with_security_id_source("ISIN");
  EXPECT_THAT(patch.security_id_source(), Optional(Eq("ISIN")));
}

TEST(DataLayer_Model_PriceSeed, Patch_Set_InstrumentSymbol) {
  PriceSeed::Patch patch{};
  EXPECT_FALSE(patch.instrument_symbol().has_value());

  patch.with_instrument_symbol("AAPL");
  EXPECT_THAT(patch.instrument_symbol(), Optional(Eq("AAPL")));
}

TEST(DataLayer_Model_PriceSeed, Patch_Set_MidPrice) {
  PriceSeed::Patch patch{};
  EXPECT_FALSE(patch.mid_price().has_value());

  patch.with_mid_price(42.42);  // NOLINT: test value
  EXPECT_THAT(patch.mid_price(), Optional(DoubleEq(42.42)));
}

TEST(DataLayer_Model_PriceSeed, Patch_Set_BidPrice) {
  PriceSeed::Patch patch{};
  EXPECT_FALSE(patch.bid_price().has_value());

  patch.with_bid_price(42.42);  // NOLINT: test value
  EXPECT_THAT(patch.bid_price(), Optional(DoubleEq(42.42)));
}

TEST(DataLayer_Model_PriceSeed, Patch_Set_OfferPrice) {
  PriceSeed::Patch patch{};
  EXPECT_FALSE(patch.offer_price().has_value());

  patch.with_offer_price(42.42);  // NOLINT: test value
  EXPECT_THAT(patch.offer_price(), Optional(DoubleEq(42.42)));
}

TEST(DataLayer_Model_PriceSeed, Patch_Set_LastUpdate) {
  PriceSeed::Patch patch{};
  EXPECT_FALSE(patch.last_update().has_value());

  patch.with_last_update("2023-08-25 12:03:45");
  EXPECT_THAT(patch.last_update(), Optional(Eq("2023-08-25 12:03:45")));
}

TEST(DataLayer_Model_PriceSeed, Get_PriceSeedID) {
  const PriceSeed::Patch patch{};

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.price_seed_id(), 42);
}

TEST(DataLayer_Model_PriceSeed, Get_Symbol_Missing) {
  const PriceSeed::Patch patch{};

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.symbol(), std::nullopt);
}

TEST(DataLayer_Model_PriceSeed, Get_Symbol_Specified) {
  PriceSeed::Patch patch{};
  patch.with_symbol("AAPL");

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.symbol(), Optional(Eq("AAPL")));
}

TEST(DataLayer_Model_PriceSeed, Get_SecurityType_Missing) {
  const PriceSeed::Patch patch{};

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.security_type(), std::nullopt);
}

TEST(DataLayer_Model_PriceSeed, Get_SecurityType_Specified) {
  PriceSeed::Patch patch{};
  patch.with_security_type("CS");

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.security_type(), Optional(Eq("CS")));
}

TEST(DataLayer_Model_PriceSeed, Get_PriceCurrency_Missing) {
  const PriceSeed::Patch patch{};

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.price_currency(), std::nullopt);
}

TEST(DataLayer_Model_PriceSeed, Get_PriceCurrency_Specified) {
  PriceSeed::Patch patch{};
  patch.with_price_currency("USD");

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.price_currency(), Optional(Eq("USD")));
}

TEST(DataLayer_Model_PriceSeed, Get_SecurityID_Missing) {
  const PriceSeed::Patch patch{};

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.security_id(), std::nullopt);
}

TEST(DataLayer_Model_PriceSeed, Get_SecurityID_Specified) {
  PriceSeed::Patch patch{};
  patch.with_security_id("US0378331005");

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.security_id(), Optional(Eq("US0378331005")));
}

TEST(DataLayer_Model_PriceSeed, Get_SecurityIDSource_Missing) {
  const PriceSeed::Patch patch{};

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.security_id_source(), std::nullopt);
}

TEST(DataLayer_Model_PriceSeed, Get_SecurityIDSource_Specified) {
  PriceSeed::Patch patch{};
  patch.with_security_id_source("ISIN");

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.security_id_source(), Optional(Eq("ISIN")));
}

TEST(DataLayer_Model_PriceSeed, Get_MidPrice_Missing) {
  const PriceSeed::Patch patch{};

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.mid_price(), std::nullopt);
}

TEST(DataLayer_Model_PriceSeed, Get_MidPrice_Specified) {
  PriceSeed::Patch patch{};
  patch.with_mid_price(42.42);

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.mid_price(), Optional(DoubleEq(42.42)));
}

TEST(DataLayer_Model_PriceSeed, Get_BidPrice_Missing) {
  const PriceSeed::Patch patch{};

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.bid_price(), std::nullopt);
}

TEST(DataLayer_Model_PriceSeed, Get_BidPrice_Specified) {
  PriceSeed::Patch patch{};
  patch.with_bid_price(42.42);

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.bid_price(), Optional(DoubleEq(42.42)));
}

TEST(DataLayer_Model_PriceSeed, Get_OfferPrice_Missing) {
  const PriceSeed::Patch patch{};

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.offer_price(), std::nullopt);
}

TEST(DataLayer_Model_PriceSeed, Get_OfferPrice_Specified) {
  PriceSeed::Patch patch{};
  patch.with_offer_price(42.42);

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.offer_price(), Optional(DoubleEq(42.42)));
}

TEST(DataLayer_Model_PriceSeed, Get_LastUpdate_Missing) {
  const PriceSeed::Patch patch{};

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_EQ(seed.last_update(), std::nullopt);
}

TEST(DataLayer_Model_PriceSeed, Get_LastUpdate_Specified) {
  PriceSeed::Patch patch{};
  patch.with_last_update("2023-08-25 12:03:45");

  PriceSeed seed = PriceSeed::create(patch, 42);
  EXPECT_THAT(seed.last_update(), Optional(Eq("2023-08-25 12:03:45")));
}

}  // namespace
}  // namespace simulator::data_layer::test