#include <gtest/gtest.h>

#include "ih/random/values/price_generation_params.hpp"

namespace simulator::generator::random::test {
namespace {

TEST(Generator_Random_PriceGenerationParams, SpecifyPriceTickRange) {
  constexpr int expected_px_tick_range = 23;

  PriceGenerationParams::Builder px_params_builder{};
  px_params_builder.set_price_tick_range(expected_px_tick_range);
  const PriceGenerationParams px_params{px_params_builder};

  EXPECT_EQ(px_params.get_price_tick_range(), expected_px_tick_range);

  EXPECT_DOUBLE_EQ(px_params.get_price_tick_size(), 0.);
  EXPECT_DOUBLE_EQ(px_params.get_price_spread(), 0.);
}

TEST(Generator_Random_PriceGenerationParams, SpecifyPriceTickSize) {
  constexpr double expected_px_tick_size = 32.235346642;

  PriceGenerationParams::Builder px_params_builder{};
  px_params_builder.set_price_tick_size(expected_px_tick_size);
  const PriceGenerationParams px_params{px_params_builder};

  EXPECT_DOUBLE_EQ(px_params.get_price_tick_size(), expected_px_tick_size);

  EXPECT_EQ(px_params.get_price_tick_range(), 0);
  EXPECT_DOUBLE_EQ(px_params.get_price_spread(), 0.);
}

TEST(Generator_Random_PriceGenerationParams, SpecifyPriceSpread) {
  constexpr double expected_px_spread = 1.00000005;

  PriceGenerationParams::Builder px_params_builder{};
  px_params_builder.set_price_spread(expected_px_spread);
  const PriceGenerationParams px_params{px_params_builder};

  EXPECT_DOUBLE_EQ(px_params.get_price_spread(), expected_px_spread);

  EXPECT_EQ(px_params.get_price_tick_range(), 0);
  EXPECT_DOUBLE_EQ(px_params.get_price_tick_size(), 0.);
}

TEST(Generator_Random_PriceGenerationParams, SpecifyAll) {
  constexpr int expected_px_tick_range = 2;
  constexpr double expected_px_tick_size = 2.000000001;
  constexpr double expected_px_spread = 1.00000005;

  PriceGenerationParams::Builder px_params_builder{};
  px_params_builder.set_price_tick_size(expected_px_tick_size);
  px_params_builder.set_price_spread(expected_px_spread);
  px_params_builder.set_price_tick_range(expected_px_tick_range);

  const PriceGenerationParams px_params{px_params_builder};

  EXPECT_EQ(px_params.get_price_tick_range(), expected_px_tick_range);
  EXPECT_DOUBLE_EQ(px_params.get_price_tick_size(), expected_px_tick_size);
  EXPECT_DOUBLE_EQ(px_params.get_price_spread(), expected_px_spread);
}

}  // namespace
}  // namespace simulator::generator::random::test