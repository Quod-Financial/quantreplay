#ifndef SIMULATOR_PROTOCOL_APP_ORDER_CANCELLATION_REQUEST_HPP_
#define SIMULATOR_PROTOCOL_APP_ORDER_CANCELLATION_REQUEST_HPP_

#include <fmt/base.h>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct OrderCancellationRequest {
  explicit OrderCancellationRequest(Session protocol_session) noexcept;

  Session session;
  InstrumentDescriptor instrument;
  std::optional<OrigClientOrderId> orig_client_order_id;
  std::optional<VenueOrderId> venue_order_id;
  std::optional<ClientOrderId> client_order_id;
  std::vector<Party> parties;
  std::optional<Side> side;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::OrderCancellationRequest> {
  using formattable = simulator::protocol::OrderCancellationRequest;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& message, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_ORDER_CANCELLATION_REQUEST_HPP_