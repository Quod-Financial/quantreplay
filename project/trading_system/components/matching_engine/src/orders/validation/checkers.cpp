#include "ih/orders/validation/checkers.hpp"

#include <optional>

#include "core/tools/time.hpp"

namespace simulator::trading_system::matching_engine::order {

auto SideSupported::check(std::optional<Side> side) -> ValidationResult {
  std::optional<ValidationError> verr;
  if (side.has_value()) {
    const auto value = *side;
    const bool supported = value == Side::Option::Buy ||
                           value == Side::Option::Sell ||
                           value == Side::Option::SellShort ||
                           value == Side::Option::SellShortExempt;
    supported ? verr = std::nullopt
              : verr = std::make_optional(ValidationError::SideUnknown);
  }
  return verr;
}

auto OrderSideSupported::operator()(const market_state::LimitOrder& order) const
    -> ValidationResult {
  const auto side = order.side;
  switch (order_book_side_) {
    case OrderBookSide::Buy:
      return side == Side::Option::Buy
                 ? std::nullopt
                 : std::make_optional(ValidationError::SideInvalid);
    case OrderBookSide::Sell: {
      const bool supported = side == Side::Option::Sell ||
                             side == Side::Option::SellShort ||
                             side == Side::Option::SellShortExempt;
      return supported ? std::nullopt
                       : std::make_optional(ValidationError::SideInvalid);
    }
  }
  return std::nullopt;
}

auto OrderTypeSupported::check(std::optional<OrderType> order_type)
    -> ValidationResult {
  const bool supported = !order_type.has_value() ||
                         order_type == OrderType::Option::Limit ||
                         order_type == OrderType::Option::Market;

  return supported ? std::nullopt
                   : std::make_optional(ValidationError::OrderTypeUnknown);
}

auto OrderStatusSupported::operator()(
    const market_state::LimitOrder& order) const -> ValidationResult {
  const auto order_status = order.order_status;
  const bool supported = order_status == OrderStatus::Option::New ||
                         order_status == OrderStatus::Option::PartiallyFilled ||
                         order_status == OrderStatus::Option::Modified;

  return supported ? std::nullopt
                   : std::make_optional(ValidationError::OrderStatusUnknown);
}

auto TotalQuantityRespectsMinimum::operator()(
    const market_state::LimitOrder& request) const -> ValidationResult {
  return field_respects_minimum(
      request.total_quantity, min_, ValidationError::TotalQuantityMinViolated);
}

auto TotalQuantityRespectsMaximum::operator()(
    const market_state::LimitOrder& order) const -> ValidationResult {
  return field_respects_maximum(
      order.total_quantity, max_, ValidationError::TotalQuantityMaxViolated);
}

auto TotalQuantityRespectsTick::operator()(
    const market_state::LimitOrder& order) const -> ValidationResult {
  return field_respects_tick(
      order.total_quantity, tick_, ValidationError::TotalQuantityTickViolated);
}

auto CumExecutedQuantityRespectsNonNegativity::operator()(
    const market_state::LimitOrder& order) const -> ValidationResult {
  return fields_respect_order(
      order.cum_executed_quantity,
      0.,
      std::greater_equal{},
      ValidationError::CumExecutedQuantityNonNegativityViolated);
}

auto CumExecutedQuantityRespectsTick::operator()(
    const market_state::LimitOrder& order) const -> ValidationResult {
  return field_respects_tick(order.cum_executed_quantity,
                             tick_,
                             ValidationError::CumExecutedQuantityTickViolated);
}

auto CumExecutedQuantityIsLessThanTotalQuantity::operator()(
    const market_state::LimitOrder& order) const -> ValidationResult {
  return fields_respect_order(
      order.cum_executed_quantity,
      order.total_quantity,
      std::less{},
      ValidationError::CumExecutedQuantityIsLessThanTotalQuantityViolated);
}

auto OrderPriceAbsent::check(std::optional<OrderPrice> price)
    -> ValidationResult {
  return !price.has_value()
             ? std::nullopt
             : std::make_optional(ValidationError::OrderPriceNotAllowed);
}

auto TimeInForceSupported::operator()(
    const market_state::LimitOrder& order) const -> ValidationResult {
  const auto time_in_force = order.time_in_force;
  const bool supported = time_in_force == TimeInForce::Option::Day ||
                         time_in_force == TimeInForce::Option::GoodTillDate ||
                         time_in_force == TimeInForce::Option::GoodTillCancel;

  return supported ? std::nullopt
                   : std::make_optional(ValidationError::TimeInForceInvalid);
}

auto OrderExpireInfoSpecified::check(
    const std::optional<ExpireTime>& expire_time,
    const std::optional<ExpireDate>& expire_date) -> ValidationResult {
  std::optional<ValidationError> verr;
  if (expire_time.has_value() && expire_date.has_value()) {
    verr = ValidationError::BothExpireDateTimeSpecified;
  } else if (!expire_time.has_value() && !expire_date.has_value()) {
    verr = ValidationError::ExpireDateTimeMissing;
  }
  return verr;
}

OrderNotExpired::OrderNotExpired(core::TzClock clock) noexcept
    : clock_(std::move(clock)) {}

auto OrderNotExpired::check_not_expired(const ExpireTime& timestamp)
    -> ValidationResult {
  const auto now = core::get_current_system_time();
  return fields_respect_order(
      timestamp, now, std::greater{}, ValidationError::OrderAlreadyExpired);
}

auto OrderNotExpired::check_not_expired(const ExpireDate& date) const
    -> ValidationResult {
  const auto today = core::as_local_time(core::get_current_tz_date(clock_));
  return fields_respect_order(
      date, today, std::greater_equal{}, ValidationError::OrderAlreadyExpired);
}

auto DayOrderNotExpired::operator()(const market_state::LimitOrder& order) const
    -> ValidationResult {
  const auto today = core::to_time(core::get_current_system_date());
  return fields_respect_order(order.order_time,
                              today,
                              std::greater_equal{},
                              ValidationError::OrderAlreadyExpired);
}

}  // namespace simulator::trading_system::matching_engine::order