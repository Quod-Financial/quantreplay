#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_ORDER_REJECT_IDS_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_ORDER_REJECT_IDS_HPP_

#include "core/domain/attributes.hpp"
#include "idgen/order_id.hpp"

namespace simulator::trading_system::idgen {

struct OrderRejectIds {
  VenueOrderId venue_order_id;
  ExecutionId execution_id;
};

[[nodiscard]]
auto generate_order_reject_ids(OrderIdContext& ctx) -> OrderRejectIds;

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_ORDER_REJECT_IDS_HPP_
