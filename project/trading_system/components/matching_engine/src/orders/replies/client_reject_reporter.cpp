#include "ih/orders/replies/client_reject_reporter.hpp"

#include <utility>

#include "common/order_rejects.hpp"
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

  // The order id is generated on ClientRejectReporter side.
  // However, the matching engine may already generate the order id before
  // rejecting a request, which means that we lost 2 order identifiers for this
  // rejected request.
  // We kept the order id generation in the ClientRejectReporter to simplify
  // the API, however this may need to be changed in the future.
  auto [order_id, exec_id] = make_aux_order_identifiers(order_id_generator_);

  emit(ClientNotification(
      make_reject(request, std::move(order_id), std::move(exec_id), reason)));
}

auto ClientRejectReporter::notify_rejected(
    const protocol::OrderModificationRequest& request, std::string_view reason)
    -> void {
  log::debug(
      "notifying the client that the order modification request has been "
      "rejected");

  emit(ClientNotification(make_reject(request, reason)));
}

auto ClientRejectReporter::notify_rejected(
    const protocol::OrderCancellationRequest& request, std::string_view reason)
    -> void {
  log::debug(
      "notifying the client that the order cancellation request has been "
      "rejected");

  emit(ClientNotification(make_reject(request, reason)));
}

}  // namespace simulator::trading_system::matching_engine::order