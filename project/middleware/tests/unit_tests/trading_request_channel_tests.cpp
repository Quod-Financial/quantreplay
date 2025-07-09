#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "middleware/channels/trading_request_channel.hpp"
#include "middleware/routing/trading_request_channel.hpp"
#include "mocks/trading_request_receiver_mock.hpp"
#include "test_utils/protocol_utils.hpp"

namespace simulator::middleware::test {
namespace {

using namespace testing;  // NOLINT

struct TradingRequestChannel : Test {
  auto bind_channel() -> void {
    std::shared_ptr<TradingRequestReceiver> receiver_pointer{
        std::addressof(receiver), [](auto* /*pointer*/) {}};
    bind_trading_request_channel(receiver_pointer);
  }

  StrictMock<TradingRequestReceiverMock> receiver;

 private:
  auto TearDown() -> void override { release_trading_request_channel(); }
};

TEST_F(TradingRequestChannel, SendsAsyncOrderPlacementRequest) {
  bind_channel();
  const auto request = make_app_message<protocol::OrderPlacementRequest>();

  EXPECT_CALL(receiver, process(A<protocol::OrderPlacementRequest>())).Times(1);
  ASSERT_NO_THROW(send_trading_request(request));
}

TEST_F(TradingRequestChannel, SendsAsyncOrderModificationRequest) {
  bind_channel();
  const auto request = make_app_message<protocol::OrderModificationRequest>();

  EXPECT_CALL(receiver, process(A<protocol::OrderModificationRequest>()))
      .Times(1);
  ASSERT_NO_THROW(send_trading_request(request));
}

TEST_F(TradingRequestChannel, SendsAsyncOrderCancellationRequest) {
  bind_channel();
  const auto request = make_app_message<protocol::OrderCancellationRequest>();

  EXPECT_CALL(receiver, process(A<protocol::OrderCancellationRequest>()))
      .Times(1);
  ASSERT_NO_THROW(send_trading_request(request));
}

TEST_F(TradingRequestChannel, SendsAsyncMarketDataRequest) {
  bind_channel();
  const auto request = make_app_message<protocol::MarketDataRequest>();

  EXPECT_CALL(receiver, process(A<protocol::MarketDataRequest>())).Times(1);
  ASSERT_NO_THROW(send_trading_request(request));
}

TEST_F(TradingRequestChannel, SendsSyncInstrumentStateRequest) {
  bind_channel();
  protocol::InstrumentStateRequest request;
  protocol::InstrumentState reply;

  EXPECT_CALL(receiver,
              process(A<const protocol::InstrumentStateRequest&>(),
                      A<protocol::InstrumentState&>()))
      .Times(1);
  ASSERT_NO_THROW(send_trading_request(request, reply));
}

TEST_F(TradingRequestChannel, ReportsChannelNotBoundWhenSendingAsyncRequest) {
  const auto request = make_app_message<protocol::OrderPlacementRequest>();

  ASSERT_THROW(send_trading_request(request), ChannelUnboundError);
}

TEST_F(TradingRequestChannel, ReportsChannelNotBoundWhenSendingSyncRequest) {
  protocol::InstrumentStateRequest request;
  protocol::InstrumentState reply;

  ASSERT_THROW(send_trading_request(request, reply), ChannelUnboundError);
}

}  // namespace
}  // namespace simulator::middleware::test