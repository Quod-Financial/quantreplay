#include "common/order_rejects.hpp"

#include <string>
#include <utility>

#include "core/domain/attributes.hpp"

namespace simulator::trading_system {

auto make_reject(const protocol::OrderPlacementRequest& request,
                 VenueOrderId venue_order_id,
                 ExecutionId execution_id,
                 std::string_view reason) -> protocol::OrderPlacementReject {
  protocol::OrderPlacementReject reject{request.session};
  reject.instrument = request.instrument;
  reject.parties = request.parties;
  reject.client_order_id = request.client_order_id;
  reject.venue_order_id = std::move(venue_order_id);
  reject.execution_id = std::move(execution_id);
  reject.reject_text = RejectText{std::string{reason}};
  reject.order_price = request.order_price;
  reject.order_quantity = request.order_quantity;
  reject.order_type = request.order_type;
  reject.side = request.side;
  reject.time_in_force = request.time_in_force;
  return reject;
}

auto make_reject(const protocol::OrderModificationRequest& request,
                 std::string_view reason) -> protocol::OrderModificationReject {
  protocol::OrderModificationReject reject{request.session};
  reject.client_order_id = request.client_order_id;
  reject.orig_client_order_id = request.orig_client_order_id;
  reject.venue_order_id = request.venue_order_id;
  reject.reject_text = RejectText{std::string{reason}};
  reject.order_status = OrderStatus::Option::Rejected;
  return reject;
}

auto make_reject(const protocol::OrderCancellationRequest& request,
                 std::string_view reason) -> protocol::OrderCancellationReject {
  protocol::OrderCancellationReject reject{request.session};
  reject.client_order_id = request.client_order_id;
  reject.orig_client_order_id = request.orig_client_order_id;
  reject.venue_order_id = request.venue_order_id;
  reject.reject_text = RejectText{std::string{reason}};
  reject.order_status = OrderStatus::Option::Rejected;
  return reject;
}

}  // namespace simulator::trading_system
