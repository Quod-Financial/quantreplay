#ifndef SIMULATOR_PROTOCOL_APP_SESSION_TERMINATED_EVENT_HPP_
#define SIMULATOR_PROTOCOL_APP_SESSION_TERMINATED_EVENT_HPP_

#include <fmt/base.h>

#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct SessionTerminatedEvent {
  explicit SessionTerminatedEvent(Session terminated_session) noexcept;

  Session session;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::SessionTerminatedEvent> {
  using formattable = simulator::protocol::SessionTerminatedEvent;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& event, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_SESSION_TERMINATED_EVENT_HPP_