#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_MATCHING_ENGINE_CONFIGURATION_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_MATCHING_ENGINE_CONFIGURATION_HPP_

#include <optional>

#include "common/attributes.hpp"
#include "core/tools/time.hpp"

namespace simulator::trading_system::matching_engine {

struct Configuration {
  core::TzClock clock;

  std::optional<PriceTick> order_price_tick = std::nullopt;
  std::optional<QuantityTick> order_quantity_tick = std::nullopt;
  std::optional<MinQuantity> order_min_quantity = std::nullopt;
  std::optional<MaxQuantity> order_max_quantity = std::nullopt;

  bool enable_cancel_on_disconnect = true;
  bool enable_trades_streaming = true;
  bool report_trade_volume = true;
  bool report_trade_parties = true;
  bool report_trade_aggressor_side = true;
  bool support_market_data_orders_exclusion = false;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_MATCHING_ENGINE_CONFIGURATION_HPP_
