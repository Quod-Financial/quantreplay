#include "ih/orders/tools/id_conversion.hpp"

#include <fmt/format.h>

#include <charconv>

#include "core/tools/string_checkers.hpp"

namespace simulator::trading_system::matching_engine::order {

auto stringify_order_id(OrderId order_id) -> std::string {
  return fmt::to_string(order_id);
}

auto parse_order_id(std::string_view source) -> std::optional<OrderId> {
  static_assert(std::is_same_v<OrderId::value_type, std::uint64_t>,
                "OrderId::value_type is expected to be std::uint64_t");

  if (!core::represents_base10_unsigned_int(source)) {
    return std::nullopt;
  }

  std::uint64_t identifier = 0;
  auto [_, err] = std::from_chars(source.data(), source.end(), identifier);
  return err == std::errc{} ? std::make_optional(OrderId{identifier})
                            : std::nullopt;
}

auto to_venue_order_id(OrderId order_id) -> VenueOrderId {
  return VenueOrderId{stringify_order_id(order_id)};
}

auto to_venue_order_id(std::optional<OrderId> order_id)
    -> std::optional<VenueOrderId> {
  return order_id.has_value() ? std::make_optional(to_venue_order_id(*order_id))
                              : std::nullopt;
}

auto to_order_id(const VenueOrderId& venue_order_id) -> std::optional<OrderId> {
  return parse_order_id(static_cast<const std::string&>(venue_order_id));
}

}  // namespace simulator::trading_system::matching_engine::order