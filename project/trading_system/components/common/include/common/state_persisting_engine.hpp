#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_STATE_PERSISTING_ENGINE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_STATE_PERSISTING_ENGINE_HPP_

#include "common/instrument_state.hpp"

namespace simulator::trading_system {

class StatePersistingEngine {
 public:
  StatePersistingEngine() = default;
  StatePersistingEngine(const StatePersistingEngine&) = delete;
  StatePersistingEngine(StatePersistingEngine&&) = delete;
  virtual ~StatePersistingEngine() = default;

  auto operator=(const StatePersistingEngine&)
      -> StatePersistingEngine& = delete;
  auto operator=(StatePersistingEngine&&) -> StatePersistingEngine& = delete;

  virtual auto store_state(market_state::InstrumentState& state) -> void = 0;

  virtual auto recover_state(market_state::InstrumentState state) -> void = 0;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_STATE_PERSISTING_ENGINE_HPP_
