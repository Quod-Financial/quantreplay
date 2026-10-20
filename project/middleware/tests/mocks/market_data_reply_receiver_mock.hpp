#ifndef SIMULATOR_MIDDLEWARE_TESTS_MOCKS_MARKET_DATA_REPLY_RECEIVER_MOCK_HPP_
#define SIMULATOR_MIDDLEWARE_TESTS_MOCKS_MARKET_DATA_REPLY_RECEIVER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/market_data_reply_channel.hpp"

namespace simulator::middleware::test {

struct MarketDataReplyReceiverMock : public MarketDataReplyReceiver {
  // clang-format off
  MOCK_METHOD(void, process, (protocol::MarketDataSnapshot), (override));
  MOCK_METHOD(void, process, (protocol::MarketDataUpdate), (override));
  MOCK_METHOD(void, process, (protocol::MarketDataReject), (override));
  // clang-format on
};

}  // namespace simulator::middleware::test

#endif  // SIMULATOR_MIDDLEWARE_TESTS_MOCKS_MARKET_DATA_REPLY_RECEIVER_MOCK_HPP_
