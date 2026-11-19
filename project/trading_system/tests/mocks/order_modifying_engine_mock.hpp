#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_ORDER_MODIFYING_ENGINE_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_ORDER_MODIFYING_ENGINE_MOCK_HPP_

#include <gmock/gmock.h>

#include "common/order_modifying_engine.hpp"

namespace simulator::trading_system::test {

struct OrderModifyingEngineMock : OrderModifyingEngine {
  // clang-format off
  MOCK_METHOD(void, execute, (protocol::OrderModificationRequest request), (override));
  MOCK_METHOD(void, execute, (protocol::OrderCancellationRequest request), (override));
  // clang-format on
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_ORDER_MODIFYING_ENGINE_MOCK_HPP_
