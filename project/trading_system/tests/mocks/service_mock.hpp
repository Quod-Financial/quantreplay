#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_SERVICE_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_SERVICE_MOCK_HPP_

#include <gmock/gmock.h>

#include <functional>

#include "runtime/service.hpp"

namespace simulator::trading_system::test {

struct ServiceMock : runtime::Service {
  MOCK_METHOD(void, execute, (std::function<void()> task), (override));
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_SERVICE_MOCK_HPP_
