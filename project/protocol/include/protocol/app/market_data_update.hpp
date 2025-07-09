#ifndef SIMULATOR_PROTOCOL_APP_MARKET_DATA_UPDATE_HPP_
#define SIMULATOR_PROTOCOL_APP_MARKET_DATA_UPDATE_HPP_

#include <fmt/base.h>

#include <optional>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_data_entry.hpp"
#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct MarketDataUpdate {
  explicit MarketDataUpdate(Session protocol_session) noexcept;

  Session session;

  std::optional<MdRequestId> request_id;
  std::vector<MarketDataEntry> market_data_entries;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::MarketDataUpdate> {
  using formattable = simulator::protocol::MarketDataUpdate;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& message, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_MARKET_DATA_UPDATE_HPP_
