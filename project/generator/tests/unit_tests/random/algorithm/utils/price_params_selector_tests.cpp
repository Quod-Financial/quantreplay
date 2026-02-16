#include <gtest/gtest.h>

#include "data_layer/api/models/listing.hpp"
#include "ih/constants.hpp"
#include "ih/random/algorithm/utils/price_params_selector.hpp"
#include "ih/random/values/price_generation_params.hpp"

namespace simulator::generator::random::test {
namespace {

struct GeneratorRandomPriceParamsSelector : public ::testing::Test {
  data_layer::Listing::Patch patch;
  static constexpr std::uint64_t listing_id = 42;

  GeneratorRandomPriceParamsSelector() { patch.with_venue_id("Venue"); }
};

TEST_F(GeneratorRandomPriceParamsSelector,
       SelectsDefaultPriceTickRangeIfItIsMissing) {
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto params = PriceParamsSelector::select(listing);
  EXPECT_EQ(params.get_price_tick_range(), constant::DefaultListingTickRange);
}

TEST_F(GeneratorRandomPriceParamsSelector, SelectsPriceTickRange) {
  constexpr std::uint32_t price_tick_range = 120;
  patch.with_random_tick_range(price_tick_range);
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto params = PriceParamsSelector::select(listing);
  EXPECT_EQ(params.get_price_tick_range(), price_tick_range);
}

TEST_F(GeneratorRandomPriceParamsSelector,
       SelectsDefaultPriceTickSizeIfItIsMissing) {
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto params = PriceParamsSelector::select(listing);
  EXPECT_DOUBLE_EQ(params.get_price_tick_size(),
                   constant::DefaultListingPriceTickSize);
}

TEST_F(GeneratorRandomPriceParamsSelector, SelectsPriceTickSize) {
  constexpr double tick_size = 12.34;
  patch.with_price_tick_size(tick_size);
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto params = PriceParamsSelector::select(listing);
  EXPECT_DOUBLE_EQ(params.get_price_tick_size(), tick_size);
}

TEST_F(GeneratorRandomPriceParamsSelector,
       SelectsDefaultPriceTickSizeIfPriceSpreadIsMissing) {
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const PriceGenerationParams params = PriceParamsSelector::select(listing);
  EXPECT_DOUBLE_EQ(params.get_price_spread(),
                   constant::DefaultListingPriceTickSize);
}

TEST_F(GeneratorRandomPriceParamsSelector,
       SelectsPriceTickSizeIfPriceSpreadIsMissing) {
  constexpr double tick_size = 12.34;
  patch.with_price_tick_size(tick_size);
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const PriceGenerationParams params = PriceParamsSelector::select(listing);
  EXPECT_DOUBLE_EQ(params.get_price_spread(), tick_size);
}

TEST_F(GeneratorRandomPriceParamsSelector, SelectsPriceSpread) {
  constexpr double price_spread = 0.15;
  patch.with_random_orders_spread(price_spread);
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const PriceGenerationParams params = PriceParamsSelector::select(listing);
  EXPECT_DOUBLE_EQ(params.get_price_spread(), price_spread);
}

TEST_F(GeneratorRandomPriceParamsSelector,
       SelectsPriceTickSizeIfPriceSpreadIsLessThanTickSize) {
  constexpr double tick_size = 0.2;
  constexpr double price_spread = 0.1;
  patch.with_price_tick_size(tick_size).with_random_orders_spread(price_spread);
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const PriceGenerationParams params = PriceParamsSelector::select(listing);
  ASSERT_DOUBLE_EQ(params.get_price_spread(), tick_size);
}

TEST_F(GeneratorRandomPriceParamsSelector,
       SelectsPriceSpreadIfItIsGreaterThanTickSize) {
  constexpr double tick_size = 0.1;
  constexpr double price_spread = 0.2;
  patch.with_price_tick_size(tick_size).with_random_orders_spread(price_spread);
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const PriceGenerationParams params = PriceParamsSelector::select(listing);
  ASSERT_DOUBLE_EQ(params.get_price_spread(), price_spread);
}

}  // namespace
}  // namespace simulator::generator::random::test