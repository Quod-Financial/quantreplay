#ifndef SIMULATOR_GENERATOR_IH_ADAPTATION_PROTOCOL_CONVERSION_HPP_
#define SIMULATOR_GENERATOR_IH_ADAPTATION_PROTOCOL_CONVERSION_HPP_

#include "core/domain/instrument_descriptor.hpp"
#include "data_layer/api/models/listing.hpp"
#include "ih/adaptation/generated_message.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/order_placement_request.hpp"

namespace simulator::generator {

[[nodiscard]] auto convert_to_instrument_descriptor(
    const data_layer::Listing& listing) -> InstrumentDescriptor;

[[nodiscard]] auto convert_to_order_placement_request(
    const GeneratedMessage& message, const InstrumentDescriptor& instrument)
    -> protocol::OrderPlacementRequest;

[[nodiscard]] auto convert_to_order_modification_request(
    const GeneratedMessage& message, const InstrumentDescriptor& instrument)
    -> protocol::OrderModificationRequest;

[[nodiscard]] auto convert_to_order_cancellation_request(
    const GeneratedMessage& message, const InstrumentDescriptor& instrument)
    -> protocol::OrderCancellationRequest;

[[nodiscard]]
auto convert_to_generated_message(
    const protocol::OrderPlacementConfirmation& confirmation)
    -> GeneratedMessage;

[[nodiscard]]
auto convert_to_generated_message(const protocol::OrderPlacementReject& reject)
    -> GeneratedMessage;

[[nodiscard]]
auto convert_to_generated_message(
    const protocol::OrderModificationConfirmation& confirmation)
    -> GeneratedMessage;

[[nodiscard]]
auto convert_to_generated_message(
    const protocol::OrderCancellationConfirmation& confirmation)
    -> GeneratedMessage;

[[nodiscard]]
auto convert_to_generated_message(const protocol::ExecutionReport& report)
    -> GeneratedMessage;

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_ADAPTATION_PROTOCOL_CONVERSION_HPP_
