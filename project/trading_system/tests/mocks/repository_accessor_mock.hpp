#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_REQUEST_EXECUTOR_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_REQUEST_EXECUTOR_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/repository/repository_accessor.hpp"

namespace simulator::trading_system::test {

struct RepositoryAccessorMock : public RepositoryAccessor {
  // clang-format off
  MOCK_METHOD(void, unicast_impl, (InstrumentId, tl::function_ref<void(TradingEngine&)>), (const override));
  MOCK_METHOD(void, broadcast_impl, (tl::function_ref<void(TradingEngine&)>), (const override));
  // clang-format on
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_REQUEST_EXECUTOR_MOCK_HPP_
