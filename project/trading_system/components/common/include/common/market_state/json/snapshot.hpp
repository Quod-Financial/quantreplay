#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_SNAPSHOT_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_SNAPSHOT_HPP_

#include "common/market_state/json/instrument_state.hpp"
#include "common/market_state/snapshot.hpp"
#include "core/common/json/type_struct.hpp"

template <>
struct simulator::core::json::Struct<
    simulator::trading_system::market_state::Snapshot> {
  static constexpr auto fields = std::make_tuple(
      Field(&simulator::trading_system::market_state::Snapshot::venue_id,
            "venue_id"),
      Field(&simulator::trading_system::market_state::Snapshot::instruments,
            "instruments"));
};

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_SNAPSHOT_HPP_
