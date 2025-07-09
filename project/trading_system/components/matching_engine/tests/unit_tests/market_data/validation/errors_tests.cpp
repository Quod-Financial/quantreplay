#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <utility>

#include "ih/market_data/validation/errors.hpp"

namespace simulator::trading_system::matching_engine::mdata::test {
namespace {

using namespace ::testing;  // NOLINT

struct MarketDataValidationErrorsDescription
    : public TestWithParam<std::pair<ValidationError, std::string_view>> {};

TEST_P(MarketDataValidationErrorsDescription, Describes) {
  const auto [error, expected] = GetParam();

  ASSERT_EQ(describe(error), expected);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Description,
    MarketDataValidationErrorsDescription,
    Values(
        std::make_pair(static_cast<ValidationError>(0xFF), "unknown market data validation error"),
        std::make_pair(ValidationError::LowPriceTickViolated, "low price tick constraint violated"),
        std::make_pair(ValidationError::HighPriceTickViolated, "high price tick constraint violated"),
        std::make_pair(ValidationError::LowPriceIsLessThanOrEqualToHighPriceViolated, "low price is less than or equal to high price constraint violated"),
        std::make_pair(ValidationError::TradePriceTickViolated, "trade price tick constraint violated"),
        std::make_pair(ValidationError::TradedQuantityTickViolated, "traded quantity multiple constraint violated"),
        std::make_pair(ValidationError::TradedQuantityMinViolated, "minimal traded quantity constraint violated"),
        std::make_pair(ValidationError::TradedQuantityMaxViolated, "maximal traded quantity constraint violated")));
// clang-format on

struct MarketDataValidationErrorsFormatting
    : public TestWithParam<std::pair<ValidationError, std::string_view>> {};

TEST_P(MarketDataValidationErrorsFormatting, FmtFormatting) {
  const auto [error, expected] = GetParam();

  ASSERT_EQ(fmt::to_string(error), expected);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    MarketDataValidationErrorsFormatting,
    Values(
        std::make_pair(static_cast<ValidationError>(0xFF), "UnknownMarketDataValidationError"),
        std::make_pair(ValidationError::LowPriceTickViolated, "LowPriceTickViolated"),
        std::make_pair(ValidationError::HighPriceTickViolated, "HighPriceTickViolated"),
        std::make_pair(ValidationError::LowPriceIsLessThanOrEqualToHighPriceViolated, "LowPriceIsLessThanOrEqualToHighPriceViolated"),
        std::make_pair(ValidationError::TradePriceTickViolated, "TradePriceTickViolated"),
        std::make_pair(ValidationError::TradedQuantityTickViolated, "TradedQuantityTickViolated"),
        std::make_pair(ValidationError::TradedQuantityMinViolated, "TradedQuantityMinViolated"),
        std::make_pair(ValidationError::TradedQuantityMaxViolated, "TradedQuantityMaxViolated")));
// clang-format on

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::test
