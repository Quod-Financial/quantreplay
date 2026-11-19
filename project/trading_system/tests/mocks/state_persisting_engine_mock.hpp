#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_STATE_PERSISTING_ENGINE_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_STATE_PERSISTING_ENGINE_MOCK_HPP_

#include <gmock/gmock.h>

#include "common/state_persisting_engine.hpp"

namespace simulator::trading_system::test {

struct StatePersistingEngineMock : StatePersistingEngine {
  // clang-format off
  MOCK_METHOD(void, store_state, (market_state::InstrumentState& state), (override));
  MOCK_METHOD(void, recover_state, (market_state::InstrumentState state), (override));
  // clang-format on
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_STATE_PERSISTING_ENGINE_MOCK_HPP_
