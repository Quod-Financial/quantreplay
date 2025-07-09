#include "ih/historical/processor.hpp"

#include <memory>
#include <string_view>
#include <vector>

#include "ih/adaptation/protocol_conversion.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/record_applier.hpp"
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
    const ActionProcessor::Contexts& available_contexts)
    : ctx_registry_{available_contexts} {}

auto ActionProcessor::process(historical::Action action) -> void {
  action.steal_records(
      [this](historical::Record record) { process(std::move(record)); });
}

auto ActionProcessor::process(historical::Record record) const -> void {
  const ContextPointer context = ctx_registry_.resolve_context(record);
  if (!context) {
    log::warn(
        "can not find corresponding instrument generation context, "
        "skipping historical {}",
        record);
    return;
  }

  const std::vector<GeneratedMessage> historical_messages =
      RecordApplier::apply(std::move(record), context);
  const InstrumentDescriptor& instrument = context->get_instrument_descriptor();

  for (const auto& historical_request : historical_messages) {
    send_message(historical_request, instrument);
  }
}

}  // namespace simulator::generator::historical
