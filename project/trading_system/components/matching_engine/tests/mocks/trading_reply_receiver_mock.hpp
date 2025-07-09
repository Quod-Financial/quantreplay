#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_TRADING_REPLY_CHANNEL_MOCK_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_TRADING_REPLY_CHANNEL_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/trading_reply_channel.hpp"

namespace simulator::trading_system::matching_engine {

struct TradingReplyReceiverMock : public middleware::TradingReplyReceiver {
  TradingReplyReceiverMock();
  ~TradingReplyReceiverMock() override;

  TradingReplyReceiverMock(TradingReplyReceiverMock const&) = delete;
  TradingReplyReceiverMock(TradingReplyReceiverMock&&) = delete;

  auto operator=(TradingReplyReceiverMock const&)
      -> TradingReplyReceiverMock& = delete;

  auto operator=(TradingReplyReceiverMock&&)
      -> TradingReplyReceiverMock& = delete;

  // clang-format off
  MOCK_METHOD(void, process, (protocol::BusinessMessageReject), (override));
  MOCK_METHOD(void, process, (protocol::ExecutionReport), (override));

  MOCK_METHOD(void, process, (protocol::OrderPlacementReject), (override));
  MOCK_METHOD(void, process, (protocol::OrderPlacementConfirmation), (override));

  MOCK_METHOD(void, process, (protocol::OrderModificationReject), (override));
  MOCK_METHOD(void, process, (protocol::OrderModificationConfirmation), (override));

  MOCK_METHOD(void, process, (protocol::OrderCancellationReject), (override));
  MOCK_METHOD(void, process, (protocol::OrderCancellationConfirmation), (override));

  MOCK_METHOD(void, process, (protocol::MarketDataReject), (override));
  MOCK_METHOD(void, process, (protocol::MarketDataSnapshot), (override));
  MOCK_METHOD(void, process, (protocol::MarketDataUpdate), (override));
  MOCK_METHOD(void, process, (protocol::SecurityStatus), (override));
  // clang-format on
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_TRADING_REPLY_CHANNEL_MOCK_HPP_