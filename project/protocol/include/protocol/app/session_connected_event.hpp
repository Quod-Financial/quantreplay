#ifndef SIMULATOR_PROTOCOL_APP_SESSION_CONNECTED_EVENT_HPP_
#define SIMULATOR_PROTOCOL_APP_SESSION_CONNECTED_EVENT_HPP_

#include <fmt/base.h>

#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct SessionConnectedEvent {
  explicit SessionConnectedEvent(Session connected_session) noexcept;

  Session session;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::SessionConnectedEvent> {
  using formattable = simulator::protocol::SessionConnectedEvent;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& event, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_SESSION_CONNECTED_EVENT_HPP_
