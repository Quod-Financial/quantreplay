#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_ORDER_BOOK_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_ORDER_BOOK_HPP_

#include "common/market_state/json/limit_order.hpp"
#include "common/market_state/snapshot.hpp"
#include "core/common/json/type_container.hpp"
#include "core/common/json/type_struct.hpp"

template <>
struct simulator::core::json::Struct<
    simulator::trading_system::market_state::OrderBook> {
  static constexpr auto fields = std::make_tuple(
      Field(
          &simulator::trading_system::market_state::OrderBook::buy_orders,
          "buy_orders"),
      Field(
          &simulator::trading_system::market_state::OrderBook::sell_orders,
          "sell_orders"));
};

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_JSON_ORDER_BOOK_HPP_
