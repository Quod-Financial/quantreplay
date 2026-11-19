#include "idgen/order_reject_ids.hpp"

#include <fmt/format.h>

#include <stdexcept>
#include <utility>

#include "common/attributes.hpp"
#include "idgen/execution_id.hpp"

namespace simulator::trading_system::idgen {

auto generate_order_reject_ids(OrderIdContext& ctx) -> OrderRejectIds {
  const OrderId order_id = generate_new_id(ctx);
  auto execution_id_context = make_execution_id_generation_ctx(order_id);
  auto execution_id = generate_new_id(execution_id_context);
  if (!execution_id.has_value()) {
    throw std::runtime_error{
        "failed to generate an execution identifier for an order reject"};
  }

  return OrderRejectIds{
      .venue_order_id = VenueOrderId{fmt::to_string(order_id)},
      .execution_id = *std::move(execution_id)};
}

}  // namespace simulator::trading_system::idgen
