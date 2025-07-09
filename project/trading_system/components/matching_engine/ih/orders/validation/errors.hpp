#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_ERRORS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_ERRORS_HPP_

#include <fmt/format.h>

#include <cstdint>
#include <string_view>

namespace simulator::trading_system::matching_engine::order {

enum class ValidationError : std::uint8_t {
  SideMissing,
  SideUnknown,
  SideInvalid,
  OrderTypeMissing,
  OrderTypeUnknown,
  OrderStatusUnknown,
  OrderQuantityMissing,
  OrderQuantityMinViolated,
  OrderQuantityMaxViolated,
  OrderQuantityTickViolated,
  TotalQuantityMinViolated,
  TotalQuantityMaxViolated,
  TotalQuantityTickViolated,
  CumExecutedQuantityNonNegativityViolated,
  CumExecutedQuantityTickViolated,
  CumExecutedQuantityIsLessThanTotalQuantityViolated,
  OrderPriceMissing,
  OrderPriceNotAllowed,
  OrderPriceTickViolated,
  TimeInForceInvalid,
  OrderAlreadyExpired,
  BothExpireDateTimeSpecified,
  ExpireDateTimeMissing
};

[[nodiscard]]
constexpr auto describe(ValidationError error) -> std::string_view {
  switch (error) {
    case ValidationError::SideMissing:
      return "side missing";
    case ValidationError::SideUnknown:
      return "unsupported side value";
    case ValidationError::SideInvalid:
      return "invalid side value";
    case ValidationError::OrderTypeMissing:
      return "order type missing";
    case ValidationError::OrderTypeUnknown:
      return "unsupported order type value";
    case ValidationError::OrderStatusUnknown:
      return "unsupported order status value";
    case ValidationError::OrderQuantityMissing:
      return "order quantity missing";
    case ValidationError::OrderQuantityMinViolated:
      return "minimal order quantity constraint violated";
    case ValidationError::OrderQuantityMaxViolated:
      return "maximal order quantity constraint violated";
    case ValidationError::OrderQuantityTickViolated:
      return "order quantity multiple constraint violated";
    case ValidationError::TotalQuantityMinViolated:
      return "total quantity minimal constraint violated";
    case ValidationError::TotalQuantityMaxViolated:
      return "total quantity maximal constraint violated";
    case ValidationError::TotalQuantityTickViolated:
      return "total quantity multiple constraint violated";
    case ValidationError::CumExecutedQuantityNonNegativityViolated:
      return "cumulative executed quantity is less than zero";
    case ValidationError::CumExecutedQuantityTickViolated:
      return "cumulative executed quantity multiple constraint violated";
    case ValidationError::CumExecutedQuantityIsLessThanTotalQuantityViolated:
      return "cumulative executed quantity is not less than total quantity";
    case ValidationError::OrderPriceMissing:
      return "order price missing";
    case ValidationError::OrderPriceNotAllowed:
      return "order price is not allowed";
    case ValidationError::OrderPriceTickViolated:
      return "order price tick constraint violated";
    case ValidationError::TimeInForceInvalid:
      return "time in force value is invalid";
    case ValidationError::OrderAlreadyExpired:
      return "order already expired";
    case ValidationError::BothExpireDateTimeSpecified:
      return "both expire date and expire time specified";
    case ValidationError::ExpireDateTimeMissing:
      return "neither expire date nor expire time specified";
  }

  return "unknown order validation error";
}

}  // namespace simulator::trading_system::matching_engine::order

template <>
struct fmt::formatter<
    simulator::trading_system::matching_engine::order::ValidationError>
    : fmt::formatter<std::string_view> {
  using formattable =
      simulator::trading_system::matching_engine::order::ValidationError;

  auto format(formattable error, fmt::format_context& context) const {
    using base = fmt::formatter<std::string_view>;

    switch (error) {
      case formattable::SideMissing:
        return base::format("SideMissing", context);
      case formattable::SideUnknown:
        return base::format("SideUnknown", context);
      case formattable::SideInvalid:
        return base::format("SideInvalid", context);
      case formattable::OrderTypeMissing:
        return base::format("OrderTypeMissing", context);
      case formattable::OrderTypeUnknown:
        return base::format("OrderTypeUnknown", context);
      case formattable::OrderStatusUnknown:
        return base::format("OrderStatusUnknown", context);
      case formattable::OrderQuantityMissing:
        return base::format("OrderQuantityMissing", context);
      case formattable::OrderQuantityMinViolated:
        return base::format("OrderQuantityMinViolated", context);
      case formattable::OrderQuantityMaxViolated:
        return base::format("OrderQuantityMaxViolated", context);
      case formattable::OrderQuantityTickViolated:
        return base::format("OrderQuantityTickViolated", context);
      case formattable::TotalQuantityMinViolated:
        return base::format("TotalQuantityMinViolated", context);
      case formattable::TotalQuantityMaxViolated:
        return base::format("TotalQuantityMaxViolated", context);
      case formattable::TotalQuantityTickViolated:
        return base::format("TotalQuantityTickViolated", context);
      case formattable::CumExecutedQuantityNonNegativityViolated:
        return base::format("CumExecutedQuantityNonNegativityViolated",
                            context);
      case formattable::CumExecutedQuantityTickViolated:
        return base::format("CumExecutedQuantityTickViolated", context);
      case formattable::CumExecutedQuantityIsLessThanTotalQuantityViolated:
        return base::format(
            "CumExecutedQuantityIsLessThanTotalQuantityViolated", context);
      case formattable::OrderPriceMissing:
        return base::format("OrderPriceMissing", context);
      case formattable::OrderPriceNotAllowed:
        return base::format("OrderPriceNotAllowed", context);
      case formattable::OrderPriceTickViolated:
        return base::format("OrderPriceTickViolated", context);
      case formattable::TimeInForceInvalid:
        return base::format("TimeInForceInvalid", context);
      case formattable::OrderAlreadyExpired:
        return base::format("OrderAlreadyExpired", context);
      case formattable::BothExpireDateTimeSpecified:
        return base::format("BothExpireDateTimeSpecified", context);
      case formattable::ExpireDateTimeMissing:
        return base::format("ExpireDateTimeMissing", context);
    }

    return base::format("UnknownOrderValidationError", context);
  }
};

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_ERRORS_HPP_