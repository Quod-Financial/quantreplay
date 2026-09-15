#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "middleware/channels/market_data_reply_channel.hpp"
#include "middleware/routing/errors.hpp"
#include "middleware/routing/market_data_reply_channel.hpp"
#include "mocks/market_data_reply_receiver_mock.hpp"
#include "test_utils/protocol_utils.hpp"

namespace simulator::middleware::test {
namespace {

using namespace testing;  // NOLINT

template <typename>
struct MarketDataReplyChannel : public Test {
  auto bind_channel() -> void {
    std::shared_ptr<MarketDataReplyReceiver> receiver_pointer{
        std::addressof(receiver), [](auto* /*pointer*/) {}};
    bind_market_data_reply_channel(receiver_pointer);
  }

  StrictMock<MarketDataReplyReceiverMock> receiver;

 private:
  auto TearDown() -> void override { release_market_data_reply_channel(); }
};

using Messages = testing::Types<protocol::MarketDataSnapshot,
                                protocol::MarketDataUpdate,
                                protocol::MarketDataReject>;

TYPED_TEST_SUITE(MarketDataReplyChannel, Messages);

TYPED_TEST(MarketDataReplyChannel,
           ReportsChannelNotBoundWhenSendingMarketDataReply) {
  const auto message = make_app_message<TypeParam>();

  ASSERT_THROW(send_market_data_reply(message), ChannelUnboundError);
}

TYPED_TEST(MarketDataReplyChannel, ReportsChannelNotBoundAfterRelease) {
  this->bind_channel();
  release_market_data_reply_channel();
  const auto message = make_app_message<TypeParam>();

  ASSERT_THROW(send_market_data_reply(message), ChannelUnboundError);
}

TYPED_TEST(MarketDataReplyChannel, SendsMarketDataReply) {
  this->bind_channel();
  const auto message = make_app_message<TypeParam>();

  EXPECT_CALL(this->receiver, process(A<TypeParam>())).Times(1);
  ASSERT_NO_THROW(send_market_data_reply(message));
}

}  // namespace
}  // namespace simulator::middleware::test
