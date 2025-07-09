#ifndef SIMULATOR_PROTOCOL_APP_MARKET_DATA_REQUEST_HPP_
#define SIMULATOR_PROTOCOL_APP_MARKET_DATA_REQUEST_HPP_

#include <fmt/base.h>

#include <optional>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct MarketDataRequest {
  explicit MarketDataRequest(Session protocol_session) noexcept;

  Session session;

  std::optional<MdRequestId> request_id;
  std::vector<InstrumentDescriptor> instruments;
  std::vector<MdEntryType> market_data_types;
  std::vector<Party> parties;
  std::optional<MarketDepth> market_depth;
  std::optional<MdSubscriptionRequestType> request_type;
  std::optional<MarketDataUpdateType> update_type;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::MarketDataRequest> {
  using formattable = simulator::protocol::MarketDataRequest;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& message, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_MARKET_DATA_REQUEST_HPP_
