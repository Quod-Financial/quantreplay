#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_INSTRUMENT_STATE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_INSTRUMENT_STATE_HPP_

#include "common/json/instrument.hpp"
#include "common/json/trade.hpp"
#include "common/market_state/json/instrument_info.hpp"
#include "common/market_state/json/order_book.hpp"
#include "common/market_state/snapshot.hpp"
#include "core/common/json/type_container.hpp"
#include "core/common/json/type_struct.hpp"

template <>
struct simulator::core::json::Struct<
    simulator::trading_system::market_state::InstrumentState> {
  static constexpr auto fields = std::make_tuple(
      Field(&simulator::trading_system::market_state::InstrumentState::
                instrument,
            "instrument"),
      Field(&simulator::trading_system::market_state::InstrumentState::
                last_trade,
            "last_trade"),
      Field(
          &simulator::trading_system::market_state::InstrumentState::info,
          "info"),
      Field(&simulator::trading_system::market_state::InstrumentState::
                order_book,
            "order_book"));
};

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_INSTRUMENT_STATE_HPP_
