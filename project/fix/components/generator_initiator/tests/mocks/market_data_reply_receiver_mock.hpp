#ifndef SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_MARKET_DATA_REPLY_RECEIVER_MOCK_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_MARKET_DATA_REPLY_RECEIVER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/market_data_reply_channel.hpp"

namespace simulator::fix::generator_initiator::test {

struct MarketDataReplyReceiverMock
    : public middleware::MarketDataReplyReceiver {
  // clang-format off
  MOCK_METHOD(void, process, (protocol::MarketDataSnapshot), (override));
  MOCK_METHOD(void, process, (protocol::MarketDataUpdate), (override));
  MOCK_METHOD(void, process, (protocol::MarketDataReject), (override));
  // clang-format on
};

}  // namespace simulator::fix::generator_initiator::test

#endif  // SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_MARKET_DATA_REPLY_RECEIVER_MOCK_HPP_
