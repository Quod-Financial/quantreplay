#include "ih/adaptation/generated_message.hpp"

namespace simulator::generator {

auto operator<<(std::ostream& os, MessageType message_type) -> std::ostream& {
  return os << fmt::to_string(message_type);
}

auto operator<<(std::ostream& os, const GeneratedMessage& message)
    -> std::ostream& {
  return os << fmt::to_string(message);
}

}  // namespace simulator::generator

auto fmt::formatter<simulator::generator::MessageType>::format(
    MessageType message_type, format_context& ctx) const
    -> decltype(ctx.out()) {
  std::string_view value{"undefined"};

  switch (message_type) {
    case MessageType::NewOrderSingle:
      value = "NewOrderSingle";
      break;
    case MessageType::OrderCancelReplaceRequest:
      value = "OrderCancelReplaceRequest";
      break;
    case MessageType::OrderCancelRequest:
      value = "OrderCancelRequest";
      break;
    case MessageType::ExecutionReport:
      value = "ExecutionReport";
      break;
  }

  return fmt::format_to(ctx.out(), "{}", value);
}

auto fmt::formatter<simulator::generator::GeneratedMessage>::format(
    formattable message, format_context& ctx) const -> decltype(ctx.out()) {
  return fmt::format_to(
      ctx.out(),
      "GeneratedMessage={{ message_type={}, order_type={}, "
      "time_in_force={}, side={}, order_price={}, quantity={}, "
      "client_order_id={}, orig_client_order_id={}, party_id={}, "
      "order_status={} }}",
      message.message_type,
      message.order_type,
      message.time_in_force,
      message.side,
      message.order_price,
      message.quantity,
      message.client_order_id,
      message.orig_client_order_id,
      message.party_id,
      message.order_status);
}