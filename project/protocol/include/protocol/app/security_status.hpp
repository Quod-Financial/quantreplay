#ifndef SIMULATOR_PROTOCOL_APP_SECURITY_STATUS_HPP_
#define SIMULATOR_PROTOCOL_APP_SECURITY_STATUS_HPP_

#include <fmt/base.h>

#include <optional>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_phase.hpp"
#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct SecurityStatus {
  explicit SecurityStatus(Session protocol_session) noexcept;

  Session session;
  InstrumentDescriptor instrument;
  std::optional<SecurityStatusReqId> request_id;
  std::optional<TradingPhase> trading_phase;
  std::optional<TradingStatus> trading_status;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::SecurityStatus> {
  using formattable = simulator::protocol::SecurityStatus;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& message, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_SECURITY_STATUS_HPP_
