#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "middleware/channels/trading_session_event_channel.hpp"
#include "middleware/routing/trading_session_event_channel.hpp"
#include "mocks/trading_session_event_listener_mock.hpp"
#include "test_utils/protocol_utils.hpp"

namespace simulator::middleware::test {
namespace {

using namespace testing;  // NOLINT

struct TradingSessionEventChannel : public Test {
  auto bind_channel() -> void {
    std::shared_ptr<TradingSessionEventListener> listener_pointer{
        std::addressof(listener), [](auto* /*pointer*/) {}};
    bind_trading_session_event_channel(listener_pointer);
  }

  StrictMock<TradingSessionEventListenerMock> listener;

 private:
  auto TearDown() -> void override { release_trading_session_event_channel(); }
};

TEST_F(TradingSessionEventChannel, EmitsSessionTerminatedEvent) {
  bind_channel();
  const auto event = make_session_event<protocol::SessionTerminatedEvent>();

  EXPECT_CALL(listener, on_event(A<const protocol::SessionTerminatedEvent&>()))
      .Times(1);
  ASSERT_NO_THROW(emit_trading_session_event(event));
}

TEST_F(TradingSessionEventChannel, ReportsChannelNotBoundWhileEmittingEvent) {
  const auto event = make_session_event<protocol::SessionTerminatedEvent>();

  ASSERT_THROW(emit_trading_session_event(event), ChannelUnboundError);
}

}  // namespace
}  // namespace simulator::middleware::test