#ifndef SIMULATOR_GENERATOR_IH_ADAPTATION_GENERATED_MESSAGE_HPP_
#define SIMULATOR_GENERATOR_IH_ADAPTATION_GENERATED_MESSAGE_HPP_

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"

namespace simulator::generator {

enum class MessageType : std::uint8_t {
  NewOrderSingle,
  OrderCancelReplaceRequest,
  OrderCancelRequest,
  ExecutionReport
};

struct GeneratedMessage {
  MessageType message_type;
  std::optional<OrderType> order_type;
  std::optional<TimeInForce> time_in_force;
  std::optional<Side> side;
  std::optional<OrderPrice> order_price;
  std::optional<Quantity> quantity;
  std::optional<ClientOrderId> client_order_id;
  std::optional<OrigClientOrderId> orig_client_order_id;
  std::optional<PartyId> party_id;
  std::optional<OrderStatus> order_status;
};

auto operator<<(std::ostream& os, MessageType& message_type) -> std::ostream&;

auto operator<<(std::ostream& os, const GeneratedMessage& message)
    -> std::ostream&;

}  // namespace simulator::generator

template <>
struct fmt::formatter<simulator::generator::MessageType>
    : fmt::formatter<std::string_view> {
  using MessageType = simulator::generator::MessageType;

  auto format(MessageType message_type, format_context& ctx) const
      -> decltype(ctx.out());
};

template <>
struct fmt::formatter<simulator::generator::GeneratedMessage>
    : fmt::formatter<std::string_view> {
  using formattable = simulator::generator::GeneratedMessage;

  auto format(formattable message, format_context& ctx) const
      -> decltype(ctx.out());
};

#endif  // SIMULATOR_GENERATOR_IH_ADAPTATION_GENERATED_MESSAGE_HPP_
