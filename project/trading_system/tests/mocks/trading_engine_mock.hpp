#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_TRADING_ENGINE_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_TRADING_ENGINE_MOCK_HPP_

#include <gmock/gmock.h>

#include "common/events.hpp"
#include "common/instrument.hpp"
#include "common/trading_engine.hpp"
#include "core/domain/attributes.hpp"

namespace simulator::trading_system::test {

struct TradingEngineMock : TradingEngine {
  // clang-format off
  MOCK_METHOD(void, execute, (protocol::OrderPlacementRequest request), (override));
  MOCK_METHOD(void, execute, (protocol::OrderModificationRequest request), (override));
  MOCK_METHOD(void, execute, (protocol::OrderCancellationRequest request), (override));
  MOCK_METHOD(void, execute, (protocol::MarketDataRequest), (override));
  MOCK_METHOD(void, execute, (protocol::SecurityStatusRequest), (override));
  MOCK_METHOD(void, provide_state, (protocol::InstrumentState & reply), (override));
  MOCK_METHOD(void, store_state, (market_state::InstrumentState& state), (override));
  MOCK_METHOD(void, recover_state, (market_state::InstrumentState event), (override));
  MOCK_METHOD(void, handle, (event::Tick event), (override));
  MOCK_METHOD(void, handle, (event::PhaseTransition event), (override));
  MOCK_METHOD(void, handle, (const protocol::SessionTerminatedEvent& event), (override));
  // clang-format on
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_TRADING_ENGINE_MOCK_HPP_
