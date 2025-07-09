#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "middleware/channels/trading_reply_channel.hpp"
#include "middleware/routing/trading_reply_channel.hpp"
#include "mocks/trading_reply_receiver_mock.hpp"
#include "test_utils/protocol_utils.hpp"

namespace simulator::middleware::test {
namespace {

using namespace testing;  // NOLINT

template <typename>
struct TradingReplyChannel : public Test {
  auto bind_channel() -> void {
    std::shared_ptr<TradingReplyReceiver> receiver_pointer{
        std::addressof(receiver), [](auto* /*pointer*/) {}};
    bind_trading_reply_channel(receiver_pointer);
  }

  StrictMock<TradingReplyReceiverMock> receiver;

 private:
  auto TearDown() -> void override { release_trading_reply_channel(); }
};

using Messages = testing::Types<protocol::ExecutionReport,
                                protocol::OrderPlacementReject,
                                protocol::OrderPlacementConfirmation,
                                protocol::OrderModificationReject,
                                protocol::OrderModificationConfirmation,
                                protocol::OrderCancellationReject,
                                protocol::OrderCancellationConfirmation,
                                protocol::MarketDataReject,
                                protocol::MarketDataSnapshot,
                                protocol::MarketDataUpdate>;

TYPED_TEST_SUITE(TradingReplyChannel, Messages);

TYPED_TEST(TradingReplyChannel, SendsReply) {
  this->bind_channel();
  const auto reply = make_app_message<TypeParam>();

  EXPECT_CALL(this->receiver, process(A<TypeParam>())).Times(1);
  EXPECT_NO_THROW(send_trading_reply(reply));
}

TYPED_TEST(TradingReplyChannel, ReportsChannelNotBoundWhenSendingReply) {
  const auto reply = make_app_message<TypeParam>();

  EXPECT_THROW(send_trading_reply(reply), ChannelUnboundError);
}

}  // namespace
}  // namespace simulator::middleware::test