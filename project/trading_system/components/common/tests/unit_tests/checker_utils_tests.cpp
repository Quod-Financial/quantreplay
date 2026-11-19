#include <gtest/gtest.h>

#include <cstdint>

#include "common/checker_utils.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/price_tick_ranges.hpp"

namespace simulator::trading_system::test {
namespace {

enum class Error : std::uint8_t { CheckFailed };

TEST(TradingSystemCheckerUtilsFieldSpecified,
     ReturnsNulloptWhenFieldIsSpecified) {
  constexpr auto field = std::make_optional(Side::Option::Buy);
  ASSERT_EQ(field_specified(field, Error::CheckFailed), std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldSpecified,
     ReturnsErrorWhenFieldIsNotSpecified) {
  constexpr std::optional<Side> field;
  ASSERT_EQ(field_specified(field, Error::CheckFailed),
            std::make_optional(Error::CheckFailed));
}

TEST(TradingSystemCheckerUtilsFieldRespectsMinimum,
     ReturnsNulloptWhenValueIsNotSpecified) {
  constexpr std::optional<OrderQuantity> value;
  constexpr auto min = std::make_optional(MinQuantity{2});
  ASSERT_EQ(field_respects_minimum(value, min, Error::CheckFailed),
            std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldRespectsMinimum,
     ReturnsErrorWhenValueIsLessThanZero) {
  constexpr auto value = std::make_optional(OrderQuantity{-1});
  constexpr std::optional<MinQuantity> min;
  ASSERT_EQ(field_respects_minimum(value, min, Error::CheckFailed),
            Error::CheckFailed);
}

TEST(TradingSystemCheckerUtilsFieldRespectsMinimum,
     ReturnsErrorWhenValueIsZero) {
  constexpr auto value = std::make_optional(OrderQuantity{0});
  constexpr std::optional<MinQuantity> min;
  ASSERT_EQ(field_respects_minimum(value, min, Error::CheckFailed),
            Error::CheckFailed);
}

TEST(TradingSystemCheckerUtilsFieldRespectsMinimum,
     ReturnsNulloptWhenMinimumIsNotSpecifiedAndValueIsGreaterThanZero) {
  constexpr auto value = std::make_optional(OrderQuantity{2});
  constexpr std::optional<MinQuantity> min;
  ASSERT_EQ(field_respects_minimum(value, min, Error::CheckFailed),
            std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldRespectsMinimum,
     ReturnsNulloptWhenValueIsGreaterThanMinimum) {
  constexpr auto value = OrderQuantity{3};
  constexpr auto min = std::make_optional(MinQuantity{2});
  ASSERT_EQ(field_respects_minimum(value, min, Error::CheckFailed),
            std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldRespectsMinimum,
     ReturnsNulloptWhenValueIsEqualToMinimum) {
  constexpr auto value = OrderQuantity{2};
  constexpr auto min = std::make_optional(MinQuantity{2});
  ASSERT_EQ(field_respects_minimum(value, min, Error::CheckFailed),
            std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldRespectsMinimum,
     ReturnsErrorWhenValueIsLessThanMin) {
  constexpr auto value = OrderQuantity{1};
  constexpr auto min = std::make_optional(MinQuantity{2});
  ASSERT_EQ(field_respects_minimum(value, min, Error::CheckFailed),
            Error::CheckFailed);
}

TEST(TradingSystemCheckerUtilsFieldRespectsMaximum,
     ReturnsNulloptWhenValueIsNotSpecified) {
  constexpr std::optional<OrderQuantity> value;
  constexpr auto max = std::make_optional(MaxQuantity{2});
  ASSERT_EQ(field_respects_maximum(value, max, Error::CheckFailed),
            std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldRespectsMaximum,
     ReturnsNulloptWhenMaximumIsNotSpecified) {
  constexpr auto value = std::make_optional(OrderQuantity{3});
  constexpr std::optional<MaxQuantity> max;
  ASSERT_EQ(field_respects_maximum(value, max, Error::CheckFailed),
            std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldRespectsMaximum,
     ReturnsNulloptWhenValueIsLessThanMax) {
  constexpr auto value = OrderQuantity{1};
  constexpr auto max = std::make_optional(MaxQuantity{2});
  ASSERT_EQ(field_respects_maximum(value, max, Error::CheckFailed),
            std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldRespectsMaximum,
     ReturnsNulloptWhenValueIsEqualToMax) {
  constexpr auto value = OrderQuantity{2};
  constexpr auto max = std::make_optional(MaxQuantity{2});
  ASSERT_EQ(field_respects_maximum(value, max, Error::CheckFailed),
            std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldRespectsMaximum,
     ReturnsErrorWhenValueIsGreaterThanMax) {
  constexpr auto value = OrderQuantity{3};
  constexpr auto max = std::make_optional(MaxQuantity{2});
  ASSERT_EQ(field_respects_maximum(value, max, Error::CheckFailed),
            Error::CheckFailed);
}

TEST(TradingSystemCheckerUtilsFieldRespectsTick,
     ReturnsNulloptWhenValueIsNotSpecified) {
  constexpr std::optional<OrderQuantity> value;
  constexpr auto tick = std::make_optional(QuantityTick{2});
  ASSERT_EQ(field_respects_tick(value, tick, Error::CheckFailed), std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldRespectsTick,
     ReturnsNulloptWhenTickIsNotSpecified) {
  constexpr auto value = std::make_optional(OrderQuantity{3});
  constexpr std::optional<QuantityTick> tick;
  ASSERT_EQ(field_respects_tick(value, tick, Error::CheckFailed), std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldRespectsTick,
     ReturnsErrorWhenTickIsLessThanZero) {
  constexpr auto value = std::make_optional(OrderQuantity{3});
  constexpr auto tick = std::make_optional(QuantityTick{-1});
  ASSERT_EQ(field_respects_tick(value, tick, Error::CheckFailed),
            Error::CheckFailed);
}

TEST(TradingSystemCheckerUtilsFieldRespectsTick, ReturnsErrorWhenTickIsZero) {
  constexpr auto value = std::make_optional(OrderQuantity{3});
  constexpr auto tick = std::make_optional(QuantityTick{0});
  ASSERT_EQ(field_respects_tick(value, tick, Error::CheckFailed),
            Error::CheckFailed);
}

TEST(TradingSystemCheckerUtilsFieldRespectsTick,
     ReturnsNulloptWhenValueIsMultipleOfTick) {
  constexpr auto value = std::make_optional(OrderQuantity{16});
  constexpr auto tick = std::make_optional(QuantityTick{2});
  ASSERT_EQ(field_respects_tick(value, tick, Error::CheckFailed), std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldRespectsTick,
     ReturnsErrorWhenValueIsNotMultipleOfTick) {
  constexpr auto value = std::make_optional(OrderQuantity{16});
  constexpr auto tick = std::make_optional(QuantityTick{3});
  ASSERT_EQ(field_respects_tick(value, tick, Error::CheckFailed),
            Error::CheckFailed);
}

// NOLINTBEGIN(*magic-numbers*)

[[nodiscard]]
auto tick_ranges_split_at_ten() -> core::PriceTickRanges {
  return {{.tick_size = 0.25, .upper_limit = 10.0}, {.tick_size = 0.3}};
}

TEST(TradingSystemCheckerUtilsPriceRespectsTick,
     ReturnsNulloptWhenPriceIsNotSpecified) {
  const std::optional<OrderPrice> price;
  ASSERT_EQ(price_respects_tick(
                price, tick_ranges_split_at_ten(), Error::CheckFailed),
            std::nullopt);
}

TEST(TradingSystemCheckerUtilsPriceRespectsTick,
     ReturnsNulloptWithoutTickRanges) {
  const auto price = std::make_optional(OrderPrice{9.6});
  ASSERT_EQ(
      price_respects_tick(price, core::PriceTickRanges{}, Error::CheckFailed),
      std::nullopt);
}

TEST(TradingSystemCheckerUtilsPriceRespectsTick,
     ReturnsErrorWhenTickOfPriceRangeIsZero) {
  const core::PriceTickRanges tick_ranges{
      {.tick_size = 0.0, .upper_limit = 10.0}, {.tick_size = 0.3}};
  const auto price = std::make_optional(OrderPrice{9.6});
  ASSERT_EQ(price_respects_tick(price, tick_ranges, Error::CheckFailed),
            Error::CheckFailed);
}

TEST(TradingSystemCheckerUtilsPriceRespectsTick,
     ReturnsErrorWhenTickOfPriceRangeIsLessThanZero) {
  const core::PriceTickRanges tick_ranges{
      {.tick_size = -0.3, .upper_limit = 10.0}, {.tick_size = 0.3}};
  const auto price = std::make_optional(OrderPrice{9.6});
  ASSERT_EQ(price_respects_tick(price, tick_ranges, Error::CheckFailed),
            Error::CheckFailed);
}

TEST(TradingSystemCheckerUtilsPriceRespectsTick,
     ReturnsErrorWhenPriceIsMultipleOnlyOfTickOfAnotherRange) {
  const auto price = std::make_optional(OrderPrice{9.6});
  ASSERT_EQ(price_respects_tick(
                price, tick_ranges_split_at_ten(), Error::CheckFailed),
            Error::CheckFailed);
}

TEST(TradingSystemCheckerUtilsPriceRespectsTick,
     ReturnsNulloptWhenPriceIsMultipleOfTickOfItsRange) {
  const auto price = std::make_optional(OrderPrice{10.2});
  ASSERT_EQ(price_respects_tick(
                price, tick_ranges_split_at_ten(), Error::CheckFailed),
            std::nullopt);
}

// NOLINTEND(*magic-numbers*)

TEST(TradingSystemCheckerUtilsFieldsRespectOrderAttributes,
     ReturnsNulloptWhenPredicateReturnsTrue) {
  constexpr auto value1 = OrderQuantity{3};
  constexpr auto value2 = OrderQuantity{16};
  ASSERT_EQ(
      fields_respect_order(value1, value2, std::less{}, Error::CheckFailed),
      std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldsRespectOrderAttributes,
     ReturnsErrorWhenPredicateReturnsFalse) {
  constexpr auto value1 = OrderQuantity{16};
  constexpr auto value2 = OrderQuantity{3};
  ASSERT_EQ(
      fields_respect_order(value1, value2, std::less{}, Error::CheckFailed),
      Error::CheckFailed);
}

TEST(TradingSystemCheckerUtilsFieldsRespectOrder,
     ReturnsNulloptWhenPredicateReturnsTrue) {
  constexpr auto value1 = OrderQuantity{3};
  constexpr double value2 = 16;
  ASSERT_EQ(
      fields_respect_order(value1, value2, std::less{}, Error::CheckFailed),
      std::nullopt);
}

TEST(TradingSystemCheckerUtilsFieldsRespectOrder,
     ReturnsErrorWhenPredicateReturnsFalse) {
  constexpr auto value1 = OrderQuantity{16};
  constexpr double value2 = 3;
  ASSERT_EQ(
      fields_respect_order(value1, value2, std::less{}, Error::CheckFailed),
      Error::CheckFailed);
}

}  // namespace
}  // namespace simulator::trading_system::test
