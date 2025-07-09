#include <gmock/gmock.h>

#include "middleware/channels/trading_admin_channel.hpp"
#include "middleware/routing/trading_admin_channel.hpp"
#include "mocks/trading_admin_receiver_mock.hpp"

namespace simulator::middleware::test {
namespace {

using namespace testing;  // NOLINT

struct TradingAdminChannel : Test {
  auto bind_channel() -> void {
    std::shared_ptr<TradingAdminRequestReceiver> receiver_pointer{
        std::addressof(receiver), [](auto* /*pointer*/) {}};
    bind_trading_admin_channel(receiver_pointer);
  }

  TradingAdminRequestReceiverMock receiver;

 private:
  auto TearDown() -> void override { release_trading_admin_channel(); }
};

TEST_F(TradingAdminChannel, SendsSyncHaltPhaseRequest) {
  bind_channel();
  constexpr protocol::HaltPhaseRequest request;
  protocol::HaltPhaseReply reply;

  EXPECT_CALL(receiver,
              process(A<const protocol::HaltPhaseRequest&>(),
                      A<protocol::HaltPhaseReply&>()));

  ASSERT_NO_THROW(send_admin_request(request, reply));
}

TEST_F(TradingAdminChannel, SendsSyncResumePhaseRequest) {
  bind_channel();
  constexpr protocol::ResumePhaseRequest request;
  protocol::ResumePhaseReply reply;

  EXPECT_CALL(receiver,
              process(A<const protocol::ResumePhaseRequest&>(),
                      A<protocol::ResumePhaseReply&>()));

  ASSERT_NO_THROW(send_admin_request(request, reply));
}

TEST_F(TradingAdminChannel, SendsSyncStoreMarketStateRequest) {
  bind_channel();
  constexpr protocol::StoreMarketStateRequest request;
  protocol::StoreMarketStateReply reply;

  EXPECT_CALL(receiver,
              process(A<const protocol::StoreMarketStateRequest&>(),
                      A<protocol::StoreMarketStateReply&>()));

  ASSERT_NO_THROW(send_admin_request(request, reply));
}

TEST_F(TradingAdminChannel, SendsSyncRecoverMarketStateRequest) {
  bind_channel();
  constexpr protocol::RecoverMarketStateRequest request;
  protocol::RecoverMarketStateReply reply;

  EXPECT_CALL(receiver,
              process(A<const protocol::RecoverMarketStateRequest&>(),
                      A<protocol::RecoverMarketStateReply&>()));

  ASSERT_NO_THROW(send_admin_request(request, reply));
}

TEST_F(TradingAdminChannel, ReportsChannelNotBoundWhenSendingSyncRequest) {
  constexpr protocol::HaltPhaseRequest request;
  protocol::HaltPhaseReply reply;

  ASSERT_THROW(send_admin_request(request, reply), ChannelUnboundError);
}

}  // namespace
}  // namespace simulator::middleware::test
