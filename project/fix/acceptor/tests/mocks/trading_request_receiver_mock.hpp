#ifndef SIMULATOR_ACCEPTOR_TESTS_MOCKS_TRADING_REQUEST_RECEIVER_MOCK_HPP_
#define SIMULATOR_ACCEPTOR_TESTS_MOCKS_TRADING_REQUEST_RECEIVER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/trading_request_channel.hpp"

namespace simulator::fix::acceptor::test {

struct TradingRequestReceiverMock : public middleware::TradingRequestReceiver {
  // clang-format off
  MOCK_METHOD(void, process, (protocol::OrderPlacementRequest));
  MOCK_METHOD(void, process, (protocol::OrderModificationRequest));
  MOCK_METHOD(void, process, (protocol::OrderCancellationRequest));
  MOCK_METHOD(void, process, (protocol::MarketDataRequest));
  MOCK_METHOD(void, process, (protocol::SecurityStatusRequest));
  MOCK_METHOD(void, process, (const protocol::InstrumentStateRequest&, protocol::InstrumentState&));
  // clang-format on
};

}  // namespace simulator::fix::acceptor::test

#endif  // SIMULATOR_ACCEPTOR_TESTS_MOCKS_TRADING_REQUEST_RECEIVER_MOCK_HPP_