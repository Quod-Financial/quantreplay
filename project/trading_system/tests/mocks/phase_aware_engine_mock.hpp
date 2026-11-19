#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_PHASE_AWARE_ENGINE_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_PHASE_AWARE_ENGINE_MOCK_HPP_

#include <gmock/gmock.h>

#include "common/events.hpp"
#include "common/phase_aware_engine.hpp"

namespace simulator::trading_system::test {

struct PhaseAwareEngineMock : PhaseAwareEngine {
  // clang-format off
  MOCK_METHOD(void, handle, (event::Tick event), (override));
  MOCK_METHOD(void, handle, (event::PhaseTransition event), (override));
  // clang-format on
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_PHASE_AWARE_ENGINE_MOCK_HPP_
