#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "middleware/channels/market_data_request_channel.hpp"
#include "middleware/channels/trading_request_channel.hpp"
#include "middleware/routing/errors.hpp"
#include "middleware/routing/market_data_request_channel.hpp"
#include "mocks/market_data_request_receiver_mock.hpp"
#include "mocks/trading_request_receiver_mock.hpp"
#include "test_utils/protocol_utils.hpp"

namespace simulator::middleware::test {
namespace {

using namespace testing;  // NOLINT

struct MarketDataRequestChannel : Test {
  auto bind_channel() -> void {
    std::shared_ptr<MarketDataRequestReceiver> receiver_pointer{
        std::addressof(receiver), [](auto* /*pointer*/) {}};
    bind_market_data_request_channel(receiver_pointer);
  }

  auto bind_trading_channel() -> void {
    std::shared_ptr<TradingRequestReceiver> receiver_pointer{
        std::addressof(trading_request_receiver), [](auto* /*pointer*/) {}};
    bind_trading_request_channel(receiver_pointer);
  }

  StrictMock<MarketDataRequestReceiverMock> receiver;
  StrictMock<TradingRequestReceiverMock> trading_request_receiver;

 private:
  auto TearDown() -> void override {
    release_market_data_request_channel();
    release_trading_request_channel();
  }
};

TEST_F(MarketDataRequestChannel, ReportsChannelNotBoundWhenSendingRequest) {
  const auto request = make_app_message<protocol::MarketDataRequest>();

  ASSERT_THROW(send_market_data_request(request), ChannelUnboundError);
}

TEST_F(MarketDataRequestChannel, ReportsChannelNotBoundAfterRelease) {
  bind_channel();
  release_market_data_request_channel();
  const auto request = make_app_message<protocol::MarketDataRequest>();

  ASSERT_THROW(send_market_data_request(request), ChannelUnboundError);
}

TEST_F(MarketDataRequestChannel, SendsMarketDataRequest) {
  bind_channel();
  const auto request = make_app_message<protocol::MarketDataRequest>();

  EXPECT_CALL(receiver, process(A<protocol::MarketDataRequest>())).Times(1);
  ASSERT_NO_THROW(send_market_data_request(request));
}

TEST_F(MarketDataRequestChannel,
       DoesNotSendMarketDataRequestToTradingRequestChannel) {
  bind_channel();
  bind_trading_channel();
  const auto request = make_app_message<protocol::MarketDataRequest>();

  EXPECT_CALL(receiver, process(A<protocol::MarketDataRequest>())).Times(1);
  EXPECT_CALL(trading_request_receiver,
              process(A<protocol::MarketDataRequest>()))
      .Times(0);
  ASSERT_NO_THROW(send_market_data_request(request));
}

}  // namespace
}  // namespace simulator::middleware::test
