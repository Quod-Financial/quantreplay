#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_ERRORS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_ERRORS_HPP_

#include <fmt/format.h>

#include <string_view>

namespace simulator::trading_system::matching_engine::mdata {

enum class ValidationError {
  LowPriceTickViolated,
  HighPriceTickViolated,
  LowPriceIsLessThanOrEqualToHighPriceViolated,
  TradePriceTickViolated,
  TradedQuantityTickViolated,
  TradedQuantityMinViolated,
  TradedQuantityMaxViolated
};

[[nodiscard]]
constexpr auto describe(ValidationError error) -> std::string_view {
  switch (error) {
    case ValidationError::LowPriceTickViolated:
      return "low price tick constraint violated";
    case ValidationError::HighPriceTickViolated:
      return "high price tick constraint violated";
    case ValidationError::LowPriceIsLessThanOrEqualToHighPriceViolated:
      return "low price is less than or equal to high price constraint "
             "violated";
    case ValidationError::TradePriceTickViolated:
      return "trade price tick constraint violated";
    case ValidationError::TradedQuantityTickViolated:
      return "traded quantity multiple constraint violated";
    case ValidationError::TradedQuantityMinViolated:
      return "minimal traded quantity constraint violated";
    case ValidationError::TradedQuantityMaxViolated:
      return "maximal traded quantity constraint violated";
  }
  return "unknown market data validation error";
}

}  // namespace simulator::trading_system::matching_engine::mdata

template <>
struct fmt::formatter<
    simulator::trading_system::matching_engine::mdata::ValidationError>
    : fmt::formatter<std::string_view> {
  using formattable =
      simulator::trading_system::matching_engine::mdata::ValidationError;

  auto format(formattable error, fmt::format_context& context) const {
    using base = fmt::formatter<std::string_view>;

    switch (error) {
      case formattable::LowPriceTickViolated:
        return base::format("LowPriceTickViolated", context);
      case formattable::HighPriceTickViolated:
        return base::format("HighPriceTickViolated", context);
      case formattable::LowPriceIsLessThanOrEqualToHighPriceViolated:
        return base::format("LowPriceIsLessThanOrEqualToHighPriceViolated",
                            context);
      case formattable::TradePriceTickViolated:
        return base::format("TradePriceTickViolated", context);
      case formattable::TradedQuantityTickViolated:
        return base::format("TradedQuantityTickViolated", context);
      case formattable::TradedQuantityMinViolated:
        return base::format("TradedQuantityMinViolated", context);
      case formattable::TradedQuantityMaxViolated:
        return base::format("TradedQuantityMaxViolated", context);
    }

    return base::format("UnknownMarketDataValidationError", context);
  }
};

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_ERRORS_HPP_
