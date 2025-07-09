#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TRADING_ENGINE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TRADING_ENGINE_HPP_

#include "common/events.hpp"
#include "common/market_state/snapshot.hpp"
#include "protocol/app/instrument_state_request.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
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

  virtual auto execute(protocol::OrderModificationRequest request) -> void = 0;

  virtual auto execute(protocol::OrderCancellationRequest request) -> void = 0;

  virtual auto execute(protocol::MarketDataRequest request) -> void = 0;

  virtual auto execute(protocol::SecurityStatusRequest request) -> void = 0;

  virtual auto provide_state(protocol::InstrumentState& reply) -> void = 0;

  virtual auto store_state(market_state::InstrumentState& state) -> void = 0;

  virtual auto recover_state(market_state::InstrumentState state) -> void = 0;

  virtual auto handle(const protocol::SessionTerminatedEvent& event)
      -> void = 0;

  virtual auto handle(event::Tick tick) -> void = 0;

  virtual auto handle(event::PhaseTransition phase_transition) -> void = 0;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TRADING_ENGINE_HPP_
