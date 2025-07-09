#ifndef SIMULATOR_PROTOCOL_APP_MARKET_DATA_SNAPSHOT_HPP_
#define SIMULATOR_PROTOCOL_APP_MARKET_DATA_SNAPSHOT_HPP_

#include <fmt/base.h>

#include <optional>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_data_entry.hpp"
#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct MarketDataSnapshot {
  explicit MarketDataSnapshot(Session protocol_session) noexcept;

  Session session;

  InstrumentDescriptor instrument;
  std::optional<MdRequestId> request_id;
  std::vector<MarketDataEntry> market_data_entries;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::MarketDataSnapshot> {
  using formattable = simulator::protocol::MarketDataSnapshot;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& message, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_MARKET_DATA_SNAPSHOT_HPP_
