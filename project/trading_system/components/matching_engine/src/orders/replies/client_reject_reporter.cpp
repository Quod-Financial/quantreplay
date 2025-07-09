#include "ih/orders/replies/client_reject_reporter.hpp"

#include <string>
#include <utility>

#include "ih/common/events/client_notification.hpp"
#include "ih/orders/tools/exec_id_generator.hpp"
#include "ih/orders/tools/id_conversion.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine::order {

namespace {

auto make_aux_order_identifiers(OrderIdGenerator& order_id_generator)
    -> std::pair<VenueOrderId, ExecutionId> {
  const OrderId rejected_order_id = generate_order_id(order_id_generator);
  return std::make_pair(to_venue_order_id(rejected_order_id),
                        generate_aux_execution_id(rejected_order_id));
}

}  // namespace

ClientRejectReporter::ClientRejectReporter(EventListener& event_listener,
                                           OrderIdGenerator& order_id_generator)
    : EventReporter(event_listener), order_id_generator_(order_id_generator) {}

auto ClientRejectReporter::notify_rejected(
    const protocol::OrderPlacementRequest& request, std::string_view reason)
    -> void {
  log::debug(
      "notifying the client that the order placement request has been "
      "rejected");

  protocol::OrderPlacementReject reject{request.session};
  reject.instrument = request.instrument;
  reject.parties = request.parties;
  reject.client_order_id = request.client_order_id;
  reject.reject_text = RejectText(std::string(reason));
  reject.order_price = request.order_price;
  reject.order_quantity = request.order_quantity;
  reject.order_type = request.order_type;
  reject.side = request.side;
  reject.time_in_force = request.time_in_force;

  // The order id is generated on ClientRejectReporter side.
  // However, the matching engine may already generate the order id before
  // rejecting a request, which means that we lost 2 order identifiers for this
  // rejected request.
  // We kept the order id generation in the ClientRejectReporter to simplify
  // the API, however this may need to be changed in the future.
  auto [order_id, exec_id] = make_aux_order_identifiers(order_id_generator_);
  reject.venue_order_id = std::move(order_id);
  reject.execution_id = std::move(exec_id);

  emit(ClientNotification(std::move(reject)));
}

auto ClientRejectReporter::notify_rejected(
    const protocol::OrderModificationRequest& request, std::string_view reason)
    -> void {
  log::debug(
      "notifying the client that the order modification request has been "
      "rejected");

  protocol::OrderModificationReject reject{request.session};
  reject.client_order_id = request.client_order_id;
  reject.orig_client_order_id = request.orig_client_order_id;
  reject.venue_order_id = request.venue_order_id;
  reject.reject_text = RejectText(std::string(reason));
  reject.order_status = OrderStatus::Option::Rejected;

  emit(ClientNotification(std::move(reject)));
}

auto ClientRejectReporter::notify_rejected(
    const protocol::OrderCancellationRequest& request, std::string_view reason)
    -> void {
  log::debug(
      "notifying the client that the order cancellation request has been "
      "rejected");

  protocol::OrderCancellationReject reject{request.session};
  reject.orig_client_order_id = request.orig_client_order_id;
  reject.venue_order_id = request.venue_order_id;
  reject.client_order_id = request.client_order_id;
  reject.reject_text = RejectText(std::string(reason));
  reject.order_status = OrderStatus::Option::Rejected;

  emit(ClientNotification(std::move(reject)));
}

}  // namespace simulator::trading_system::matching_engine::order