#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_ORDER_REJECTS_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_ORDER_REJECTS_HPP_

#include <string_view>

#include "core/domain/attributes.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/order_placement_request.hpp"

namespace simulator::trading_system {

[[nodiscard]]
auto make_reject(const protocol::OrderPlacementRequest& request,
                 VenueOrderId venue_order_id,
                 ExecutionId execution_id,
                 std::string_view reason) -> protocol::OrderPlacementReject;

[[nodiscard]]
auto make_reject(const protocol::OrderModificationRequest& request,
                 std::string_view reason) -> protocol::OrderModificationReject;

[[nodiscard]]
auto make_reject(const protocol::OrderCancellationRequest& request,
                 std::string_view reason) -> protocol::OrderCancellationReject;

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_ORDER_REJECTS_HPP_
