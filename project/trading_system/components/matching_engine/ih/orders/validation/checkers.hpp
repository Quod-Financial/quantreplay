#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_CHECKERS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_CHECKERS_HPP_

#include <functional>
#include <optional>

#include "common/attributes.hpp"
#include "common/market_state/snapshot.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"
#include "ih/common/validation/checker_utils.hpp"
#include "ih/orders/validation/errors.hpp"
#include "ih/orders/validation/order_book_side.hpp"
#include "protocol/app/order_placement_request.hpp"

namespace simulator::trading_system::matching_engine::order {

using ValidationResult = std::optional<ValidationError>;

struct SideSpecified {
  auto operator()(const auto& request) -> ValidationResult {
    static_assert(
        requires { request.side; },
        "given type does not contain side field required by a matcher");

    return field_specified(request.side, ValidationError::SideMissing);
  }
};

struct SideSupported {
  auto operator()(const auto& request) -> ValidationResult {
    static_assert(
        requires { request.side; },
        "given type does not contain side field required by a matcher");

    return check(request.side);
  }

 private:
  static auto check(std::optional<Side> side) -> ValidationResult;
};

struct OrderSideSupported {
  explicit OrderSideSupported(OrderBookSide order_book_side)
      : order_book_side_(order_book_side) {}

  auto operator()(const market_state::LimitOrder& order) const
      -> ValidationResult;

 private:
  OrderBookSide order_book_side_;
};

struct OrderTypeSpecified {
  auto operator()(const auto& request) -> ValidationResult {
    static_assert(
        requires { request.order_type; },
        "given type does not contain order_type field required by a matcher");

    return field_specified(request.order_type,
                           ValidationError::OrderTypeMissing);
  }
};

struct OrderTypeSupported {
  auto operator()(const auto& request) -> ValidationResult {
    static_assert(
        requires { request.order_type; },
        "given type does not contain order_type field required by a matcher");

    return check(request.order_type);
  }

 private:
  static auto check(std::optional<OrderType> order_type) -> ValidationResult;
};

struct OrderStatusSupported {
  auto operator()(const market_state::LimitOrder& order) const
      -> ValidationResult;
};

struct OrderQuantitySpecified {
  auto operator()(const auto& request) -> ValidationResult {
    static_assert(
        requires { request.order_quantity; },
        "given type does not contain quantity field required by a matcher");

    return field_specified(request.order_quantity,
                           ValidationError::OrderQuantityMissing);
  }
};

struct OrderQuantityRespectsMinimum {
  explicit OrderQuantityRespectsMinimum(std::optional<MinQuantity> min)
      : min_(min) {}

  auto operator()(const auto& request) -> ValidationResult {
    static_assert(
        requires { request.order_quantity; },
        "given type does not contain quantity field required by a matcher");

    return field_respects_minimum(request.order_quantity,
                                  min_,
                                  ValidationError::OrderQuantityMinViolated);
  }

 private:
  std::optional<MinQuantity> min_;
};

struct OrderQuantityRespectsMaximum {
  explicit OrderQuantityRespectsMaximum(std::optional<MaxQuantity> max)
      : max_(max) {}

  auto operator()(const auto& request) -> ValidationResult {
    static_assert(
        requires { request.order_quantity; },
        "given type does not contain quantity field required by a matcher");

    return field_respects_maximum(request.order_quantity,
                                  max_,
                                  ValidationError::OrderQuantityMaxViolated);
  }

 private:
  std::optional<MaxQuantity> max_;
};

struct OrderQuantityRespectsTick {
  explicit OrderQuantityRespectsTick(std::optional<QuantityTick> tick)
      : tick_(tick) {}

  auto operator()(const auto& request) -> ValidationResult {
    static_assert(
        requires { request.order_quantity; },
        "given type does not contain quantity field required by a matcher");

    return field_respects_tick(request.order_quantity,
                               tick_,
                               ValidationError::OrderQuantityTickViolated);
  }

 private:
  std::optional<QuantityTick> tick_;
};

struct TotalQuantityRespectsMinimum {
  explicit TotalQuantityRespectsMinimum(std::optional<MinQuantity> min)
      : min_{min} {}

  auto operator()(const market_state::LimitOrder& request) const
      -> ValidationResult;

 private:
  std::optional<MinQuantity> min_;
};

struct TotalQuantityRespectsMaximum {
  explicit TotalQuantityRespectsMaximum(std::optional<MaxQuantity> max)
      : max_{max} {}

  auto operator()(const market_state::LimitOrder& order) const
      -> ValidationResult;

 private:
  std::optional<MaxQuantity> max_;
};

struct TotalQuantityRespectsTick {
  explicit TotalQuantityRespectsTick(std::optional<QuantityTick> tick)
      : tick_{tick} {}

  auto operator()(const market_state::LimitOrder& order) const
      -> ValidationResult;

 private:
  std::optional<QuantityTick> tick_;
};

struct CumExecutedQuantityRespectsNonNegativity {
  auto operator()(const market_state::LimitOrder& order) const
      -> ValidationResult;
};

struct CumExecutedQuantityRespectsTick {
  explicit CumExecutedQuantityRespectsTick(std::optional<QuantityTick> tick)
      : tick_{tick} {}

  auto operator()(const market_state::LimitOrder& order) const
      -> ValidationResult;

 private:
  std::optional<QuantityTick> tick_;
};

struct CumExecutedQuantityIsLessThanTotalQuantity {
  auto operator()(const market_state::LimitOrder& order) const
      -> ValidationResult;
};

struct OrderPriceSpecified {
  auto operator()(const auto& request) -> ValidationResult {
    static_assert(
        requires { request.order_price; },
        "given type does not contain price field required by a matcher");

    return field_specified(request.order_price,
                           ValidationError::OrderPriceMissing);
  }
};

struct OrderPriceAbsent {
  auto operator()(const auto& request) -> ValidationResult {
    static_assert(
        requires { request.order_price; },
        "given type does not contain price field required by a matcher");

    return check(request.order_price);
  }

 private:
  static auto check(std::optional<OrderPrice> price) -> ValidationResult;
};

struct OrderPriceRespectsTick {
  explicit OrderPriceRespectsTick(std::optional<PriceTick> tick)
      : tick_(tick) {}

  auto operator()(const auto& request) -> ValidationResult {
    static_assert(
        requires { request.order_price; },
        "given type does not contain price field required by a matcher");

    return field_respects_tick(
        request.order_price, tick_, ValidationError::OrderPriceTickViolated);
  }

 private:
  std::optional<PriceTick> tick_;
};

struct TimeInForceSupported {
  auto operator()(const market_state::LimitOrder& order) const
      -> ValidationResult;
};

struct OrderExpireInfoSpecified {
  auto operator()(const auto& request) const -> ValidationResult {
    static_assert(
        requires {
          request.expire_time;
          request.expire_date;
        }, "given type does not contain fields required by a matcher");

    return check(request.expire_time, request.expire_date);
  }

 private:
  static auto check(const std::optional<ExpireTime>& expire_time,
                    const std::optional<ExpireDate>& expire_date)
      -> ValidationResult;
};

struct OrderNotExpired {
  explicit OrderNotExpired(core::TzClock clock) noexcept;

  auto operator()(const auto& request) const -> ValidationResult {
    static_assert(
        requires {
          request.expire_time;
          request.expire_date;
        }, "given type does not contain fields required by a matcher");

    ValidationResult result;
    if (request.expire_time.has_value()) {
      result = check_not_expired(*request.expire_time);
    }
    if (request.expire_date.has_value()) {
      result = check_not_expired(*request.expire_date);
    }
    return result;
  }

 private:
  static auto check_not_expired(const ExpireTime& timestamp)
      -> ValidationResult;

  auto check_not_expired(const ExpireDate& date) const -> ValidationResult;

  core::TzClock clock_;
};

struct DayOrderNotExpired {
  auto operator()(const market_state::LimitOrder& order) const
      -> ValidationResult;
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_CHECKERS_HPP_
