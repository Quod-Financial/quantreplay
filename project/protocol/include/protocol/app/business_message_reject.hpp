#ifndef SIMULATOR_PROTOCOL_APP_BUSINESS_MESSAGE_REJECT_HPP_
#define SIMULATOR_PROTOCOL_APP_BUSINESS_MESSAGE_REJECT_HPP_

#include <fmt/base.h>

#include <optional>

#include "core/domain/attributes.hpp"
#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct BusinessMessageReject {
  explicit BusinessMessageReject(Session protocol_session) noexcept;

  Session session;
  std::optional<BusinessRejectRefId> ref_id;
  std::optional<RejectText> text;
  std::optional<SeqNum> ref_seq_num;
  std::optional<BusinessRejectReason> business_reject_reason;
  std::optional<RejectedMessageType> ref_message_type;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::BusinessMessageReject> {
  using formattable = simulator::protocol::BusinessMessageReject;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& message, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_BUSINESS_MESSAGE_REJECT_HPP_
