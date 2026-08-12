#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "middleware/channels/trading_session_event_channel.hpp"
#include "middleware/routing/errors.hpp"
#include "middleware/routing/trading_session_event_channel.hpp"
#include "mocks/trading_session_event_listener_mock.hpp"
#include "test_utils/protocol_utils.hpp"

namespace simulator::middleware::test {
namespace {

using namespace testing;  // NOLINT

struct TradingSessionConnectionEventChannel : public Test {
  auto bind_channel() -> void {
    std::shared_ptr<TradingSessionConnectionEventListener> listener_pointer{
        std::addressof(listener), [](auto* /*pointer*/) {}};
    bind_trading_session_connection_event_channel(listener_pointer);
  }

  StrictMock<TradingSessionConnectionEventListenerMock> listener;

 private:
  auto TearDown() -> void override {
    release_trading_session_connection_event_channel();
  }
};

TEST_F(TradingSessionConnectionEventChannel, EmitsSessionConnectedEvent) {
  bind_channel();
  const auto event = make_session_event<protocol::SessionConnectedEvent>();

  EXPECT_CALL(listener, on_event(A<const protocol::SessionConnectedEvent&>()))
      .Times(1);
  ASSERT_NO_THROW(emit_trading_session_event(event));
}

TEST_F(TradingSessionConnectionEventChannel,
       ReportsChannelNotBoundWhileEmittingEvent) {
  const auto event = make_session_event<protocol::SessionConnectedEvent>();

  ASSERT_THROW(emit_trading_session_event(event), ChannelUnboundError);
}

struct TradingSessionTerminationEventChannel : public Test {
  auto bind_channel() -> void {
    std::shared_ptr<TradingSessionTerminationEventListener> listener_pointer{
        std::addressof(listener), [](auto* /*pointer*/) {}};
    bind_trading_session_termination_event_channel(listener_pointer);
  }

  StrictMock<TradingSessionTerminationEventListenerMock> listener;

 private:
  auto TearDown() -> void override {
    release_trading_session_termination_event_channel();
  }
};

TEST_F(TradingSessionTerminationEventChannel, EmitsSessionTerminatedEvent) {
  bind_channel();
  const auto event = make_session_event<protocol::SessionTerminatedEvent>();

  EXPECT_CALL(listener, on_event(A<const protocol::SessionTerminatedEvent&>()))
      .Times(1);
  ASSERT_NO_THROW(emit_trading_session_event(event));
}

TEST_F(TradingSessionTerminationEventChannel,
       ReportsChannelNotBoundWhileEmittingEvent) {
  const auto event = make_session_event<protocol::SessionTerminatedEvent>();

  ASSERT_THROW(emit_trading_session_event(event), ChannelUnboundError);
}

}  // namespace
}  // namespace simulator::middleware::test
