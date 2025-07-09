#include <gtest/gtest.h>

#include "core/tools/numeric.hpp"

namespace simulator::core::test {
namespace {

TEST(CommonNumericUtilsEqual, ZerosFloatingPointValuesAreEqual) {
  EXPECT_TRUE(equal(0., 0.));
}

TEST(CommonNumericUtilsEqual, DifferentValuesAreEqualPerTolerance) {
  // Note: the 3rd digit is not compared, so expression:
  // 1.001 == 1.002 is considered as true with 1e-3 precision
  constexpr double precision = 1e-3;

  EXPECT_TRUE(equal(0.001, 0.002, precision));
}

TEST(CommonNumericUtilsEqual, DifferentValuesAreDifferentPerTolerance) {
  constexpr double precision = 1e-15;

  EXPECT_FALSE(equal(0.000000000000010, 0.000000000000020, precision));
}

TEST(CommonNumericUtilsIsNumber, EmptyStringIsNotNumber) {
  EXPECT_FALSE(is_number(""));
}

TEST(CommonNumericUtilsIsNumber, StringWithSpacesIsNotNumber) {
  EXPECT_FALSE(is_number(" 4 "));
}

TEST(CommonNumericUtilsIsNumber, StringWithLettersIsNotNumber) {
  EXPECT_FALSE(is_number("4a"));
}

TEST(CommonNumericUtilsIsNumber, StringWithDigitsIsNumber) {
  EXPECT_TRUE(is_number("1234567890"));
}

TEST(CommonNumericUtilsIsFullyDivisible, ReturnsTrueIfValueIsZero) {
  ASSERT_TRUE(is_fully_divisible(0., 0.1));
}

TEST(CommonNumericUtilsIsFullyDivisible, ReturnsTrueIfValueIsFullyDivisible) {
  ASSERT_TRUE(is_fully_divisible(2.2, 0.1));
}

TEST(CommonNumericUtilsIsFullyDivisible,
     ReturnsFalseIfValueIsNotFullyDivisible) {
  ASSERT_FALSE(is_fully_divisible(2.2, 0.5));
}

TEST(CommonNumericUtilsIsFullyDivisible, ReturnsFalseIfDivisorIsZero) {
  ASSERT_FALSE(is_fully_divisible(1., 0.));
}

TEST(CommonNumericUtilsIsFullyDivisible, ReturnsFalseIfValueIsNan) {
  constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
  ASSERT_FALSE(is_fully_divisible(nan, 1.));
}

TEST(CommonNumericUtilsIsFullyDivisible, ReturnsFalseIfDivisorIsNan) {
  constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
  ASSERT_FALSE(is_fully_divisible(1., nan));
}

TEST(CommonNumericUtilsIsFullyDivisible, ReturnsFalseIfValueIsInfinity) {
  constexpr auto inf = std::numeric_limits<double>::infinity();
  ASSERT_FALSE(is_fully_divisible(inf, 1.));
}

TEST(CommonNumericUtilsIsFullyDivisible, ReturnsFalseIfDivisorIsInfinity) {
  constexpr auto inf = std::numeric_limits<double>::infinity();
  ASSERT_FALSE(is_fully_divisible(1., inf));
}

}  // namespace
}  // namespace simulator::core::test
