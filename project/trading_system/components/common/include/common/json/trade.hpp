#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_JSON_TRADE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_JSON_TRADE_HPP_

#include "common/trade.hpp"
#include "core/common/json/type.hpp"

namespace simulator::core::json {

template <>
struct Type<simulator::trading_system::Trade> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value)
      -> simulator::trading_system::Trade;

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const simulator::trading_system::Trade& value)
      -> void;
};

}  // namespace simulator::core::json

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_JSON_TRADE_HPP_
