#include "common/trade.hpp"

#include "common/json/trade.hpp"

namespace simulator::core::json {

auto Type<trading_system::Trade>::read_json_value(
    const rapidjson::Value& json_value) -> simulator::trading_system::Trade {
  auto buyer =
      json::read_json_value<std::optional<BuyerId>>(json_value, "buyer");
  auto seller =
      json::read_json_value<std::optional<SellerId>>(json_value, "seller");
  auto trade_price = json::read_json_value<Price>(json_value, "trade_price");
  auto traded_quantity =
      json::read_json_value<Quantity>(json_value, "traded_quantity");
  auto aggressor_side = json::read_json_value<std::optional<AggressorSide>>(
      json_value, "aggressor_side");
  auto trade_time =
      json::read_json_value<core::sys_us>(json_value, "trade_time");
  auto market_phase =
      json::read_json_value<MarketPhase>(json_value, "market_phase");

  return {.buyer = std::move(buyer),
          .seller = std::move(seller),
          .trade_price = std::move(trade_price),
          .traded_quantity = std::move(traded_quantity),
          .aggressor_side = std::move(aggressor_side),
          .trade_time = std::move(trade_time),
          .market_phase = std::move(market_phase)};
}

auto Type<trading_system::Trade>::write_json_value(
    rapidjson::Value& json_value,
    rapidjson::Document::AllocatorType& allocator,
    const simulator::trading_system::Trade& value) -> void {
  json_value = rapidjson::Value(rapidjson::Type::kObjectType);
  json_value.MemberReserve(7, allocator);

  json::write_json_value(json_value, allocator, value.buyer, "buyer");
  json::write_json_value(json_value, allocator, value.seller, "seller");
  json::write_json_value(
      json_value, allocator, value.trade_price, "trade_price");
  json::write_json_value(
      json_value, allocator, value.traded_quantity, "traded_quantity");
  json::write_json_value(
      json_value, allocator, value.aggressor_side, "aggressor_side");
  json::write_json_value(json_value, allocator, value.trade_time, "trade_time");
  json::write_json_value(
      json_value, allocator, value.market_phase, "market_phase");
}

}  // namespace simulator::core::json
