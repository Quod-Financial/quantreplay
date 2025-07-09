#include "ih/orders/validation/client_request_validator.hpp"

#include <functional>
#include <optional>

#include "core/common/std_formatter.hpp"
#include "ih/orders/validation/checkers.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine::order {

auto ClientRequestValidator::configure(Config config) -> void {
  config_ = std::move(config);
}

auto ClientRequestValidator::validate(
    const protocol::OrderPlacementRequest& request) const -> Conclusion {
  log::debug("validating order placement request");

  Validation validation{request, describe};
  run(validation);
  auto conclusion = validation.successful()
                        ? Conclusion::success()
                        : Conclusion::failure(validation.error_description());

  if (conclusion.failed()) {
    log::debug(
        "validation failed with '{}' error, request was considered as invalid: "
        "{}",
        validation.error(),
        request);
  }

  return conclusion;
}

auto ClientRequestValidator::validate(
    const protocol::OrderModificationRequest& request) const -> Conclusion {
  log::debug("validating order modification request");

  Validation validation{request, describe};
  run(validation);
  auto conclusion = validation.successful()
                        ? Conclusion::success()
                        : Conclusion::failure(validation.error_description());

  if (conclusion.failed()) {
    log::debug(
        "validation failed with '{}' error, request was considered as invalid: "
        "{}",
        validation.error(),
        request);
  }

  return conclusion;
}

auto ClientRequestValidator::validate(
    const protocol::OrderCancellationRequest& request) const -> Conclusion {
  log::debug("validating order cancellation request");

  Validation validation{request, describe};
  run(validation);
  auto conclusion = validation.successful()
                        ? Conclusion::success()
                        : Conclusion::failure(validation.error_description());

  if (conclusion.failed()) {
    log::debug(
        "validation failed with '{}' error, request was considered as invalid: "
        "{}",
        validation.error(),
        request);
  }

  return conclusion;
}

auto ClientRequestValidator::validate(const market_state::LimitOrder& order,
                                      OrderBookSide order_book_side) const
    -> Conclusion {
  log::debug("validating market_state::LimitOrder");

  Validation validation{order, describe};
  run(validation, order_book_side);
  auto conclusion = validation.successful()
                        ? Conclusion::success()
                        : Conclusion::failure(validation.error_description());

  if (conclusion.failed()) {
    log::debug(
        "validation failed with '{}' error, market_state::LimitOrder "
        "was considered as invalid: {}",
        validation.error(),
        order);
  }

  return conclusion;
}

auto ClientRequestValidator::run(
    Validation<protocol::OrderPlacementRequest>& validation) const -> void {
  const protocol::OrderPlacementRequest& request = validation.request();

  validation.expect(SideSpecified())
      .expect(SideSupported())
      .expect(OrderTypeSpecified())
      .expect(OrderTypeSupported())
      .expect(OrderQuantitySpecified())
      .expect(OrderQuantityRespectsMinimum(config_.min_quantity))
      .expect(OrderQuantityRespectsMaximum(config_.max_quantity))
      .expect(OrderQuantityRespectsTick(config_.quantity_tick));

  if (request.order_type == OrderType::Option::Limit) {
    validation.expect(OrderPriceSpecified())
        .expect(OrderPriceRespectsTick(config_.price_tick));
  } else if (request.order_type == OrderType::Option::Market) {
    validation.expect(OrderPriceAbsent());
  }

  if (request.time_in_force == TimeInForce::Option::GoodTillDate) {
    validation.expect(OrderExpireInfoSpecified())
        .expect(OrderNotExpired(config_.clock));
  }
}

auto ClientRequestValidator::run(
    Validation<protocol::OrderModificationRequest>& validation) const -> void {
  const protocol::OrderModificationRequest& request = validation.request();

  validation.expect(SideSpecified())
      .expect(SideSupported())
      .expect(OrderTypeSpecified())
      .expect(OrderTypeSupported())
      .expect(OrderQuantitySpecified())
      .expect(OrderQuantityRespectsMinimum(config_.min_quantity))
      .expect(OrderQuantityRespectsMaximum(config_.max_quantity))
      .expect(OrderQuantityRespectsTick(config_.quantity_tick));

  if (request.order_type == OrderType::Option::Limit) {
    validation.expect(OrderPriceSpecified())
        .expect(OrderPriceRespectsTick(config_.price_tick));
  } else if (request.order_type == OrderType::Option::Market) {
    validation.expect(OrderPriceAbsent());
  }

  if (request.time_in_force == TimeInForce::Option::GoodTillDate) {
    validation.expect(OrderExpireInfoSpecified())
        .expect(OrderNotExpired(config_.clock));
  }
}

auto ClientRequestValidator::run(
    Validation<protocol::OrderCancellationRequest>& validation) const -> void {
  validation.expect(SideSpecified()).expect(SideSupported());
}

auto ClientRequestValidator::run(
    Validation<market_state::LimitOrder>& validation,
    OrderBookSide order_book_side) const -> void {
  validation.expect(OrderSideSupported{order_book_side})
      .expect(TotalQuantityRespectsMinimum{config_.min_quantity})
      .expect(TotalQuantityRespectsMaximum{config_.max_quantity})
      .expect(TotalQuantityRespectsTick{config_.quantity_tick})
      .expect(CumExecutedQuantityRespectsNonNegativity{})
      .expect(CumExecutedQuantityRespectsTick{config_.quantity_tick})
      .expect(CumExecutedQuantityIsLessThanTotalQuantity{})
      .expect(OrderPriceRespectsTick{config_.price_tick})
      .expect(OrderStatusSupported{})
      .expect(TimeInForceSupported{});

  const market_state::LimitOrder& order = validation.request();
  if (order.time_in_force == TimeInForce::Option::Day) {
    validation.expect(DayOrderNotExpired());
  }
  if (order.time_in_force == TimeInForce::Option::GoodTillDate) {
    validation.expect(OrderExpireInfoSpecified())
        .expect(OrderNotExpired(config_.clock));
  }
}

}  // namespace simulator::trading_system::matching_engine::order