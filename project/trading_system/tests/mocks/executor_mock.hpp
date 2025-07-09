#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_EXECUTOR_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_EXECUTOR_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/execution/execution_system.hpp"

namespace simulator::trading_system::test {

struct ExecutorMock : public Executor {
  MOCK_METHOD(void,
              execute_request,
              (protocol::OrderPlacementRequest),
              (const, override));
  MOCK_METHOD(void,
              execute_request,
              (protocol::OrderModificationRequest),
              (const, override));
  MOCK_METHOD(void,
              execute_request,
              (protocol::OrderCancellationRequest),
              (const, override));
  MOCK_METHOD(void,
              execute_request,
              (protocol::MarketDataRequest),
              (const, override));
  MOCK_METHOD(void,
              execute_request,
              (protocol::SecurityStatusRequest),
              (const, override));
  MOCK_METHOD(void,
              execute_request,
              (const protocol::InstrumentStateRequest&,
               protocol::InstrumentState&),
              (const, override));
  MOCK_METHOD(void,
              store_state_request,
              (std::vector<market_state::InstrumentState>&),
              (const, override));
  MOCK_METHOD(void,
              recover_state_request,
              (std::vector<market_state::InstrumentState>),
              (const, override));
  MOCK_METHOD(void,
              handle,
              (const protocol::SessionTerminatedEvent&),
              (const, override));
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_EXECUTOR_MOCK_HPP_
