#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TRADING_ENGINE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TRADING_ENGINE_HPP_

#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::trading_system {

class TradingEngine {
 public:
  TradingEngine() = default;
  TradingEngine(const TradingEngine&) = delete;
  TradingEngine(TradingEngine&&) = delete;
  virtual ~TradingEngine() = default;

  auto operator=(const TradingEngine&) -> TradingEngine& = delete;
  auto operator=(TradingEngine&&) -> TradingEngine& = delete;

  virtual auto execute(protocol::OrderPlacementRequest request) -> void = 0;

  virtual auto execute(protocol::MarketDataRequest request) -> void = 0;

  virtual auto execute(protocol::SecurityStatusRequest request) -> void = 0;

  virtual auto handle(const protocol::SessionTerminatedEvent& event)
      -> void = 0;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TRADING_ENGINE_HPP_
