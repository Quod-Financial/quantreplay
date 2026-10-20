#ifndef SIMULATOR_MIDDLEWARE_TESTS_MOCKS_MARKET_DATA_REQUEST_RECEIVER_MOCK_HPP_
#define SIMULATOR_MIDDLEWARE_TESTS_MOCKS_MARKET_DATA_REQUEST_RECEIVER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/market_data_request_channel.hpp"

namespace simulator::middleware::test {

struct MarketDataRequestReceiverMock : public MarketDataRequestReceiver {
  MOCK_METHOD(void, process, (protocol::MarketDataRequest), (override));
};

}  // namespace simulator::middleware::test

#endif  // SIMULATOR_MIDDLEWARE_TESTS_MOCKS_MARKET_DATA_REQUEST_RECEIVER_MOCK_HPP_
