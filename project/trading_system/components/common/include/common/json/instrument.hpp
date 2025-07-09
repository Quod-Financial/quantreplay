#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_JSON_INSTRUMENT_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_JSON_INSTRUMENT_HPP_

#include "common/instrument.hpp"
#include "core/common/json/type_struct.hpp"

template <>
struct simulator::core::json::Struct<simulator::trading_system::Instrument> {
  static constexpr auto fields = std::make_tuple(
      Field(&simulator::trading_system::Instrument::symbol, "symbol"),
      Field(&simulator::trading_system::Instrument::price_currency,
            "price_currency"),
      Field(&simulator::trading_system::Instrument::base_currency,
            "base_currency"),
      Field(&simulator::trading_system::Instrument::security_exchange,
            "security_exchange"),
      Field(&simulator::trading_system::Instrument::party_id, "party_id"),
      Field(&simulator::trading_system::Instrument::cusip, "cusip"),
      Field(&simulator::trading_system::Instrument::sedol, "sedol"),
      Field(&simulator::trading_system::Instrument::isin, "isin"),
      Field(&simulator::trading_system::Instrument::ric, "ric"),
      Field(&simulator::trading_system::Instrument::exchange_id, "exchange_id"),
      Field(&simulator::trading_system::Instrument::bloomberg_id,
            "bloomberg_id"),
      Field(&simulator::trading_system::Instrument::price_tick, "price_tick"),
      Field(&simulator::trading_system::Instrument::quantity_tick,
            "quantity_tick"),
      Field(&simulator::trading_system::Instrument::min_quantity,
            "min_quantity"),
      Field(&simulator::trading_system::Instrument::max_quantity,
            "max_quantity"),
      Field(&simulator::trading_system::Instrument::party_role, "party_role"),
      Field(&simulator::trading_system::Instrument::security_type,
            "security_type"));
};

#endif  //  SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_JSON_INSTRUMENT_HPP_
