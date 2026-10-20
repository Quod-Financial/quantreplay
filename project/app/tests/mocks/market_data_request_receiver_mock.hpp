#ifndef SIMULATOR_APP_TESTS_MOCKS_MARKET_DATA_REQUEST_RECEIVER_MOCK_HPP_
#define SIMULATOR_APP_TESTS_MOCKS_MARKET_DATA_REQUEST_RECEIVER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/market_data_request_channel.hpp"
#include "protocol/app/market_data_request.hpp"

namespace simulator::test {

struct MarketDataRequestReceiverMock
    : public middleware::MarketDataRequestReceiver {
  MOCK_METHOD(void, process, (protocol::MarketDataRequest), (override));
};

}  // namespace simulator::test

#endif  // SIMULATOR_APP_TESTS_MOCKS_MARKET_DATA_REQUEST_RECEIVER_MOCK_HPP_
