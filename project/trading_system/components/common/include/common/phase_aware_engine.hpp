#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_PHASE_AWARE_ENGINE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_PHASE_AWARE_ENGINE_HPP_

#include "common/events.hpp"

namespace simulator::trading_system {

class PhaseAwareEngine {
 public:
  PhaseAwareEngine() = default;
  PhaseAwareEngine(const PhaseAwareEngine&) = delete;
  PhaseAwareEngine(PhaseAwareEngine&&) = delete;
  virtual ~PhaseAwareEngine() = default;

  auto operator=(const PhaseAwareEngine&) -> PhaseAwareEngine& = delete;
  auto operator=(PhaseAwareEngine&&) -> PhaseAwareEngine& = delete;

  virtual auto handle(event::Tick tick) -> void = 0;

  virtual auto handle(event::PhaseTransition phase_transition) -> void = 0;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_PHASE_AWARE_ENGINE_HPP_
