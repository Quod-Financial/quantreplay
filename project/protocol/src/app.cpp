#include <fmt/base.h>

#include "core/common/name.hpp"
#include "core/tools/format.hpp"
#include "protocol/app/business_message_reject.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/instrument_state_request.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::protocol {

BusinessMessageReject::BusinessMessageReject(Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

ExecutionReport::ExecutionReport(Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

MarketDataReject::MarketDataReject(Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

MarketDataRequest::MarketDataRequest(Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

MarketDataSnapshot::MarketDataSnapshot(Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

MarketDataUpdate::MarketDataUpdate(Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

OrderCancellationConfirmation::OrderCancellationConfirmation(
    Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

OrderCancellationReject::OrderCancellationReject(
    Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

OrderCancellationRequest::OrderCancellationRequest(
    Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

OrderModificationConfirmation::OrderModificationConfirmation(
    Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

OrderModificationReject::OrderModificationReject(
    Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

OrderModificationRequest::OrderModificationRequest(
    Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

OrderPlacementConfirmation::OrderPlacementConfirmation(
    Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

OrderPlacementReject::OrderPlacementReject(Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

OrderPlacementRequest::OrderPlacementRequest(Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

SecurityStatus::SecurityStatus(Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

SecurityStatusRequest::SecurityStatusRequest(Session protocol_session) noexcept
    : session(std::move(protocol_session)) {}

SessionTerminatedEvent::SessionTerminatedEvent(
    Session terminated_session) noexcept
    : session(std::move(terminated_session)) {}

}  // namespace simulator::protocol

auto fmt::formatter<simulator::protocol::BusinessMessageReject>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "BusinessMessageReject={{ "
                   "{}, {}={}, {}={}, {}={}, {}={}, {}={}"
                   "}}",
                   message.session,
                   name_of(message.business_reject_reason),
                   message.business_reject_reason,
                   name_of(message.text),
                   message.text,
                   name_of(message.ref_message_type),
                   message.ref_message_type,
                   name_of(message.ref_seq_num),
                   message.ref_seq_num,
                   name_of(message.ref_id),
                   message.ref_id);
}

auto fmt::formatter<simulator::protocol::ExecutionReport>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(context.out(),
                   "ExecutionReport={{ "
                   "{}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, "
                   "{}={}, {}={}, {}={}, {:p}={} "
                   "}}",
                   message.session,
                   name_of(message.execution_type),
                   message.execution_type,
                   name_of(message.order_status),
                   message.order_status,
                   name_of(message.client_order_id),
                   message.client_order_id,
                   name_of(message.venue_order_id),
                   message.venue_order_id,
                   name_of(message.execution_id),
                   message.execution_id,
                   name_of(message.order_type),
                   message.order_type,
                   name_of(message.time_in_force),
                   message.time_in_force,
                   name_of(message.side),
                   message.side,
                   name_of(message.execution_price),
                   message.execution_price,
                   name_of(message.executed_quantity),
                   message.executed_quantity,
                   name_of(message.cum_executed_quantity),
                   message.cum_executed_quantity,
                   name_of(message.leaves_quantity),
                   message.leaves_quantity,
                   name_of(message.order_price),
                   message.order_price,
                   name_of(message.expire_date),
                   message.expire_date,
                   name_of(message.expire_time),
                   message.expire_time,
                   name_of(message.short_sale_exempt_reason),
                   message.short_sale_exempt_reason,
                   name_of(message.instrument),
                   message.instrument,
                   name_of(message.parties),
                   format_collection(message.parties));
}

auto fmt::formatter<simulator::protocol::InstrumentStateRequest>::format(
    const formattable& request, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "InstrumentStateRequest={{ {}={} }}",
                   name_of(request.instrument),
                   request.instrument);
}

auto fmt::formatter<simulator::protocol::InstrumentState>::format(
    const formattable& state, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "InstrumentState={{ {}={}, {}={}, {}={}, {}={} }}",
                   name_of(state.best_bid_price),
                   state.best_bid_price,
                   name_of(state.current_bid_depth),
                   state.current_bid_depth,
                   name_of(state.best_offer_price),
                   state.best_offer_price,
                   name_of(state.current_offer_depth),
                   state.current_offer_depth);
}

auto fmt::formatter<simulator::protocol::MarketDataReject>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "MarketDataReject={{ {}, {}={}, {}={}, {}={} }}",
                   message.session,
                   name_of(message.request_id),
                   message.request_id,
                   name_of(message.reject_reason),
                   message.reject_reason,
                   name_of(message.reject_text),
                   message.reject_text);
}

auto fmt::formatter<simulator::protocol::MarketDataRequest>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(context.out(),
                   "MarketDataRequest={{ "
                   "{}, {}={}, {}={}, {}={}, {}={} {:p}={}, {:p}={}, {:p}={} "
                   "}}",
                   message.session,
                   name_of(message.request_id),
                   message.request_id,
                   name_of(message.request_type),
                   message.request_type,
                   name_of(message.update_type),
                   message.update_type,
                   name_of(message.market_depth),
                   message.market_depth,
                   name_of(message.market_data_types),
                   format_collection(message.market_data_types),
                   name_of(message.instruments),
                   format_collection(message.instruments),
                   name_of(message.parties),
                   format_collection(message.parties));
}

auto fmt::formatter<simulator::protocol::MarketDataSnapshot>::format(
    const formattable& snapshot, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(context.out(),
                   "MarketDataSnapshot={{ {}, {}={}, {}={}, {:p}={} }}",
                   snapshot.session,
                   name_of(snapshot.request_id),
                   snapshot.request_id,
                   name_of(snapshot.instrument),
                   snapshot.instrument,
                   name_of(snapshot.market_data_entries),
                   format_collection(snapshot.market_data_entries));
}

auto fmt::formatter<simulator::protocol::MarketDataUpdate>::format(
    const formattable& update, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(context.out(),
                   "MarketDataUpdate={{ {}, {}={}, {:p}={} }}",
                   update.session,
                   name_of(update.request_id),
                   update.request_id,
                   name_of(update.market_data_entries),
                   format_collection(update.market_data_entries));
}

auto fmt::formatter<simulator::protocol::OrderCancellationConfirmation>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(context.out(),
                   "OrderCancellationConfirmation={{ "
                   "{}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {:p}={} "
                   "}}",
                   message.session,
                   name_of(message.client_order_id),
                   message.client_order_id,
                   name_of(message.orig_client_order_id),
                   message.orig_client_order_id,
                   name_of(message.venue_order_id),
                   message.venue_order_id,
                   name_of(message.execution_id),
                   message.execution_id,
                   name_of(message.leaving_quantity),
                   message.leaving_quantity,
                   name_of(message.cum_executed_quantity),
                   message.cum_executed_quantity,
                   name_of(message.order_status),
                   message.order_status,
                   name_of(message.order_type),
                   message.order_type,
                   name_of(message.time_in_force),
                   message.time_in_force,
                   name_of(message.side),
                   message.side,
                   name_of(message.short_sale_exempt_reason),
                   message.short_sale_exempt_reason,
                   name_of(message.expire_date),
                   message.expire_date,
                   name_of(message.expire_time),
                   message.expire_time,
                   name_of(message.instrument),
                   message.instrument,
                   name_of(message.parties),
                   format_collection(message.parties));
}

auto fmt::formatter<simulator::protocol::OrderCancellationReject>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "OrderCancellationReject={{ "
                   "{}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={} "
                   "}}",
                   message.session,
                   name_of(message.client_order_id),
                   message.client_order_id,
                   name_of(message.orig_client_order_id),
                   message.orig_client_order_id,
                   name_of(message.venue_order_id),
                   message.venue_order_id,
                   name_of(message.order_status),
                   message.order_status,
                   name_of(message.reject_text),
                   message.reject_text);
}

auto fmt::formatter<simulator::protocol::OrderCancellationRequest>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(context.out(),
                   "OrderCancellationRequest={{ "
                   "{}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {:p}={} "
                   "}}",
                   message.session,
                   name_of(message.client_order_id),
                   message.client_order_id,
                   name_of(message.orig_client_order_id),
                   message.orig_client_order_id,
                   name_of(message.venue_order_id),
                   message.venue_order_id,
                   name_of(message.side),
                   message.side,
                   name_of(message.instrument),
                   message.instrument,
                   name_of(message.parties),
                   format_collection(message.parties));
}

auto fmt::formatter<simulator::protocol::OrderModificationConfirmation>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(context.out(),
                   "OrderModificationConfirmation={{ "
                   "{}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {:p}={} "
                   "}}",
                   message.session,
                   name_of(message.client_order_id),
                   message.client_order_id,
                   name_of(message.orig_client_order_id),
                   message.orig_client_order_id,
                   name_of(message.venue_order_id),
                   message.venue_order_id,
                   name_of(message.execution_id),
                   message.execution_id,
                   name_of(message.leaving_quantity),
                   message.leaving_quantity,
                   name_of(message.cum_executed_quantity),
                   message.cum_executed_quantity,
                   name_of(message.order_price),
                   message.order_price,
                   name_of(message.order_status),
                   message.order_status,
                   name_of(message.order_type),
                   message.order_type,
                   name_of(message.time_in_force),
                   message.time_in_force,
                   name_of(message.side),
                   message.side,
                   name_of(message.short_sale_exempt_reason),
                   message.short_sale_exempt_reason,
                   name_of(message.expire_date),
                   message.expire_date,
                   name_of(message.expire_time),
                   message.expire_time,
                   name_of(message.instrument),
                   message.instrument,
                   name_of(message.parties),
                   format_collection(message.parties));
}

auto fmt::formatter<simulator::protocol::OrderModificationReject>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "OrderModificationReject={{ "
                   "{}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={} "
                   "}}",
                   message.session,
                   name_of(message.client_order_id),
                   message.client_order_id,
                   name_of(message.orig_client_order_id),
                   message.orig_client_order_id,
                   name_of(message.venue_order_id),
                   message.venue_order_id,
                   name_of(message.order_status),
                   message.order_status,
                   name_of(message.reject_text),
                   message.reject_text);
}

auto fmt::formatter<simulator::protocol::OrderModificationRequest>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(context.out(),
                   "OrderModificationRequest={{ "
                   "{}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, "
                   "{}={}, {}={}, {}={}, {}={}, {:p}={} "
                   "}}",
                   message.session,
                   name_of(message.client_order_id),
                   message.client_order_id,
                   name_of(message.orig_client_order_id),
                   message.orig_client_order_id,
                   name_of(message.venue_order_id),
                   message.venue_order_id,
                   name_of(message.order_type),
                   message.order_type,
                   name_of(message.time_in_force),
                   message.time_in_force,
                   name_of(message.side),
                   message.side,
                   name_of(message.order_quantity),
                   message.order_quantity,
                   name_of(message.order_price),
                   message.order_price,
                   name_of(message.expire_date),
                   message.expire_date,
                   name_of(message.expire_time),
                   message.expire_time,
                   name_of(message.short_sale_exempt_reason),
                   message.short_sale_exempt_reason,
                   name_of(message.instrument),
                   message.instrument,
                   name_of(message.parties),
                   format_collection(message.parties));
}

auto fmt::formatter<simulator::protocol::OrderPlacementConfirmation>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(context.out(),
                   "OrderPlacementConfirmation={{ "
                   "{}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, "
                   "{}={}, {}={}, {}={}, {}={}, {:p}={} "
                   "}}",
                   message.session,
                   name_of(message.client_order_id),
                   message.client_order_id,
                   name_of(message.venue_order_id),
                   message.venue_order_id,
                   name_of(message.execution_id),
                   message.execution_id,
                   name_of(message.order_quantity),
                   message.order_quantity,
                   name_of(message.order_price),
                   message.order_price,
                   name_of(message.order_type),
                   message.order_type,
                   name_of(message.time_in_force),
                   message.time_in_force,
                   name_of(message.side),
                   message.side,
                   name_of(message.short_sale_exempt_reason),
                   message.short_sale_exempt_reason,
                   name_of(message.expire_date),
                   message.expire_date,
                   name_of(message.expire_time),
                   message.expire_time,
                   name_of(message.instrument),
                   message.instrument,
                   name_of(message.parties),
                   format_collection(message.parties));
}

auto fmt::formatter<simulator::protocol::OrderPlacementReject>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(context.out(),
                   "OrderPlacementReject={{ "
                   "{}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {:p}={} "
                   "}}",
                   message.session,
                   name_of(message.client_order_id),
                   message.client_order_id,
                   name_of(message.venue_order_id),
                   message.venue_order_id,
                   name_of(message.execution_id),
                   message.execution_id,
                   name_of(message.order_quantity),
                   message.order_quantity,
                   name_of(message.order_type),
                   message.order_type,
                   name_of(message.time_in_force),
                   message.time_in_force,
                   name_of(message.side),
                   message.side,
                   name_of(message.short_sale_exempt_reason),
                   message.short_sale_exempt_reason,
                   name_of(message.expire_date),
                   message.expire_date,
                   name_of(message.expire_time),
                   message.expire_time,
                   name_of(message.reject_text),
                   message.reject_text,
                   name_of(message.instrument),
                   message.instrument,
                   name_of(message.parties),
                   format_collection(message.parties));
}

auto fmt::formatter<simulator::protocol::OrderPlacementRequest>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(context.out(),
                   "OrderPlacementRequest={{ "
                   "{}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, "
                   "{}={}, {}={}, {:p}={} "
                   "}}",
                   message.session,
                   name_of(message.client_order_id),
                   message.client_order_id,
                   name_of(message.order_type),
                   message.order_type,
                   name_of(message.time_in_force),
                   message.time_in_force,
                   name_of(message.side),
                   message.side,
                   name_of(message.order_quantity),
                   message.order_quantity,
                   name_of(message.order_price),
                   message.order_price,
                   name_of(message.expire_date),
                   message.expire_date,
                   name_of(message.expire_time),
                   message.expire_time,
                   name_of(message.short_sale_exempt_reason),
                   message.short_sale_exempt_reason,
                   name_of(message.instrument),
                   message.instrument,
                   name_of(message.parties),
                   format_collection(message.parties));
}

auto fmt::formatter<simulator::protocol::SessionTerminatedEvent>::format(
    const formattable& event, format_context& context) const
    -> decltype(context.out()) {
  return format_to(
      context.out(), "SessionTerminatedEvent={{ {} }}", event.session);
}

auto fmt::formatter<simulator::protocol::SecurityStatus>::format(
    const formattable& message, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "SecurityStatus={{ "
                   "{}, {}={}, {}={}, {}={} "
                   "}}",
                   message.session,
                   name_of(message.trading_phase),
                   message.trading_phase,
                   name_of(message.trading_status),
                   message.trading_status,
                   name_of(message.instrument),
                   message.instrument);
}

auto fmt::formatter<simulator::protocol::SecurityStatusRequest>::format(
    const formattable& request, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "SecurityStatusRequest={{ {}, {}={}, {}={}, {}={}, {}={} }}",
                   request.session,
                   name_of(request.instrument),
                   request.instrument,
                   name_of(request.request_id),
                   request.request_id,
                   name_of(request.request_type),
                   request.request_type,
                   name_of(request.seq_num),
                   request.seq_num);
}