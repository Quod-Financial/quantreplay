#include <utility>

#include "ih/orders/replies/cancellation_reply_builders.hpp"
#include "ih/orders/replies/execution_reply_builders.hpp"
#include "ih/orders/replies/modification_reply_builders.hpp"
#include "ih/orders/replies/placement_reply_builders.hpp"
#include "ih/orders/tools/exec_id_generator.hpp"
#include "ih/orders/tools/id_conversion.hpp"

namespace simulator::trading_system::matching_engine {

CancellationConfirmationBuilder::CancellationConfirmationBuilder(
    protocol::Session session)
    : message_(std::move(session)) {}

auto CancellationConfirmationBuilder::build() const
    -> protocol::OrderCancellationConfirmation {
  return message_;
}

auto CancellationConfirmationBuilder::for_order(const LimitOrder& order)
    -> CancellationConfirmationBuilder& {
  message_.instrument = order.instrument();
  message_.parties = order.attributes().order_parties();
  message_.venue_order_id = order::to_venue_order_id(order.id());
  message_.leaving_quantity = order.leaves_quantity();
  message_.cum_executed_quantity = order.cum_executed_quantity();
  message_.order_price = order.price();
  message_.order_status = order.status();
  message_.side = order.side();
  message_.time_in_force = order.time_in_force();
  message_.short_sale_exempt_reason = order.short_sale_exemption_reason();
  message_.expire_time = order.expire_time();
  message_.expire_date = order.expire_date();
  message_.order_type = OrderType::Option::Limit;
  return *this;
}

auto CancellationConfirmationBuilder::with_execution_id(ExecutionId identifier)
    -> CancellationConfirmationBuilder& {
  message_.execution_id = std::move(identifier);
  return *this;
}

auto CancellationConfirmationBuilder::with_client_order_id(
    std::optional<ClientOrderId> identifier)
    -> CancellationConfirmationBuilder& {
  message_.client_order_id = std::move(identifier);
  return *this;
}

auto CancellationConfirmationBuilder::with_orig_client_order_id(
    std::optional<OrigClientOrderId> identifier)
    -> CancellationConfirmationBuilder& {
  message_.orig_client_order_id = std::move(identifier);
  return *this;
}

auto prepare_cancellation_confirmation(const LimitOrder& order)
    -> CancellationConfirmationBuilder {
  CancellationConfirmationBuilder builder{order.client_session()};
  builder.for_order(order);
  return builder;
}

CancellationRejectBuilder::CancellationRejectBuilder(protocol::Session session)
    : message_(std::move(session)) {
  message_.order_status = OrderStatus::Option::Rejected;
}

auto CancellationRejectBuilder::build() const
    -> protocol::OrderCancellationReject {
  return message_;
}

auto CancellationRejectBuilder::for_request(
    const protocol::OrderCancellationRequest& request)
    -> CancellationRejectBuilder& {
  message_.client_order_id = request.client_order_id;
  message_.orig_client_order_id = request.orig_client_order_id;
  message_.venue_order_id = request.venue_order_id;
  return *this;
}

auto CancellationRejectBuilder::with_reason(RejectText reason)
    -> CancellationRejectBuilder& {
  message_.reject_text = std::move(reason);
  return *this;
}

auto CancellationRejectBuilder::with_order_status(OrderStatus status)
    -> CancellationRejectBuilder& {
  message_.order_status = status;
  return *this;
}

auto CancellationRejectBuilder::with_order_id(std::optional<OrderId> identifier)
    -> CancellationRejectBuilder& {
  message_.venue_order_id = order::to_venue_order_id(identifier);
  return *this;
}

auto CancellationRejectBuilder::with_client_order_id(
    std::optional<ClientOrderId> identifier) -> CancellationRejectBuilder& {
  message_.client_order_id = std::move(identifier);
  return *this;
}

auto CancellationRejectBuilder::with_orig_client_order_id(
    std::optional<OrigClientOrderId> identifier) -> CancellationRejectBuilder& {
  message_.orig_client_order_id = std::move(identifier);
  return *this;
}

auto prepare_cancellation_reject(const protocol::OrderCancellationRequest&
                                     request) -> CancellationRejectBuilder {
  CancellationRejectBuilder builder{request.session};
  builder.for_request(request);
  return builder;
}

auto prepare_cancellation_reject(const OrderCancel& cancel)
    -> CancellationRejectBuilder {
  CancellationRejectBuilder builder{cancel.client_session};
  builder.with_order_id(cancel.order_id)
      .with_client_order_id(cancel.client_order_id)
      .with_orig_client_order_id(cancel.orig_client_order_id);
  return builder;
}

ExecutionReportBuilder::ExecutionReportBuilder(protocol::Session session)
    : message_(std::move(session)) {
  message_.execution_type = ExecutionType::Option::OrderTraded;
}

auto ExecutionReportBuilder::build() const -> protocol::ExecutionReport {
  return message_;
}

auto ExecutionReportBuilder::for_order(const LimitOrder& order)
    -> ExecutionReportBuilder& {
  message_.instrument = order.instrument();
  message_.parties = order.attributes().order_parties();
  message_.venue_order_id = order::to_venue_order_id(order.id());
  message_.client_order_id = order.client_order_id();
  message_.order_price = order.price();
  message_.leaves_quantity = order.leaves_quantity();
  message_.cum_executed_quantity = order.cum_executed_quantity();
  message_.order_status = order.status();
  message_.side = order.side();
  message_.time_in_force = order.time_in_force();
  message_.short_sale_exempt_reason = order.short_sale_exemption_reason();
  message_.expire_time = order.expire_time();
  message_.expire_date = order.expire_date();
  message_.order_type = OrderType::Option::Limit;
  return *this;
}

auto ExecutionReportBuilder::for_order(const MarketOrder& order)
    -> ExecutionReportBuilder& {
  message_.instrument = order.instrument();
  message_.parties = order.attributes().order_parties();
  message_.venue_order_id = order::to_venue_order_id(order.id());
  message_.client_order_id = order.client_order_id();
  message_.leaves_quantity = order.leaves_quantity();
  message_.cum_executed_quantity = order.cum_executed_quantity();
  message_.order_status = order.status();
  message_.side = order.side();
  message_.time_in_force = order.time_in_force();
  message_.short_sale_exempt_reason = order.short_sale_exemption_reason();
  message_.expire_time = order.expire_time();
  message_.expire_date = order.expire_date();
  message_.order_type = OrderType::Option::Market;
  return *this;
}

auto ExecutionReportBuilder::with_execution_id(ExecutionId identifier)
    -> ExecutionReportBuilder& {
  message_.execution_id = std::move(identifier);
  return *this;
}

auto ExecutionReportBuilder::with_execution_price(ExecutionPrice price)
    -> ExecutionReportBuilder& {
  message_.execution_price = price;
  return *this;
}

auto ExecutionReportBuilder::with_executed_quantity(ExecutedQuantity quantity)
    -> ExecutionReportBuilder& {
  message_.executed_quantity = quantity;
  return *this;
}

auto ExecutionReportBuilder::with_counterparty(
    std::optional<Party> counterparty) -> ExecutionReportBuilder& {
  if (counterparty.has_value()) {
    message_.parties.emplace_back(std::move(*counterparty));
  }
  return *this;
}

auto prepare_execution_report(const LimitOrder& order)
    -> ExecutionReportBuilder {
  ExecutionReportBuilder builder{order.client_session()};
  builder.for_order(order);
  return builder;
}

auto prepare_execution_report(const MarketOrder& order)
    -> ExecutionReportBuilder {
  ExecutionReportBuilder builder{order.client_session()};
  builder.for_order(order);
  return builder;
}

ModificationConfirmationBuilder::ModificationConfirmationBuilder(
    protocol::Session session)
    : message_(std::move(session)) {}

auto ModificationConfirmationBuilder::build() const
    -> protocol::OrderModificationConfirmation {
  return message_;
}

auto ModificationConfirmationBuilder::for_order(const LimitOrder& order)
    -> ModificationConfirmationBuilder& {
  message_.instrument = order.instrument();
  message_.parties = order.attributes().order_parties();
  message_.venue_order_id = order::to_venue_order_id(order.id());
  message_.client_order_id = order.client_order_id();
  message_.order_price = order.price();
  message_.leaving_quantity = order.leaves_quantity();
  message_.cum_executed_quantity = order.cum_executed_quantity();
  message_.order_status = order.status();
  message_.side = order.side();
  message_.time_in_force = order.time_in_force();
  message_.short_sale_exempt_reason = order.short_sale_exemption_reason();
  message_.expire_time = order.expire_time();
  message_.expire_date = order.expire_date();
  message_.order_type = OrderType::Option::Limit;
  return *this;
}

auto ModificationConfirmationBuilder::with_execution_id(ExecutionId identifier)
    -> ModificationConfirmationBuilder& {
  message_.execution_id = std::move(identifier);
  return *this;
}

auto ModificationConfirmationBuilder::with_orig_client_order_id(
    std::optional<OrigClientOrderId> identifier)
    -> ModificationConfirmationBuilder& {
  message_.orig_client_order_id = std::move(identifier);
  return *this;
}

auto prepare_modification_confirmation(const LimitOrder& order)
    -> ModificationConfirmationBuilder {
  ModificationConfirmationBuilder builder{order.client_session()};
  builder.for_order(order);
  return builder;
}

ModificationRejectBuilder::ModificationRejectBuilder(protocol::Session session)
    : message_(std::move(session)) {
  message_.order_status = OrderStatus::Option::Rejected;
}

auto ModificationRejectBuilder::build() const
    -> protocol::OrderModificationReject {
  return message_;
}

auto ModificationRejectBuilder::for_request(
    const protocol::OrderModificationRequest& request)
    -> ModificationRejectBuilder& {
  message_.venue_order_id = request.venue_order_id;
  message_.client_order_id = request.client_order_id;
  message_.orig_client_order_id = request.orig_client_order_id;
  return *this;
}

auto ModificationRejectBuilder::with_reason(RejectText reject_reason)
    -> ModificationRejectBuilder& {
  message_.reject_text = std::move(reject_reason);
  return *this;
}

auto ModificationRejectBuilder::with_order_status(OrderStatus status)
    -> ModificationRejectBuilder& {
  message_.order_status = status;
  return *this;
}

auto ModificationRejectBuilder::with_order_id(std::optional<OrderId> identifier)
    -> ModificationRejectBuilder& {
  if (identifier.has_value()) {
    message_.venue_order_id = order::to_venue_order_id(*identifier);
  }
  return *this;
}

auto ModificationRejectBuilder::with_client_order_id(
    std::optional<ClientOrderId> identifier) -> ModificationRejectBuilder& {
  message_.client_order_id = std::move(identifier);
  return *this;
}

auto ModificationRejectBuilder::with_orig_client_order_id(
    std::optional<OrigClientOrderId> identifier) -> ModificationRejectBuilder& {
  message_.orig_client_order_id = std::move(identifier);
  return *this;
}

auto prepare_modification_reject(const protocol::OrderModificationRequest&
                                     request) -> ModificationRejectBuilder {
  ModificationRejectBuilder builder{request.session};
  builder.for_request(request);
  return builder;
}

auto prepare_modification_reject(const LimitUpdate& update)
    -> ModificationRejectBuilder {
  ModificationRejectBuilder builder{update.client_session};
  builder.with_order_id(update.order_id)
      .with_client_order_id(update.client_order_id)
      .with_orig_client_order_id(update.orig_client_order_id);
  return builder;
}

PlacementConfirmationBuilder::PlacementConfirmationBuilder(
    protocol::Session session)
    : message_(std::move(session)) {}

auto PlacementConfirmationBuilder::build() const
    -> protocol::OrderPlacementConfirmation {
  return message_;
}

auto PlacementConfirmationBuilder::for_order(const LimitOrder& order)
    -> PlacementConfirmationBuilder& {
  message_.instrument = order.instrument();
  message_.parties = order.attributes().order_parties();
  message_.client_order_id = order.client_order_id();
  message_.venue_order_id = order::to_venue_order_id(order.id());
  message_.order_price = order.price();
  message_.order_quantity = order.total_quantity();
  message_.side = order.side();
  message_.time_in_force = order.time_in_force();
  message_.short_sale_exempt_reason = order.short_sale_exemption_reason();
  message_.expire_time = order.expire_time();
  message_.expire_date = order.expire_date();
  message_.order_type = OrderType::Option::Limit;
  return *this;
}

auto PlacementConfirmationBuilder::for_order(const MarketOrder& order)
    -> PlacementConfirmationBuilder& {
  message_.instrument = order.instrument();
  message_.parties = order.attributes().order_parties();
  message_.client_order_id = order.client_order_id();
  message_.venue_order_id = order::to_venue_order_id(order.id());
  message_.order_quantity = order.total_quantity();
  message_.side = order.side();
  message_.time_in_force = order.time_in_force();
  message_.short_sale_exempt_reason = order.short_sale_exemption_reason();
  message_.expire_time = order.expire_time();
  message_.expire_date = order.expire_date();
  message_.order_type = OrderType::Option::Market;
  return *this;
}

auto PlacementConfirmationBuilder::with_execution_id(ExecutionId identifier)
    -> PlacementConfirmationBuilder& {
  message_.execution_id = std::move(identifier);
  return *this;
}

auto prepare_placement_confirmation(const LimitOrder& order)
    -> PlacementConfirmationBuilder {
  PlacementConfirmationBuilder builder{order.client_session()};
  builder.for_order(order);
  return builder;
}

auto prepare_placement_confirmation(const MarketOrder& order)
    -> PlacementConfirmationBuilder {
  PlacementConfirmationBuilder builder{order.client_session()};
  builder.for_order(order);
  return builder;
}

PlacementRejectBuilder::PlacementRejectBuilder(protocol::Session session)
    : message_(std::move(session)) {}

auto PlacementRejectBuilder::build() const -> protocol::OrderPlacementReject {
  return message_;
}

auto PlacementRejectBuilder::for_request(
    const protocol::OrderPlacementRequest& request) -> PlacementRejectBuilder& {
  message_.instrument = request.instrument;
  message_.order_price = request.order_price;
  message_.order_quantity = request.order_quantity;
  message_.side = request.side;
  message_.order_type = request.order_type;
  message_.time_in_force = request.time_in_force;
  message_.client_order_id = request.client_order_id;
  message_.parties = request.parties;
  message_.short_sale_exempt_reason = request.short_sale_exempt_reason;
  message_.expire_time = request.expire_time;
  message_.expire_date = request.expire_date;
  return *this;
}

auto PlacementRejectBuilder::for_order(const LimitOrder& order)
    -> PlacementRejectBuilder& {
  message_.venue_order_id = order::to_venue_order_id(order.id());
  message_.instrument = order.instrument();
  message_.order_price = order.price();
  message_.order_quantity = order.total_quantity();
  message_.side = order.side();
  message_.time_in_force = order.time_in_force();
  message_.client_order_id = order.client_order_id();
  message_.parties = order.attributes().order_parties();
  message_.short_sale_exempt_reason = order.short_sale_exemption_reason();
  message_.expire_time = order.expire_time();
  message_.expire_date = order.expire_date();
  message_.order_type = OrderType::Option::Limit;
  return *this;
}

auto PlacementRejectBuilder::for_order(const MarketOrder& order)
    -> PlacementRejectBuilder& {
  message_.venue_order_id = order::to_venue_order_id(order.id());
  message_.instrument = order.instrument();
  message_.order_quantity = order.total_quantity();
  message_.side = order.side();
  message_.time_in_force = order.time_in_force();
  message_.client_order_id = order.client_order_id();
  message_.parties = order.attributes().order_parties();
  message_.short_sale_exempt_reason = order.short_sale_exemption_reason();
  message_.expire_time = order.expire_time();
  message_.expire_date = order.expire_date();
  message_.order_type = OrderType::Option::Market;
  return *this;
}

auto PlacementRejectBuilder::with_reason(RejectText reason)
    -> PlacementRejectBuilder& {
  message_.reject_text = std::move(reason);
  return *this;
}

auto PlacementRejectBuilder::with_order_id(OrderId identifier)
    -> PlacementRejectBuilder& {
  message_.venue_order_id = order::to_venue_order_id(identifier);
  return *this;
}

auto PlacementRejectBuilder::with_execution_id(ExecutionId identifier)
    -> PlacementRejectBuilder& {
  message_.execution_id = std::move(identifier);
  return *this;
}

auto prepare_placement_reject(const protocol::OrderPlacementRequest& request,
                              OrderId rejected_order_id)
    -> PlacementRejectBuilder {
  PlacementRejectBuilder builder{request.session};
  builder.for_request(request)
      .with_order_id(rejected_order_id)
      .with_execution_id(order::generate_aux_execution_id(rejected_order_id));
  return builder;
}

auto prepare_placement_reject(const LimitOrder& order)
    -> PlacementRejectBuilder {
  PlacementRejectBuilder builder{order.client_session()};
  builder.for_order(order);
  return builder;
}

auto prepare_placement_reject(const MarketOrder& order)
    -> PlacementRejectBuilder {
  PlacementRejectBuilder builder{order.client_session()};
  builder.for_order(order);
  return builder;
}

}  // namespace simulator::trading_system::matching_engine