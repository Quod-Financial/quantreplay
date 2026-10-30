#include "ih/historical/processing/action_processor.hpp"

#include <cstdint>
#include <memory>
#include <mutex>
#include <string_view>
#include <vector>

#include "ih/adaptation/protocol_conversion.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/processing/record_applier.hpp"
#include "log/logging.hpp"
#include "middleware/routing/trading_request_channel.hpp"

namespace simulator::generator::historical {
namespace {

auto send_message(const GeneratedMessage& message,
                  const InstrumentDescriptor& instrument) -> void {
  try {
    if (message.message_type == MessageType::NewOrderSingle) {
      middleware::send_trading_request(
          convert_to_order_placement_request(message, instrument));
    } else if (message.message_type == MessageType::OrderCancelReplaceRequest) {
      middleware::send_trading_request(
          convert_to_order_modification_request(message, instrument));
    } else if (message.message_type == MessageType::OrderCancelRequest) {
      middleware::send_trading_request(
          convert_to_order_cancellation_request(message, instrument));
    }
  } catch (const middleware::ChannelUnboundError&) {
    log::err(
        "failed to send message from historical replier - trading request "
        "channel is not bound");
  }
}

[[nodiscard]]
auto apply_exclusively(historical::Record record,
                       const std::shared_ptr<OrderInstrumentContext>& context,
                       std::uint64_t datasource_id)
    -> std::vector<GeneratedMessage> {
  const std::unique_lock lock = context->lock_record_application();
  return RecordApplier::apply(std::move(record), context, datasource_id);
}

}  // namespace

ActionProcessor::ContextsRegistry::ContextsRegistry(
    const ActionProcessor::Contexts& available_contexts) noexcept {
  for (const auto& context : available_contexts) {
    if (!context) {
      continue;
    }

    const auto& symbol = context->get_instrument().symbol();
    if (symbol && !symbol->empty()) {
      registry_.emplace(*symbol, context);
    } else {
      log::err(
          "failed to insert listing with listing_id {} into registry because "
          "its symbol is empty",
          context->get_instrument().listing_id());
    }
  }
}

auto ActionProcessor::ContextsRegistry::resolve_context(
    const historical::Record& record) const
    -> std::shared_ptr<OrderInstrumentContext> {
  const std::string_view instrument = record.instrument();
  auto target_it = registry_.find(instrument);
  return target_it != std::end(registry_) ? target_it->second : nullptr;
}

ActionProcessor::ActionProcessor(
    const ActionProcessor::Contexts& available_contexts,
    std::uint64_t datasource_id)
    : ctx_registry_{available_contexts}, datasource_id_{datasource_id} {}

auto ActionProcessor::process(historical::Action action) -> void {
  action.steal_records(
      [this](historical::Record record) { process(std::move(record)); });
}

auto ActionProcessor::process(historical::Record record) -> void {
  const ContextPointer context = ctx_registry_.resolve_context(record);
  if (!context) {
    log::warn(
        "can not find corresponding instrument generation context, "
        "skipping historical {}",
        record);
    return;
  }

  const std::vector<GeneratedMessage> historical_messages =
      apply_exclusively(std::move(record), context, datasource_id_);
  const InstrumentDescriptor& instrument = context->get_instrument_descriptor();

  for (const auto& historical_request : historical_messages) {
    send_message(historical_request, instrument);
  }
}

}  // namespace simulator::generator::historical
