#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/historical/parsing/parsing.hpp"

namespace simulator::generator::historical::test {
namespace {

using namespace ::testing;

TEST(GeneratorHistoricalParsingParseValue, DoesNotParseStringStartsWithLetter) {
  double result{42.42};
  ASSERT_FALSE(parse_value("A5.5bc", result));
  ASSERT_THAT(result, DoubleEq(42.42));
}

TEST(GeneratorHistoricalParsingParseValue,
     DoesNotParseDoubleFromStringStartsWithDecimal) {
  double result{42.42};
  ASSERT_FALSE(parse_value("5.5NotANumber", result));
  ASSERT_THAT(result, DoubleEq(42.42));
}

TEST(GeneratorHistoricalParsingParseValue, ParsesNaN) {
  double result{42.42};
  ASSERT_TRUE(parse_value("NaN", result));
  ASSERT_THAT(result, IsNan());
}

TEST(GeneratorHistoricalParsingParseValue, ParsesDecimalString) {
  double result{3.14};
  ASSERT_TRUE(parse_value("42.42", result));
  ASSERT_THAT(result, DoubleEq(42.42));
}

TEST(GeneratorHistoricalParsingParseValue, DoesNotParseIncorrectTimepoint) {
  Timepoint result;
  ASSERT_FALSE(parse_value("20", result));
  ASSERT_EQ(result.time_since_epoch().count(), 0);
}

TEST(GeneratorHistoricalParsingParseValue, ParsesTimepointInMilliseconds) {
  Timepoint result;
  ASSERT_TRUE(parse_value("2023-01-20 12:00:32.123456", result));
  ASSERT_EQ(result.time_since_epoch().count(), 1674216032123000000);
}

TEST(GeneratorHistoricalParsingParseValue, ParsesStringAsItIs) {
  std::string result;
  ASSERT_TRUE(parse_value("dummy", result));
  ASSERT_EQ(result, "dummy");
}

TEST(GeneratorHistoricalParsingParseCell, DoesNotParseCellOfEmptyRow) {
  const Row row;
  double result{42.42};

  ASSERT_FALSE(parse_cell(row, 0, result));
  ASSERT_THAT(result, DoubleEq(42.42));
}

TEST(GeneratorHistoricalParsingParseCell, DoesNotParseCellOutOfRowBounds) {
  const Row row{"column0", "column1"};
  double result{42.42};

  ASSERT_FALSE(parse_cell(row, 2, result));
  ASSERT_THAT(result, DoubleEq(42.42));
}

TEST(GeneratorHistoricalParsingParseCell, ParsesDecimalCell) {
  const Row row{"42.42", "column1", ""};
  double result{3.14};

  ASSERT_TRUE(parse_cell(row, 0, result));
  ASSERT_THAT(result, DoubleEq(42.42));
}

TEST(GeneratorHistoricalParsingParseCell, ParsesStringCell) {
  const Row row{"42.42", "column1"};
  double result{3.14};

  ASSERT_TRUE(parse_cell(row, 0, result));
  ASSERT_THAT(result, DoubleEq(42.42));
}

}  // namespace
}  // namespace simulator::generator::historical::test