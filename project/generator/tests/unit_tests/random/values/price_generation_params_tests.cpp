#include <gmock/gmock.h>

#include <optional>

#include "core/tools/price_tick_ranges.hpp"
#include "ih/random/values/price_generation_params.hpp"

namespace simulator::generator::random::test {
namespace {

using namespace ::testing;  // NOLINT

TEST(GeneratorRandomPriceGenerationParams, SpecifyPriceTickRange) {
  constexpr int expected_px_tick_range = 23;

  PriceGenerationParams::Builder px_params_builder{};
  px_params_builder.set_price_tick_range(expected_px_tick_range);
  const PriceGenerationParams px_params{px_params_builder};

  EXPECT_EQ(px_params.get_price_tick_range(), expected_px_tick_range);

  EXPECT_EQ(px_params.get_price_tick_ranges(), core::PriceTickRanges{});
  EXPECT_EQ(px_params.get_price_spread(), std::nullopt);
}

TEST(GeneratorRandomPriceGenerationParams, SpecifyPriceTickRanges) {
  const core::PriceTickRanges expected_px_tick_ranges{
      {.tick_size = 32.235346642}};

  PriceGenerationParams::Builder px_params_builder{};
  px_params_builder.set_price_tick_ranges(expected_px_tick_ranges);
  const PriceGenerationParams px_params{px_params_builder};

  EXPECT_EQ(px_params.get_price_tick_ranges(), expected_px_tick_ranges);

  EXPECT_EQ(px_params.get_price_tick_range(), 0);
  EXPECT_EQ(px_params.get_price_spread(), std::nullopt);
}

TEST(GeneratorRandomPriceGenerationParams, SpecifyPriceSpread) {
  constexpr double expected_px_spread = 1.00000005;

  PriceGenerationParams::Builder px_params_builder{};
  px_params_builder.set_price_spread(expected_px_spread);
  const PriceGenerationParams px_params{px_params_builder};

  EXPECT_THAT(px_params.get_price_spread(),
              Optional(DoubleEq(expected_px_spread)));

  EXPECT_EQ(px_params.get_price_tick_range(), 0);
  EXPECT_EQ(px_params.get_price_tick_ranges(), core::PriceTickRanges{});
}

TEST(GeneratorRandomPriceGenerationParams, SpecifyAll) {
  constexpr int expected_px_tick_range = 2;
  const core::PriceTickRanges expected_px_tick_ranges{
      {.tick_size = 2.000000001}};
  constexpr double expected_px_spread = 1.00000005;

  PriceGenerationParams::Builder px_params_builder{};
  px_params_builder.set_price_tick_ranges(expected_px_tick_ranges);
  px_params_builder.set_price_spread(expected_px_spread);
  px_params_builder.set_price_tick_range(expected_px_tick_range);

  const PriceGenerationParams px_params{px_params_builder};

  EXPECT_EQ(px_params.get_price_tick_range(), expected_px_tick_range);
  EXPECT_EQ(px_params.get_price_tick_ranges(), expected_px_tick_ranges);
  EXPECT_THAT(px_params.get_price_spread(),
              Optional(DoubleEq(expected_px_spread)));
}

}  // namespace
}  // namespace simulator::generator::random::test