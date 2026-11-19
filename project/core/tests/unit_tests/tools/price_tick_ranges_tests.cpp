#include <gmock/gmock.h>

#include <optional>
#include <utility>

#include "core/tools/price_tick_ranges.hpp"

namespace simulator::core::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

[[nodiscard]]
auto unsorted_ranges() -> PriceTickRanges {
  return {{.tick_size = 0.000001},
          {.tick_size = 0.01, .upper_limit = 10.0},
          {.tick_size = 0.001, .upper_limit = 1.0}};
}

[[nodiscard]]
auto bounded_ranges() -> PriceTickRanges {
  return {{.tick_size = 0.2, .upper_limit = 1.5},
          {.tick_size = 0.5, .upper_limit = 2.0}};
}

struct CorePriceTickRangesTick
    : public TestWithParam<std::pair<double, double>> {};

TEST_P(CorePriceTickRangesTick, IsTickSizeOfRangeHoldingPrice) {
  const auto [price, tick_size] = GetParam();

  ASSERT_THAT(unsorted_ranges().tick_of(price), Optional(DoubleEq(tick_size)));
}

INSTANTIATE_TEST_SUITE_P(Prices,
                         CorePriceTickRangesTick,
                         Values(std::make_pair(-1.0, 0.001),
                                std::make_pair(0.0, 0.001),
                                std::make_pair(0.5, 0.001),
                                std::make_pair(1.0, 0.001),
                                std::make_pair(1.001, 0.01),
                                std::make_pair(10.0, 0.01),
                                std::make_pair(10.000001, 0.000001),
                                std::make_pair(1000.0, 0.000001)));

TEST(CorePriceTickRanges, HasNoTickWithoutRows) {
  ASSERT_THAT(PriceTickRanges{}.tick_of(1.0), Eq(std::nullopt));
}

TEST(CorePriceTickRanges, AppliesHighestUpperLimitRowAboveEveryUpperLimit) {
  ASSERT_THAT(bounded_ranges().tick_of(2.5), Optional(DoubleEq(0.5)));
}

TEST(CorePriceTickRanges, AppliesFirstOfRowsWithoutUpperLimit) {
  const PriceTickRanges ranges{{.tick_size = 0.1},
                               {.tick_size = 0.5},
                               {.tick_size = 0.01, .upper_limit = 1.0}};

  ASSERT_THAT(ranges.tick_of(3.0), Optional(DoubleEq(0.1)));
}

TEST(CorePriceTickRanges, AppliesFirstOfRowsWithSameUpperLimit) {
  const PriceTickRanges ranges{{.tick_size = 0.1, .upper_limit = 10.0},
                               {.tick_size = 0.5, .upper_limit = 10.0},
                               {.tick_size = 1.0}};

  EXPECT_THAT(ranges.tick_of(10.0), Optional(DoubleEq(0.1)));
  EXPECT_THAT(ranges.tick_of(10.5), Optional(DoubleEq(1.0)));
}

TEST(CorePriceTickRanges,
     AppliesFirstOfRowsWithSameHighestUpperLimitAboveThatLimit) {
  const PriceTickRanges ranges{{.tick_size = 0.1, .upper_limit = 10.0},
                               {.tick_size = 0.5, .upper_limit = 10.0}};

  ASSERT_THAT(ranges.tick_of(10.5), Optional(DoubleEq(0.1)));
}

TEST(CorePriceTickRanges, TakesPriceWithinToleranceOfUpperLimitForThatLimit) {
  const PriceTickRanges ranges{{.tick_size = 0.1, .upper_limit = 0.3},
                               {.tick_size = 0.5}};
  const double price = 0.1 + 0.2;
  ASSERT_GT(price, 0.3);

  ASSERT_THAT(ranges.tick_of(price), Optional(DoubleEq(0.1)));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::core::test
