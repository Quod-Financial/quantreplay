#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_TRADING_ENGINE_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_TRADING_ENGINE_MOCK_HPP_

#include <gmock/gmock.h>

#include "common/trading_engine.hpp"

namespace simulator::trading_system::test {

struct TradingEngineMock : TradingEngine {
  // clang-format off
  MOCK_METHOD(void, execute, (protocol::OrderPlacementRequest request), (override));
  MOCK_METHOD(void, execute, (protocol::MarketDataRequest), (override));
  MOCK_METHOD(void, execute, (protocol::SecurityStatusRequest), (override));
  MOCK_METHOD(void, handle, (const protocol::SessionTerminatedEvent& event), (override));
  // clang-format on
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_TRADING_ENGINE_MOCK_HPP_
