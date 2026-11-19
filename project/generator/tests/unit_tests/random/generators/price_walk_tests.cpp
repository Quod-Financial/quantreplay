#include <gmock/gmock.h>

#include <cstdint>
#include <utility>

#include "core/tools/price_tick_ranges.hpp"
#include "ih/random/generators/price_walk.hpp"

namespace simulator::generator::random::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

[[nodiscard]]
auto coarser_above_ten() -> core::PriceTickRanges {
  return {{.tick_size = 0.3, .upper_limit = 10.0},
          {.tick_size = 0.5, .upper_limit = 20.0}};
}

[[nodiscard]]
auto coarser_above_one_and_half() -> core::PriceTickRanges {
  return {{.tick_size = 0.2, .upper_limit = 1.5},
          {.tick_size = 0.5, .upper_limit = 2.0}};
}

struct GeneratorRandomPriceWalkUp
    : public TestWithParam<std::pair<std::uint32_t, double>> {};

TEST_P(GeneratorRandomPriceWalkUp, MovesByRangeTickSizeAndRoundsUpOnChange) {
  const auto [ticks, walked_price] = GetParam();

  const auto walk =
      walk_price(9.0, PriceDirection::Up, ticks, coarser_above_ten());

  ASSERT_DOUBLE_EQ(walk.price, walked_price);
}

INSTANTIATE_TEST_SUITE_P(Ticks,
                         GeneratorRandomPriceWalkUp,
                         Values(std::make_pair(0U, 9.0),
                                std::make_pair(1U, 9.3),
                                std::make_pair(2U, 9.6),
                                std::make_pair(3U, 9.9),
                                std::make_pair(4U, 10.5),
                                std::make_pair(5U, 11.0)));

struct GeneratorRandomPriceWalkDown
    : public TestWithParam<std::pair<std::uint32_t, double>> {};

TEST_P(GeneratorRandomPriceWalkDown,
       MovesByRangeTickSizeAndRoundsDownOnChange) {
  const auto [ticks, walked_price] = GetParam();

  const auto walk = walk_price(
      2.5, PriceDirection::Down, ticks, coarser_above_one_and_half());

  ASSERT_DOUBLE_EQ(walk.price, walked_price);
}

INSTANTIATE_TEST_SUITE_P(Ticks,
                         GeneratorRandomPriceWalkDown,
                         Values(std::make_pair(0U, 2.5),
                                std::make_pair(1U, 2.0),
                                std::make_pair(2U, 1.4),
                                std::make_pair(3U, 1.2),
                                std::make_pair(4U, 1.0),
                                std::make_pair(5U, 0.8)));

struct GeneratorRandomPriceWalkNonPositiveTickSize
    : public TestWithParam<double> {};

TEST_P(GeneratorRandomPriceWalkNonPositiveTickSize, IsNotRoundedTo) {
  const core::PriceTickRanges tick_ranges{
      {.tick_size = 0.5, .upper_limit = 10.0}, {.tick_size = GetParam()}};

  const auto walk = walk_price(9.75, PriceDirection::Up, 1, tick_ranges);

  ASSERT_DOUBLE_EQ(walk.price, 10.25);
}

INSTANTIATE_TEST_SUITE_P(TickSizes,
                         GeneratorRandomPriceWalkNonPositiveTickSize,
                         Values(0.0, -0.3));

TEST(GeneratorRandomPriceWalk, MovesRemainingTicksByTickSizeOfNextRange) {
  const core::PriceTickRanges tick_ranges{
      {.tick_size = 0.2, .upper_limit = 10.0}, {.tick_size = 0.3}};

  const auto walk = walk_price(12.9, PriceDirection::Down, 12, tick_ranges);

  ASSERT_DOUBLE_EQ(walk.price, 9.4);
}

TEST(GeneratorRandomPriceWalk, KeepsPriceOnUpperLimitInRangeOfThatLimit) {
  const auto walk = walk_price(9.7, PriceDirection::Up, 1, coarser_above_ten());

  ASSERT_DOUBLE_EQ(walk.price, 10.0);
}

TEST(GeneratorRandomPriceWalk, RoundsPriceOnStepFromUpperLimit) {
  const auto walk = walk_price(9.7, PriceDirection::Up, 2, coarser_above_ten());

  ASSERT_DOUBLE_EQ(walk.price, 10.5);
}

TEST(GeneratorRandomPriceWalk, RoundsAgainWhenRoundingEntersNextRange) {
  const core::PriceTickRanges tick_ranges{
      {.tick_size = 0.25, .upper_limit = 10.0},
      {.tick_size = 2.0, .upper_limit = 11.0},
      {.tick_size = 5.0}};

  const auto walk = walk_price(10.0, PriceDirection::Up, 1, tick_ranges);

  ASSERT_DOUBLE_EQ(walk.price, 15.0);
}

TEST(GeneratorRandomPriceWalk, DoesNotMovePriceByZeroTickSize) {
  const core::PriceTickRanges tick_ranges{{.tick_size = 0.0}};

  const auto walk = walk_price(5.0, PriceDirection::Up, 3, tick_ranges);

  ASSERT_DOUBLE_EQ(walk.price, 5.0);
}

TEST(GeneratorRandomPriceWalk, MovesPriceByNegativeTickSize) {
  const core::PriceTickRanges tick_ranges{{.tick_size = -0.5}};

  const auto walk = walk_price(5.0, PriceDirection::Up, 2, tick_ranges);

  ASSERT_DOUBLE_EQ(walk.price, 4.0);
}

TEST(GeneratorRandomPriceWalk, KeepsLowestRangeTickSizeBelowZero) {
  const core::PriceTickRanges tick_ranges{
      {.tick_size = 0.25, .upper_limit = 1.0}, {.tick_size = 0.5}};

  const auto walk = walk_price(0.5, PriceDirection::Down, 4, tick_ranges);

  ASSERT_DOUBLE_EQ(walk.price, -0.5);
}

TEST(GeneratorRandomPriceWalk, KeepsStartPriceWithoutTickRanges) {
  const auto walk =
      walk_price(5.0, PriceDirection::Up, 3, core::PriceTickRanges{});

  ASSERT_DOUBLE_EQ(walk.price, 5.0);
}

TEST(GeneratorRandomPriceWalk, CountsRoundingIntoDistance) {
  const auto walk = walk_price(9.0, PriceDirection::Up, 5, coarser_above_ten());

  ASSERT_DOUBLE_EQ(walk.distance, 2.0);
}

TEST(GeneratorRandomPriceShift, RoundsPriceDownInRangeWithAnotherTickSize) {
  const double price =
      shift_price(2.0, 0.5, PriceDirection::Down, coarser_above_one_and_half());

  ASSERT_DOUBLE_EQ(price, 1.4);
}

TEST(GeneratorRandomPriceShift, RoundsPriceUpInRangeWithAnotherTickSize) {
  const double price =
      shift_price(9.9, 0.3, PriceDirection::Up, coarser_above_ten());

  ASSERT_DOUBLE_EQ(price, 10.5);
}

TEST(GeneratorRandomPriceShift, DoesNotRoundPriceThatStaysInRange) {
  const double price =
      shift_price(3.0, 0.7, PriceDirection::Down, coarser_above_one_and_half());

  ASSERT_DOUBLE_EQ(price, 2.3);
}

TEST(GeneratorRandomDistancePrice, RoundsTickSizeAtZeroUpToTickSizeOfItsRange) {
  const core::PriceTickRanges tick_ranges{
      {.tick_size = 0.01, .upper_limit = 0.005}, {.tick_size = 0.02}};

  const double price = distance_price(0.01, PriceDirection::Down, tick_ranges);

  ASSERT_DOUBLE_EQ(price, 0.02);
}

TEST(GeneratorRandomDistancePrice, DoesNotRoundNegativeDistance) {
  const core::PriceTickRanges tick_ranges{{.tick_size = 0.25}};

  const double price = distance_price(-0.3, PriceDirection::Up, tick_ranges);

  ASSERT_DOUBLE_EQ(price, -0.3);
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::random::test
