#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_INSTRUMENT_INFO_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_INSTRUMENT_INFO_HPP_

#include "common/market_state/snapshot.hpp"
#include "core/common/json/type_struct.hpp"

template <>
struct simulator::core::json::Struct<
    simulator::trading_system::market_state::InstrumentInfo> {
  static constexpr auto fields = std::make_tuple(
      Field(&simulator::trading_system::market_state::InstrumentInfo::low_price,
            "low_price"),
      Field(
          &simulator::trading_system::market_state::InstrumentInfo::high_price,
          "high_price"));
};

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_INSTRUMENT_INFO_HPP_
