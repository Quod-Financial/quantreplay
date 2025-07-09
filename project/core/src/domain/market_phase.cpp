#include "core/domain/json/market_phase.hpp"

#include "core/domain/market_phase.hpp"

namespace simulator::core::json {

auto Type<simulator::MarketPhase>::read_json_value(
    const rapidjson::Value& json_value) -> simulator::MarketPhase {
  auto phase = json::read_json_value<TradingPhase>(json_value, "trading_phase");
  auto status =
      json::read_json_value<TradingStatus>(json_value, "trading_status");
  return simulator::MarketPhase{std::move(phase), std::move(status)};
}

auto Type<simulator::MarketPhase>::write_json_value(
    rapidjson::Value& json_value,
    rapidjson::Document::AllocatorType& allocator,
    const simulator::MarketPhase& value) -> void {
  json_value = rapidjson::Value(rapidjson::Type::kObjectType);
  json_value.MemberReserve(2, allocator);
  json::write_json_value(
      json_value, allocator, value.trading_phase(), "trading_phase");
  json::write_json_value(
      json_value, allocator, value.trading_status(), "trading_status");
}

}  // namespace simulator::core::json
