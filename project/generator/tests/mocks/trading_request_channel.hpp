#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_TRADING_REQUEST_CHANNEL_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_TRADING_REQUEST_CHANNEL_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/trading_request_channel.hpp"

namespace simulator::generator::mock {

struct TradingRequestReceiver
    : public simulator::middleware::TradingRequestReceiver {
  // clang-format off
  MOCK_METHOD(void, process, (simulator::protocol::OrderPlacementRequest));
  MOCK_METHOD(void, process, (simulator::protocol::OrderModificationRequest));
  MOCK_METHOD(void, process, (simulator::protocol::OrderCancellationRequest));
  MOCK_METHOD(void, process, (simulator::protocol::MarketDataRequest));
  MOCK_METHOD(void, process, (simulator::protocol::SecurityStatusRequest));
  MOCK_METHOD(void, process, (const simulator::protocol::InstrumentStateRequest&, simulator::protocol::InstrumentState&));
  // clang-format on
};

// clang-format on

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_TRADING_REQUEST_CHANNEL_HPP_
