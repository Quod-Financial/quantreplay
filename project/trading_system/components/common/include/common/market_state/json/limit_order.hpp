#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_LIMIT_ORDER_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_LIMIT_ORDER_HPP_

#include "common/market_state/json/session.hpp"
#include "common/market_state/snapshot.hpp"
#include "core/common/json/type_struct.hpp"
#include "core/domain/json/instrument_descriptor.hpp"
#include "core/domain/json/party.hpp"
#include "protocol/types/json/session.hpp"

template <>
struct simulator::core::json::Struct<
    simulator::trading_system::market_state::LimitOrder> {
  static constexpr auto fields = std::make_tuple(
      Field(&simulator::trading_system::market_state::LimitOrder::
                client_instrument_descriptor,
            "client_instrument_descriptor"),
      Field(
          &simulator::trading_system::market_state::LimitOrder::client_session,
          "client_session"),
      Field(
          &simulator::trading_system::market_state::LimitOrder::client_order_id,
          "client_order_id"),
      Field(&simulator::trading_system::market_state::LimitOrder::order_parties,
            "order_parties"),
      Field(&simulator::trading_system::market_state::LimitOrder::expire_time,
            "expire_time"),
      Field(&simulator::trading_system::market_state::LimitOrder::expire_date,
            "expire_date"),
      Field(&simulator::trading_system::market_state::LimitOrder::
                short_sale_exemption_reason,
            "short_sale_exemption_reason"),
      Field(&simulator::trading_system::market_state::LimitOrder::time_in_force,
            "time_in_force"),
      Field(&simulator::trading_system::market_state::LimitOrder::order_id,
            "order_id"),
      Field(&simulator::trading_system::market_state::LimitOrder::order_time,
            "order_time"),
      Field(&simulator::trading_system::market_state::LimitOrder::side, "side"),
      Field(&simulator::trading_system::market_state::LimitOrder::order_status,
            "order_status"),
      Field(&simulator::trading_system::market_state::LimitOrder::order_price,
            "order_price"),
      Field(
          &simulator::trading_system::market_state::LimitOrder::total_quantity,
          "total_quantity"),
      Field(&simulator::trading_system::market_state::LimitOrder::
                cum_executed_quantity,
            "cum_executed_quantity"));
};

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_LIMIT_ORDER_HPP_
