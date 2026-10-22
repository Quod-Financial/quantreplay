#ifndef SIMULATOR_TRADING_SYSTEM_ACCEPTOR_TESTS_MOCKS_TRADING_REQUEST_RECEIVER_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_ACCEPTOR_TESTS_MOCKS_TRADING_REQUEST_RECEIVER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/trading_request_channel.hpp"

namespace simulator::fix::trading_system_acceptor::test {

struct TradingRequestReceiverMock : public middleware::TradingRequestReceiver {
  // clang-format off
  MOCK_METHOD(void, process, (protocol::OrderPlacementRequest), (override));
  MOCK_METHOD(void, process, (protocol::OrderModificationRequest), (override));
  MOCK_METHOD(void, process, (protocol::OrderCancellationRequest), (override));
  MOCK_METHOD(void, process, (protocol::MarketDataRequest), (override));
  MOCK_METHOD(void, process, (protocol::SecurityStatusRequest), (override));
  // clang-format on
};

}  // namespace simulator::fix::trading_system_acceptor::test

#endif  // SIMULATOR_TRADING_SYSTEM_ACCEPTOR_TESTS_MOCKS_TRADING_REQUEST_RECEIVER_MOCK_HPP_