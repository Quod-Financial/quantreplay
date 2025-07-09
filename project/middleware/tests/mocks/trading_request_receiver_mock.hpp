#ifndef SIMULATOR_MIDDLEWARE_TESTS_MOCKS_TRADING_REQUEST_RECEIVER_MOCK_HPP_
#define SIMULATOR_MIDDLEWARE_TESTS_MOCKS_TRADING_REQUEST_RECEIVER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/trading_request_channel.hpp"

namespace simulator::middleware::test {

struct TradingRequestReceiverMock : public TradingRequestReceiver {
  // clang-format off
  MOCK_METHOD(void, process, (protocol::OrderPlacementRequest), (override));
  MOCK_METHOD(void, process, (protocol::OrderModificationRequest), (override));
  MOCK_METHOD(void, process, (protocol::OrderCancellationRequest), (override));
  MOCK_METHOD(void, process, (protocol::MarketDataRequest), (override));
  MOCK_METHOD(void, process, (protocol::SecurityStatusRequest), (override));

  MOCK_METHOD(void, process, (const protocol::InstrumentStateRequest&, protocol::InstrumentState&), (override));
  // clang-format on
};

}  // namespace simulator::middleware::test

#endif  // SIMULATOR_MIDDLEWARE_TESTS_MOCKS_TRADING_REQUEST_RECEIVER_MOCK_HPP_