#include <gtest/gtest.h>

#include <limits>

#include "core/tools/numeric.hpp"

namespace simulator::core::test {
namespace {

TEST(CommonNumericUtilsRoundToTick, ReturnsValueUnchangedWhenTickIsZero) {
  ASSERT_DOUBLE_EQ(round_to_tick(1.234, 0.0), 1.234);
}

TEST(CommonNumericUtilsRoundToTick, ReturnsValueUnchangedWhenTickIsNegative) {
  ASSERT_DOUBLE_EQ(round_to_tick(1.234, -0.1), 1.234);
}

TEST(CommonNumericUtilsRoundToTick, ReturnsValueUnchangedWhenValueIsNan) {
  constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
  ASSERT_TRUE(std::isnan(round_to_tick(nan, 0.1)));
}

TEST(CommonNumericUtilsRoundToTick, ReturnsValueUnchangedWhenTickIsNan) {
  constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
  ASSERT_DOUBLE_EQ(round_to_tick(1.234, nan), 1.234);
}

TEST(CommonNumericUtilsRoundToTick, ReturnsValueUnchangedWhenValueIsInfinity) {
  constexpr auto inf = std::numeric_limits<double>::infinity();
  ASSERT_TRUE(std::isinf(round_to_tick(inf, 0.1)));
}

TEST(CommonNumericUtilsRoundToTick, KeepsValueAlignedWithTick) {
  ASSERT_DOUBLE_EQ(round_to_tick(42.5, 0.5), 42.5);
}

TEST(CommonNumericUtilsRoundToTick, RoundsHalfAwayFromZeroForPositiveValues) {
  ASSERT_DOUBLE_EQ(round_to_tick(42.05, 0.1), 42.1);
}

TEST(CommonNumericUtilsRoundToTick, RoundsHalfAwayFromZeroForNegativeValues) {
  ASSERT_DOUBLE_EQ(round_to_tick(-42.05, 0.1), -42.1);
}

TEST(CommonNumericUtilsRoundToTick, RoundsDownTowardsLowerTick) {
  ASSERT_DOUBLE_EQ(round_to_tick(42.04, 0.1), 42.0);
}

TEST(CommonNumericUtilsRoundToTick, RoundsUpTowardsHigherTick) {
  ASSERT_DOUBLE_EQ(round_to_tick(42.06, 0.1), 42.1);
}

TEST(CommonNumericUtilsRoundToTick, RoundsValueToNearestSubCentTick) {
  ASSERT_DOUBLE_EQ(round_to_tick(0.123456, 0.0001), 0.1235);
}

TEST(CommonNumericUtilsRoundToTick, ReturnsZeroForZeroValue) {
  ASSERT_DOUBLE_EQ(round_to_tick(0.0, 0.05), 0.0);
}

}  // namespace
}  // namespace simulator::core::test
