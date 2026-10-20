#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_TRADING_REQUEST_CHANNEL_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_TRADING_REQUEST_CHANNEL_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/trading_request_channel.hpp"

namespace simulator::generator::mock {

struct TradingRequestReceiver
    : public simulator::middleware::TradingRequestReceiver {
  // clang-format off
  MOCK_METHOD(void, process, (simulator::protocol::OrderPlacementRequest), (override));
  MOCK_METHOD(void, process, (simulator::protocol::OrderModificationRequest), (override));
  MOCK_METHOD(void, process, (simulator::protocol::OrderCancellationRequest), (override));
  MOCK_METHOD(void, process, (simulator::protocol::MarketDataRequest), (override));
  MOCK_METHOD(void, process, (simulator::protocol::SecurityStatusRequest), (override));
  // clang-format on
};

// clang-format on

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_TRADING_REQUEST_CHANNEL_HPP_
