#include <gmock/gmock.h>

#include <optional>

#include "core/tools/price_tick_ranges.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/price_tick_range.hpp"
#include "ih/constants.hpp"
#include "ih/random/algorithm/utils/price_params_selector.hpp"
#include "ih/random/values/price_generation_params.hpp"

namespace simulator::generator::random::test {
namespace {

struct GeneratorRandomPriceParamsSelector : public ::testing::Test {
  data_layer::Listing::Patch patch;
  static constexpr std::uint64_t listing_id = 42;

  GeneratorRandomPriceParamsSelector() {
    patch.with_venue_id("Venue").with_engine_type(
        data_layer::Listing::EngineType::Matching);
  }
};

TEST_F(GeneratorRandomPriceParamsSelector,
       SelectsDefaultPriceTickRangeIfItIsMissing) {
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto params = PriceParamsSelector{listing}.select();
  EXPECT_EQ(params.get_price_tick_range(), constant::DefaultListingTickRange);
}

TEST_F(GeneratorRandomPriceParamsSelector, SelectsPriceTickRange) {
  constexpr std::uint32_t price_tick_range = 120;
  patch.with_random_tick_range(price_tick_range);
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto params = PriceParamsSelector{listing}.select();
  EXPECT_EQ(params.get_price_tick_range(), price_tick_range);
}

TEST_F(GeneratorRandomPriceParamsSelector,
       SelectsNoPriceTickRangesIfListingHasNone) {
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto params = PriceParamsSelector{listing}.select();
  EXPECT_EQ(params.get_price_tick_ranges(), core::PriceTickRanges{});
}

// NOLINTBEGIN(*magic-numbers*)
TEST_F(GeneratorRandomPriceParamsSelector,
       SelectsPriceTickRangesInSavedRowOrder) {
  using data_layer::PriceTickRange;
  patch.with_price_tick_range(PriceTickRange::Patch{}.with_tick_size(0.5))
      .with_price_tick_range(PriceTickRange::Patch{}.with_tick_size(0.25))
      .with_price_tick_range(
          PriceTickRange::Patch{}.with_tick_size(0.01).with_upper_limit(10.0));
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto params = PriceParamsSelector{listing}.select();
  EXPECT_EQ(params.get_price_tick_ranges(),
            (core::PriceTickRanges{{.tick_size = 0.5},
                                   {.tick_size = 0.01, .upper_limit = 10.0}}));
}
// NOLINTEND(*magic-numbers*)

TEST_F(GeneratorRandomPriceParamsSelector, SelectsNoPriceSpreadIfItIsMissing) {
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const PriceGenerationParams params = PriceParamsSelector{listing}.select();
  EXPECT_EQ(params.get_price_spread(), std::nullopt);
}

TEST_F(GeneratorRandomPriceParamsSelector, SelectsPriceSpread) {
  constexpr double price_spread = 0.15;
  patch.with_random_price_spread(price_spread);
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const PriceGenerationParams params = PriceParamsSelector{listing}.select();
  EXPECT_THAT(params.get_price_spread(),
              ::testing::Optional(::testing::DoubleEq(price_spread)));
}

}  // namespace
}  // namespace simulator::generator::random::test