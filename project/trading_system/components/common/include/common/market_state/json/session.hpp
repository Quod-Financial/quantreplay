#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_SESSION_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_SESSION_HPP_

#include "common/market_state/snapshot.hpp"
#include "core/common/json/type_struct.hpp"
#include "protocol/types/json/session.hpp"
#include "protocol/types/session.hpp"

template <>
struct simulator::core::json::Struct<
    simulator::trading_system::market_state::Session> {
  static constexpr auto fields = std::make_tuple(
      Field(&simulator::trading_system::market_state::Session::type,
            "type"),
      Field(&simulator::trading_system::market_state::Session::fix_session,
            "fix_session"));
};

#endif  //  SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_SESSION_HPP_
