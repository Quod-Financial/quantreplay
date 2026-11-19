#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_MATCHING_ENGINE_MATCHING_ENGINE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_MATCHING_ENGINE_MATCHING_ENGINE_HPP_

#include <memory>

#include "common/events.hpp"
#include "common/instrument.hpp"
#include "common/instrument_state.hpp"
#include "common/order_modifying_engine.hpp"
#include "common/phase_aware_engine.hpp"
#include "common/state_persisting_engine.hpp"
#include "common/trading_engine.hpp"
#include "matching_engine/configuration.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/app/session_terminated_event.hpp"
#include "runtime/mux.hpp"
#include "runtime/service.hpp"

namespace simulator::trading_system::matching_engine {

class MatchingEngine final : public TradingEngine,
                             public OrderModifyingEngine,
                             public StatePersistingEngine,
                             public PhaseAwareEngine {
 public:
  class Implementation;

  MatchingEngine(const Instrument& instrument,
                 const Configuration& configuration,
                 runtime::Service& executor) noexcept;

  MatchingEngine() = delete;
  MatchingEngine(const MatchingEngine&) = delete;
  MatchingEngine(MatchingEngine&&) = delete;
  ~MatchingEngine() noexcept override;

  auto operator=(const MatchingEngine&) -> MatchingEngine& = delete;
  auto operator=(MatchingEngine&&) -> MatchingEngine& = delete;

  auto execute(protocol::OrderPlacementRequest request) -> void override;

  auto execute(protocol::OrderModificationRequest request) -> void override;

  auto execute(protocol::OrderCancellationRequest request) -> void override;

  auto execute(protocol::MarketDataRequest request) -> void override;

  auto execute(protocol::SecurityStatusRequest request) -> void override;

  auto store_state(market_state::InstrumentState& state) -> void override;

  auto recover_state(market_state::InstrumentState state) -> void override;

  auto handle(const protocol::SessionTerminatedEvent& event) -> void override;

  auto handle(event::Tick tick) -> void override;

  auto handle(event::PhaseTransition phase_transition) -> void override;

 private:
  runtime::Mux mux_;
  std::unique_ptr<Implementation> implementation_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_MATCHING_ENGINE_MATCHING_ENGINE_HPP_
