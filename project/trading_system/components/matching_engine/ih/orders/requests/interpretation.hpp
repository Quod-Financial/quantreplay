#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REQUESTS_INTERPRETATION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REQUESTS_INTERPRETATION_HPP_

#include <optional>
#include <string_view>
#include <tl/expected.hpp>
#include <variant>

#include "core/domain/attributes.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_metadata.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"

namespace simulator::trading_system::matching_engine {

enum class OrderRequestError : std::uint8_t {
  OrderTypeInvalid,
  OrderTypeMissing,
  SideInvalid,
  SideMissing,
  TimeInForceInvalid,
  PriceMissing,
  QuantityMissing,
  OrderIdInvalid,
};

using NewOrderInterpretation =
    std::variant<OrderRequestError, LimitOrder, MarketOrder>;
using UpdateInterpretation = std::variant<OrderRequestError, LimitUpdate>;
using CancelInterpretation = std::variant<OrderRequestError, OrderCancel>;

// Converts a placement request to an order interpretation.
// When interpretation succeeds, the value returned contains a concrete
// order type, which depends on OrderType value specified in the request.
// In case error occurs, the value returned contains an error,
// represented by an OrderRequestError enum value.
class PlacementInterpreter {
 public:
  PlacementInterpreter() = delete;
  explicit PlacementInterpreter(OrderId new_order_id);

  [[nodiscard]]
  auto interpret(const protocol::OrderPlacementRequest& request)
      -> NewOrderInterpretation;

 private:
  auto create_order_record(const protocol::OrderPlacementRequest& request,
                           OrderAttributes order_attributes) const
      -> tl::expected<OrderRecord, OrderRequestError>;

  auto interpret_as_limit_order(const protocol::OrderPlacementRequest& request)
      -> NewOrderInterpretation;

  auto interpret_as_market_order(const protocol::OrderPlacementRequest& request)
      -> NewOrderInterpretation;

  OrderId new_order_id_;
};

// Converts a modification request to an order update interpretation.
// When interpretation succeeds, the value returned contains a concrete
// order type update, which depends on OrderType value specified in the
// request. In case error occurs, the value returned contains an error,
// represented by an OrderRequestError enum value.
class ModificationInterpreter {
 public:
  [[nodiscard]]
  auto interpret(const protocol::OrderModificationRequest& request) const
      -> UpdateInterpretation;

 private:
  auto interpret_as_limit_update(
      const protocol::OrderModificationRequest& request) const
      -> UpdateInterpretation;
};

// Converts cancellation request to a cancel interpretation.
// When interpretation succeeds, the value returned contains
// an OrderCancel object.
// In case error occurs, the value returned contains an error,
// represented by an OrderRequestError enum value.
class CancellationInterpreter {
 public:
  [[nodiscard]]
  auto interpret(const protocol::OrderCancellationRequest& request) const
      -> CancelInterpretation;
};

[[nodiscard]]
auto describe(OrderRequestError error) -> std::string_view;

// Converts given error to a human-readable reject text.
// The std::invalid_argument is thrown if the error has an unexpected value.
[[nodiscard]]
auto convert_to_reason_text(OrderRequestError error) -> RejectText;

namespace detail {

[[nodiscard]]
auto interpret_side(std::optional<Side> side)
    -> tl::expected<Side, OrderRequestError>;

[[nodiscard]]
auto interpret_order_type(std::optional<OrderType> order_type)
    -> tl::expected<OrderType, OrderRequestError>;

[[nodiscard]]
auto interpret_time_in_force(std::optional<TimeInForce> time_in_force)
    -> tl::expected<TimeInForce, OrderRequestError>;

[[nodiscard]]
auto interpret_order_id(const std::optional<VenueOrderId>& venue_order_id)
    -> tl::expected<std::optional<OrderId>, OrderRequestError>;

struct OrderAttributesCreator {
  static auto create_from(const protocol::OrderPlacementRequest& request)
      -> tl::expected<OrderAttributes, OrderRequestError>;

  static auto create_from(const protocol::OrderModificationRequest& request)
      -> tl::expected<OrderAttributes, OrderRequestError>;

 private:
  template <typename RequestType>
  static auto create_attributes(const RequestType& request)
      -> tl::expected<OrderAttributes, OrderRequestError>;
};

}  // namespace detail
}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REQUESTS_INTERPRETATION_HPP_