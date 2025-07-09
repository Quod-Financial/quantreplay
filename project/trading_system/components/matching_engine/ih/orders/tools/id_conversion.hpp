#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_ID_CONVERSION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_ID_CONVERSION_HPP_

#include <optional>
#include <string>
#include <string_view>

#include "common/attributes.hpp"
#include "core/domain/attributes.hpp"

namespace simulator::trading_system::matching_engine::order {

[[nodiscard]]
auto stringify_order_id(OrderId order_id) -> std::string;

[[nodiscard]]
auto parse_order_id(std::string_view source) -> std::optional<OrderId>;

[[nodiscard]]
auto to_venue_order_id(OrderId order_id) -> VenueOrderId;

[[nodiscard]]
auto to_venue_order_id(std::optional<OrderId> order_id)
    -> std::optional<VenueOrderId>;

[[nodiscard]]
auto to_order_id(const VenueOrderId& venue_order_id) -> std::optional<OrderId>;

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_ID_CONVERSION_HPP_
