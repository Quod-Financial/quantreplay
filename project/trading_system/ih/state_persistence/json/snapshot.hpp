#ifndef SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON_SNAPSHOT_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON_SNAPSHOT_HPP_

#include <rapidjson/document.h>

#include <tl/expected.hpp>

#include "common/instrument_state.hpp"
#include "ih/state_persistence/snapshot.hpp"

namespace simulator::trading_system::json {

auto read(const rapidjson::Value& json_value, market_state::Snapshot& dest)
    -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::Snapshot& source)
    -> tl::expected<void, std::string>;

}  // namespace simulator::trading_system::json

#endif  // SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON_SNAPSHOT_HPP_
