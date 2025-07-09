#ifndef SIMULATOR_PROTOCOL_APP_SECURITY_STATUS_REQUEST_HPP_
#define SIMULATOR_PROTOCOL_APP_SECURITY_STATUS_REQUEST_HPP_

#include <fmt/base.h>

#include <optional>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct SecurityStatusRequest {
  explicit SecurityStatusRequest(Session protocol_session) noexcept;

  Session session;
  InstrumentDescriptor instrument;
  std::optional<SeqNum> seq_num;
  std::optional<SecurityStatusReqId> request_id;
  std::optional<MdSubscriptionRequestType> request_type;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::SecurityStatusRequest> {
  using formattable = simulator::protocol::SecurityStatusRequest;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& message, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_SECURITY_STATUS_REQUEST_HPP_
