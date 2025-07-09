#include <gtest/gtest.h>

#include <string>

#include "data_layer/api/models/price_seed.hpp"
#include "ih/utils/validator.hpp"

namespace simulator::generator::test {
namespace {

// NOLINTBEGIN(*magic-numbers*)

class GeneratorValidator : public testing::Test {
 public:
  static constexpr std::uint64_t listing_id = 11;
  data_layer::Listing::Patch listing_patch;
  data_layer::PriceSeed::Patch price_seed_patch;

  GeneratorValidator() {
    listing_patch.with_venue_id("Venue");
    price_seed_patch.with_symbol("AAPL");
  }
};

TEST_F(GeneratorValidator, Listing_IsAcceptable_SymbolAbsent) {
  const auto listing = data_layer::Listing::create(listing_patch, listing_id);
  ASSERT_FALSE(listing.symbol());

  EXPECT_FALSE(Validator::is_acceptable(listing));
}

TEST_F(GeneratorValidator, Listing_IsAcceptable_SymbolIsEmpty) {
  listing_patch.with_symbol({});
  const auto listing = data_layer::Listing::create(listing_patch, listing_id);
  ASSERT_TRUE(listing.symbol());

  EXPECT_FALSE(Validator::is_acceptable(listing));
}

TEST_F(GeneratorValidator, Listing_IsAcceptable_ListingDisabled) {
  listing_patch.with_enabled_flag(false);
  const auto listing = data_layer::Listing::create(listing_patch, listing_id);

  EXPECT_FALSE(Validator::is_acceptable(listing));
}

TEST_F(GeneratorValidator, Listing_IsAcceptable_AcceptableListing) {
  listing_patch.with_enabled_flag(true).with_symbol("AAPL");
  const auto listing = data_layer::Listing::create(listing_patch, listing_id);

  EXPECT_TRUE(Validator::is_acceptable(listing));
}

TEST_F(GeneratorValidator, Listing_IsAcceptableForGeneration_SymbolAbsent) {
  listing_patch.with_enabled_flag(true);
  const auto listing = data_layer::Listing::create(listing_patch, listing_id);
  ASSERT_FALSE(listing.symbol());

  EXPECT_FALSE(Validator::is_acceptable_for_random_generation(listing));
}

TEST_F(GeneratorValidator, Listing_IsAcceptableForGeneration_SymbolIsEmpty) {
  listing_patch.with_enabled_flag(true).with_symbol({});
  const auto listing = data_layer::Listing::create(listing_patch, listing_id);

  EXPECT_FALSE(Validator::is_acceptable_for_random_generation(listing));
}

TEST_F(GeneratorValidator, Listing_IsAcceptableForGeneration_ListingDisabled) {
  listing_patch.with_enabled_flag(false);
  const auto listing = data_layer::Listing::create(listing_patch, listing_id);

  EXPECT_FALSE(Validator::is_acceptable_for_random_generation(listing));
}

TEST_F(GeneratorValidator,
       Listing_IsAcceptableForGeneration_RandomOrdersDisabled) {
  listing_patch.with_enabled_flag(true)
      .with_symbol("AAPL")
      .with_random_orders_enabled_flag(false);
  const auto listing = data_layer::Listing::create(listing_patch, listing_id);

  EXPECT_FALSE(Validator::is_acceptable_for_random_generation(listing));
}

TEST_F(GeneratorValidator,
       Listing_IsAcceptableForGeneration_RandomOrdersRate_Zero) {
  listing_patch.with_enabled_flag(true)
      .with_symbol("AAPL")
      .with_random_orders_enabled_flag(true)
      .with_random_orders_rate(0);
  const auto listing = data_layer::Listing::create(listing_patch, listing_id);

  EXPECT_FALSE(Validator::is_acceptable_for_random_generation(listing));
}

TEST_F(GeneratorValidator,
       Listing_IsAcceptableForGeneration_AcceptableListing) {
  listing_patch.with_enabled_flag(true)
      .with_symbol("AAPL")
      .with_random_orders_enabled_flag(true)
      .with_random_orders_rate(1);
  const auto listing = data_layer::Listing::create(listing_patch, listing_id);

  EXPECT_TRUE(Validator::is_acceptable_for_random_generation(listing));
}

TEST_F(GeneratorValidator, PriceSeed_IsAcceptable_PricesAbsent) {
  const auto price_seed = data_layer::PriceSeed::create(price_seed_patch, 1);
  ASSERT_FALSE(price_seed.bid_price().has_value());
  ASSERT_FALSE(price_seed.offer_price().has_value());
  ASSERT_FALSE(price_seed.mid_price().has_value());

  EXPECT_FALSE(Validator::is_acceptable(price_seed));
}

TEST_F(GeneratorValidator, PriceSeed_IsAcceptable_BidPriceOnly) {
  price_seed_patch.with_bid_price(118.5);
  const auto price_seed = data_layer::PriceSeed::create(price_seed_patch, 1);

  ASSERT_TRUE(price_seed.bid_price().has_value());
  ASSERT_FALSE(price_seed.offer_price().has_value());
  ASSERT_FALSE(price_seed.mid_price().has_value());

  EXPECT_FALSE(Validator::is_acceptable(price_seed));
}

TEST_F(GeneratorValidator, PriceSeed_IsAcceptable_OfferPriceOnly) {
  price_seed_patch.with_offer_price(122.5);
  const auto price_seed = data_layer::PriceSeed::create(price_seed_patch, 1);

  ASSERT_FALSE(price_seed.bid_price().has_value());
  ASSERT_TRUE(price_seed.offer_price().has_value());
  ASSERT_FALSE(price_seed.mid_price().has_value());

  EXPECT_FALSE(Validator::is_acceptable(price_seed));
}

TEST_F(GeneratorValidator, PriceSeed_IsAcceptable_MidPriceOnly) {
  price_seed_patch.with_mid_price(120.5);
  const auto price_seed = data_layer::PriceSeed::create(price_seed_patch, 1);

  ASSERT_FALSE(price_seed.bid_price().has_value());
  ASSERT_FALSE(price_seed.offer_price().has_value());
  ASSERT_TRUE(price_seed.mid_price().has_value());

  EXPECT_TRUE(Validator::is_acceptable(price_seed));
}

TEST_F(GeneratorValidator, PriceSeed_IsAcceptable_BidAndMidPrices) {
  price_seed_patch.with_bid_price(118.5).with_mid_price(120.5);
  const auto price_seed = data_layer::PriceSeed::create(price_seed_patch, 1);

  ASSERT_TRUE(price_seed.bid_price().has_value());
  ASSERT_FALSE(price_seed.offer_price().has_value());
  ASSERT_TRUE(price_seed.mid_price().has_value());

  EXPECT_TRUE(Validator::is_acceptable(price_seed));
}

TEST_F(GeneratorValidator, PriceSeed_IsAcceptable_OfferAndMidPrices) {
  price_seed_patch.with_mid_price(120.5).with_offer_price(122.5);
  const auto price_seed = data_layer::PriceSeed::create(price_seed_patch, 1);

  ASSERT_FALSE(price_seed.bid_price().has_value());
  ASSERT_TRUE(price_seed.offer_price().has_value());
  ASSERT_TRUE(price_seed.mid_price().has_value());

  EXPECT_TRUE(Validator::is_acceptable(price_seed));
}

TEST_F(GeneratorValidator, PriceSeed_IsAcceptable_BidAndOfferPrices) {
  price_seed_patch.with_bid_price(118.5).with_offer_price(122.5);
  const auto price_seed = data_layer::PriceSeed::create(price_seed_patch, 1);

  ASSERT_TRUE(price_seed.bid_price().has_value());
  ASSERT_TRUE(price_seed.offer_price().has_value());
  ASSERT_FALSE(price_seed.mid_price().has_value());

  EXPECT_TRUE(Validator::is_acceptable(price_seed));
}

TEST_F(GeneratorValidator, PriceSeed_IsAcceptable_AllPrices) {
  price_seed_patch.with_bid_price(118.5).with_mid_price(120.5).with_offer_price(
      122.5);
  const auto price_seed = data_layer::PriceSeed::create(price_seed_patch, 1);

  ASSERT_TRUE(price_seed.bid_price().has_value());
  ASSERT_TRUE(price_seed.offer_price().has_value());
  ASSERT_TRUE(price_seed.mid_price().has_value());

  EXPECT_TRUE(Validator::is_acceptable(price_seed));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::test