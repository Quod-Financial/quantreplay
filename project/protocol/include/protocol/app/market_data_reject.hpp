#ifndef SIMULATOR_PROTOCOL_APP_MARKET_DATA_REJECT_HPP_
#define SIMULATOR_PROTOCOL_APP_MARKET_DATA_REJECT_HPP_

#include <fmt/base.h>

#include <optional>

#include "core/domain/attributes.hpp"
#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct MarketDataReject {
  explicit MarketDataReject(Session protocol_session) noexcept;

  Session session;
  std::optional<MdRequestId> request_id;
  std::optional<RejectText> reject_text;
  std::optional<MdRejectReason> reject_reason;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::MarketDataReject> {
  using formattable = simulator::protocol::MarketDataReject;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& message, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_MARKET_DATA_REJECT_HPP_
