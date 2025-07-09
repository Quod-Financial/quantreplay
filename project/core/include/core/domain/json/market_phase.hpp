#ifndef SIMULATOR_CORE_DOMAIN_JSON_MARKET_PHASE_HPP_
#define SIMULATOR_CORE_DOMAIN_JSON_MARKET_PHASE_HPP_

#include "core/common/json/type.hpp"
#include "core/domain/market_phase.hpp"

namespace simulator::core::json {

template <>
struct Type<simulator::MarketPhase> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value)
      -> simulator::MarketPhase;

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const simulator::MarketPhase& value) -> void;
};

}  // namespace simulator::core::json

#endif  // SIMULATOR_CORE_DOMAIN_JSON_MARKET_PHASE_HPP_
