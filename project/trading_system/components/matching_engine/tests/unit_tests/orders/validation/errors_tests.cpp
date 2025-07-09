#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/orders/validation/errors.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

struct OrderValidationErrorsDescription
    : public TestWithParam<std::pair<ValidationError, std::string_view>> {};

TEST_P(OrderValidationErrorsDescription, Describes) {
  const auto [error, expected] = GetParam();

  ASSERT_EQ(describe(error), expected);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Description,
    OrderValidationErrorsDescription,
    Values(
        std::make_pair(static_cast<ValidationError>(0xFF), "unknown order validation error"),
        std::make_pair(ValidationError::SideMissing, "side missing"),
        std::make_pair(ValidationError::SideUnknown, "unsupported side value"),
        std::make_pair(ValidationError::SideInvalid, "invalid side value"),
        std::make_pair(ValidationError::OrderTypeMissing, "order type missing"),
        std::make_pair(ValidationError::OrderTypeUnknown, "unsupported order type value"),
        std::make_pair(ValidationError::OrderStatusUnknown, "unsupported order status value"),
        std::make_pair(ValidationError::OrderQuantityMissing, "order quantity missing"),
        std::make_pair(ValidationError::OrderQuantityMinViolated, "minimal order quantity constraint violated"),
        std::make_pair(ValidationError::OrderQuantityMaxViolated, "maximal order quantity constraint violated"),
        std::make_pair(ValidationError::OrderQuantityTickViolated, "order quantity multiple constraint violated"),
        std::make_pair(ValidationError::TotalQuantityMinViolated, "total quantity minimal constraint violated"),
        std::make_pair(ValidationError::TotalQuantityMaxViolated, "total quantity maximal constraint violated"),
        std::make_pair(ValidationError::TotalQuantityTickViolated, "total quantity multiple constraint violated"),
        std::make_pair(ValidationError::CumExecutedQuantityNonNegativityViolated, "cumulative executed quantity is less than zero"),
        std::make_pair(ValidationError::CumExecutedQuantityTickViolated, "cumulative executed quantity multiple constraint violated"),
        std::make_pair(ValidationError::CumExecutedQuantityIsLessThanTotalQuantityViolated, "cumulative executed quantity is not less than total quantity"),
        std::make_pair(ValidationError::OrderPriceMissing, "order price missing"),
        std::make_pair(ValidationError::OrderPriceNotAllowed, "order price is not allowed"),
        std::make_pair(ValidationError::OrderPriceTickViolated, "order price tick constraint violated"),
        std::make_pair(ValidationError::TimeInForceInvalid, "time in force value is invalid"),
        std::make_pair(ValidationError::OrderAlreadyExpired, "order already expired"),
        std::make_pair(ValidationError::BothExpireDateTimeSpecified, "both expire date and expire time specified"),
        std::make_pair(ValidationError::ExpireDateTimeMissing, "neither expire date nor expire time specified")));
// clang-format on

struct OrderValidationErrorsFormatting
    : public TestWithParam<std::pair<ValidationError, std::string_view>> {};

TEST_P(OrderValidationErrorsFormatting, FmtFormatting) {
  const auto [error, expected] = GetParam();

  ASSERT_EQ(fmt::to_string(error), expected);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    OrderValidationErrorsFormatting,
    Values(
        std::make_pair(static_cast<ValidationError>(0xFF), "UnknownOrderValidationError"),
        std::make_pair(ValidationError::SideMissing, "SideMissing"),
        std::make_pair(ValidationError::SideUnknown, "SideUnknown"),
        std::make_pair(ValidationError::SideInvalid, "SideInvalid"),
        std::make_pair(ValidationError::OrderTypeMissing, "OrderTypeMissing"),
        std::make_pair(ValidationError::OrderTypeUnknown, "OrderTypeUnknown"),
        std::make_pair(ValidationError::OrderStatusUnknown, "OrderStatusUnknown"),
        std::make_pair(ValidationError::OrderQuantityMissing, "OrderQuantityMissing"),
        std::make_pair(ValidationError::OrderQuantityMinViolated, "OrderQuantityMinViolated"),
        std::make_pair(ValidationError::OrderQuantityMaxViolated, "OrderQuantityMaxViolated"),
        std::make_pair(ValidationError::OrderQuantityTickViolated, "OrderQuantityTickViolated"),
        std::make_pair(ValidationError::TotalQuantityMinViolated, "TotalQuantityMinViolated"),
        std::make_pair(ValidationError::TotalQuantityMaxViolated, "TotalQuantityMaxViolated"),
        std::make_pair(ValidationError::TotalQuantityTickViolated, "TotalQuantityTickViolated"),
        std::make_pair(ValidationError::CumExecutedQuantityNonNegativityViolated, "CumExecutedQuantityNonNegativityViolated"),
        std::make_pair(ValidationError::CumExecutedQuantityTickViolated, "CumExecutedQuantityTickViolated"),
        std::make_pair(ValidationError::CumExecutedQuantityIsLessThanTotalQuantityViolated, "CumExecutedQuantityIsLessThanTotalQuantityViolated"),
        std::make_pair(ValidationError::OrderPriceMissing, "OrderPriceMissing"),
        std::make_pair(ValidationError::OrderPriceNotAllowed, "OrderPriceNotAllowed"),
        std::make_pair(ValidationError::OrderPriceTickViolated, "OrderPriceTickViolated"),
        std::make_pair(ValidationError::TimeInForceInvalid, "TimeInForceInvalid"),
        std::make_pair(ValidationError::OrderAlreadyExpired, "OrderAlreadyExpired"),
        std::make_pair(ValidationError::BothExpireDateTimeSpecified, "BothExpireDateTimeSpecified"),
        std::make_pair(ValidationError::ExpireDateTimeMissing, "ExpireDateTimeMissing")));
// clang-format on

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
