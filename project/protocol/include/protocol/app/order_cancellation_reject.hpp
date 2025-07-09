#ifndef SIMULATOR_PROTOCOL_APP_ORDER_CANCELLATION_REJECT_HPP_
#define SIMULATOR_PROTOCOL_APP_ORDER_CANCELLATION_REJECT_HPP_

#include <fmt/base.h>

#include <optional>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct OrderCancellationReject {
  explicit OrderCancellationReject(Session protocol_session) noexcept;

  Session session;
  std::optional<OrigClientOrderId> orig_client_order_id;
  std::optional<VenueOrderId> venue_order_id;
  std::optional<ClientOrderId> client_order_id;
  std::optional<RejectText> reject_text;
  std::optional<OrderStatus> order_status;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::OrderCancellationReject> {
  using formattable = simulator::protocol::OrderCancellationReject;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& message, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_ORDER_CANCELLATION_REJECT_HPP_