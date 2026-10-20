#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_MARKET_DATA_REQUEST_CHANNEL_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_MARKET_DATA_REQUEST_CHANNEL_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/market_data_request_channel.hpp"

namespace simulator::generator::mock {

struct MarketDataRequestReceiver
    : public simulator::middleware::MarketDataRequestReceiver {
  // clang-format off
  MOCK_METHOD(void, process, (simulator::protocol::MarketDataRequest), (override));
  // clang-format on
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_MARKET_DATA_REQUEST_CHANNEL_HPP_
