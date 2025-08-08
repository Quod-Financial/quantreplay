#include "ih/orders/requests/interpretation.hpp"

#include <optional>
#include <stdexcept>
#include <string_view>
#include <tl/expected.hpp>

#include "core/common/unreachable.hpp"
#include "core/domain/attributes.hpp"
#include "ih/orders/book/order_metadata.hpp"
#include "ih/orders/tools/id_conversion.hpp"

namespace simulator::trading_system::matching_engine {
namespace detail {

auto interpret_side(std::optional<Side> side)
    -> tl::expected<Side, OrderRequestError> {
  if (!side.has_value()) {
    return tl::make_unexpected(OrderRequestError::SideMissing);
  }
  switch (static_cast<Side::Option>(*side)) {
    case Side::Option::Buy:
    case Side::Option::Sell:
    case Side::Option::SellShort:
    case Side::Option::SellShortExempt:
      return *side;
  }
  return tl::make_unexpected(OrderRequestError::SideInvalid);
}

auto interpret_order_type(std::optional<OrderType> order_type)
    -> tl::expected<OrderType, OrderRequestError> {
  if (order_type == OrderType::Option::Limit ||
      order_type == OrderType::Option::Market) [[likely]] {
    return *order_type;
  }

  return order_type.has_value()
             ? tl::make_unexpected(OrderRequestError::OrderTypeInvalid)
             : tl::make_unexpected(OrderRequestError::OrderTypeMissing);
}

auto interpret_time_in_force(std::optional<TimeInForce> time_in_force)
    -> tl::expected<TimeInForce, OrderRequestError> {
  if (!time_in_force.has_value()) {
    return TimeInForce::Option::Day;
  }

  switch (static_cast<TimeInForce::Option>(*time_in_force)) {
    case TimeInForce::Option::Day:
    case TimeInForce::Option::ImmediateOrCancel:
    case TimeInForce::Option::FillOrKill:
    case TimeInForce::Option::GoodTillDate:
    case TimeInForce::Option::GoodTillCancel:
      return *time_in_force;
  }

  return tl::make_unexpected(OrderRequestError::TimeInForceInvalid);
}

auto interpret_order_id(const std::optional<VenueOrderId>& venue_order_id)
    -> tl::expected<std::optional<OrderId>, OrderRequestError> {
  if (!venue_order_id.has_value()) {
    return std::nullopt;
  }

  const auto& identifier = static_cast<const std::string&>(*venue_order_id);
  if (const auto conv = order::parse_order_id(identifier)) {
    return conv;
  }
  return tl::make_unexpected(OrderRequestError::OrderIdInvalid);
}

// region OrderAttributesCreator

auto OrderAttributesCreator::create_from(
    const protocol::OrderPlacementRequest& request)
    -> tl::expected<OrderAttributes, OrderRequestError> {
  return create_attributes(request);
}

auto OrderAttributesCreator::create_from(
    const protocol::OrderModificationRequest& request)
    -> tl::expected<OrderAttributes, OrderRequestError> {
  return create_attributes(request);
}

template <typename RequestType>
auto OrderAttributesCreator::create_attributes(const RequestType& request)
    -> tl::expected<OrderAttributes, OrderRequestError> {
  const auto time_in_force = interpret_time_in_force(request.time_in_force);
  if (!time_in_force.has_value()) {
    return tl::make_unexpected(time_in_force.error());
  }

  OrderAttributes attributes;
  attributes.set_time_in_force(*time_in_force);
  attributes.set_order_parties(request.parties);
  if (request.client_order_id.has_value()) {
    attributes.set_client_order_id(*request.client_order_id);
  }
  if (request.short_sale_exempt_reason.has_value()) {
    attributes.set_short_sale_exemption_reason(
        *request.short_sale_exempt_reason);
  }
  if (request.expire_time.has_value()) {
    attributes.set_expire_time(*request.expire_time);
  }
  if (request.expire_date.has_value()) {
    attributes.set_expire_date(*request.expire_date);
  }
  return attributes;
}

// endregion OrderAttributesCreator

}  // namespace detail

// region PlacementInterpreter

PlacementInterpreter::PlacementInterpreter(OrderId new_order_id)
    : new_order_id_(new_order_id) {}

auto PlacementInterpreter::interpret(
    const protocol::OrderPlacementRequest& request) -> NewOrderInterpretation {
  const auto order_type = detail::interpret_order_type(request.order_type);
  if (!order_type.has_value()) {
    return order_type.error();
  }

  switch (order_type->value()) {
    case OrderType::Option::Limit:
      return interpret_as_limit_order(request);
    case OrderType::Option::Market:
      return interpret_as_market_order(request);
  }

  // The execution must not reach this point, as this means that
  // interpret_order_type returned an undefined OrderType value.
  core::unreachable();
}

auto PlacementInterpreter::create_order_record(
    const protocol::OrderPlacementRequest& request,
    OrderAttributes order_attributes) const
    -> tl::expected<OrderRecord, OrderRequestError> {
  const auto side = detail::interpret_side(request.side);
  if (!side.has_value()) {
    return tl::make_unexpected(side.error());
  }

  return OrderRecord{new_order_id_,
                     *side,
                     request.session,
                     request.instrument,
                     std::move(order_attributes)};
}

auto PlacementInterpreter::interpret_as_limit_order(
    const protocol::OrderPlacementRequest& request) -> NewOrderInterpretation {
  auto attributes = detail::OrderAttributesCreator::create_from(request);
  if (!attributes.has_value()) {
    return attributes.error();
  }

  auto order_record = create_order_record(request, std::move(*attributes));
  if (!order_record.has_value()) {
    return order_record.error();
  }

  if (!request.order_price.has_value()) {
    return OrderRequestError::PriceMissing;
  }
  if (!request.order_quantity.has_value()) {
    return OrderRequestError::QuantityMissing;
  }
  return LimitOrder{request.order_price.value(),
                    request.order_quantity.value(),
                    std::move(order_record.value())};
}

auto PlacementInterpreter::interpret_as_market_order(
    const protocol::OrderPlacementRequest& request) -> NewOrderInterpretation {
  auto attributes = detail::OrderAttributesCreator::create_from(request);
  if (!attributes.has_value()) {
    return attributes.error();
  }
  // Implicitly interpret all market orders as IoC orders
  attributes->set_time_in_force(TimeInForce::Option::ImmediateOrCancel);

  auto order_record = create_order_record(request, std::move(*attributes));
  if (!order_record.has_value()) {
    return order_record.error();
  }
  if (!request.order_quantity.has_value()) {
    return OrderRequestError::QuantityMissing;
  }

  return MarketOrder{request.order_quantity.value(),
                     std::move(order_record.value())};
}

// endregion PlacementInterpreter

// region ModificationInterpreter

auto ModificationInterpreter::interpret(
    const protocol::OrderModificationRequest& request) const
    -> UpdateInterpretation {
  const auto order_type = detail::interpret_order_type(request.order_type);
  if (!order_type.has_value()) {
    return order_type.error();
  }
  if (order_type->value() != OrderType::Option::Limit) {
    // Non-limit orders can not be amended in the matching engine
    return OrderRequestError::OrderTypeInvalid;
  }

  return interpret_as_limit_update(request);
}

auto ModificationInterpreter::interpret_as_limit_update(
    const protocol::OrderModificationRequest& request) const
    -> UpdateInterpretation {
  const auto side = detail::interpret_side(request.side);
  if (!side.has_value()) {
    return side.error();
  }

  const auto order_id = detail::interpret_order_id(request.venue_order_id);
  if (!order_id.has_value()) {
    return order_id.error();
  }

  auto attributes = detail::OrderAttributesCreator::create_from(request);
  if (!attributes.has_value()) {
    return attributes.error();
  }

  if (!request.order_price.has_value()) {
    return OrderRequestError::PriceMissing;
  }
  if (!request.order_quantity.has_value()) {
    return OrderRequestError::QuantityMissing;
  }

  LimitUpdate update{request.session,
                     side.value(),
                     LimitOrder::Update{.price = *request.order_price,
                                        .quantity = *request.order_quantity,
                                        .attributes = std::move(*attributes)}};

  update.order_id = order_id.value();
  update.client_order_id = request.client_order_id;
  update.orig_client_order_id = request.orig_client_order_id;

  return update;
}

// endregion ModificationInterpreter

// region CancellationInterpreter

auto CancellationInterpreter::interpret(
    const protocol::OrderCancellationRequest& request) const
    -> CancelInterpretation {
  const auto side = detail::interpret_side(request.side);
  if (!side.has_value()) {
    return side.error();
  }

  const auto order_id = detail::interpret_order_id(request.venue_order_id);
  if (!order_id.has_value()) {
    return order_id.error();
  }

  OrderCancel action{request.session, side.value()};
  action.order_id = order_id.value();
  action.client_order_id = request.client_order_id;
  action.orig_client_order_id = request.orig_client_order_id;
  return action;
}

// endregion CancellationInterpreter

[[nodiscard]]
auto describe(OrderRequestError error) -> std::string_view {
  switch (error) {
    case OrderRequestError::OrderTypeInvalid:
      return "unknown order type";
    case OrderRequestError::OrderTypeMissing:
      return "order type missing";
    case OrderRequestError::SideInvalid:
      return "unknown order side";
    case OrderRequestError::SideMissing:
      return "order side missing";
    case OrderRequestError::TimeInForceInvalid:
      return "unknown time in force";
    case OrderRequestError::PriceMissing:
      return "order price missing";
    case OrderRequestError::QuantityMissing:
      return "order quantity missing";
    case OrderRequestError::OrderIdInvalid:
      return "invalid order identifier format";
  }

  throw std::invalid_argument(fmt::format(
      "cannot describe unsupported OrderRequestError value '0x{:x}' ",
      core::underlying_cast(error)));
}

auto convert_to_reason_text(OrderRequestError error) -> RejectText {
  return RejectText(std::string(describe(error)));
}

}  // namespace simulator::trading_system::matching_engine