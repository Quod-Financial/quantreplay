#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "middleware/channels/generator_initiator_event_channel.hpp"
#include "middleware/channels/trading_session_event_channel.hpp"
#include "middleware/routing/errors.hpp"
#include "middleware/routing/generator_initiator_event_channel.hpp"
#include "middleware/routing/trading_session_event_channel.hpp"
#include "mocks/generator_initiator_event_listener_mock.hpp"
#include "mocks/trading_session_event_listener_mock.hpp"
#include "test_utils/protocol_utils.hpp"

namespace simulator::middleware::test {
namespace {

using namespace testing;  // NOLINT

struct GeneratorInitiatorConnectionEventChannel : public Test {
  auto bind_channel() -> void {
    std::shared_ptr<GeneratorInitiatorConnectionEventListener> listener_pointer{
        std::addressof(listener), [](auto* /*pointer*/) {}};
    bind_generator_initiator_connection_event_channel(listener_pointer);
  }

  auto bind_trading_channel() -> void {
    std::shared_ptr<TradingSessionConnectionEventListener> listener_pointer{
        std::addressof(trading_listener), [](auto* /*pointer*/) {}};
    bind_trading_session_connection_event_channel(listener_pointer);
  }

  StrictMock<GeneratorInitiatorConnectionEventListenerMock> listener;
  StrictMock<TradingSessionConnectionEventListenerMock> trading_listener;

 private:
  auto TearDown() -> void override {
    release_generator_initiator_connection_event_channel();
    release_trading_session_connection_event_channel();
  }
};

TEST_F(GeneratorInitiatorConnectionEventChannel,
       ReportsChannelNotBoundWhileEmittingEvent) {
  const auto event = make_session_event<protocol::SessionConnectedEvent>();

  ASSERT_THROW(emit_generator_initiator_event(event), ChannelUnboundError);
}

TEST_F(GeneratorInitiatorConnectionEventChannel,
       ReportsChannelNotBoundAfterRelease) {
  bind_channel();
  release_generator_initiator_connection_event_channel();
  const auto event = make_session_event<protocol::SessionConnectedEvent>();

  ASSERT_THROW(emit_generator_initiator_event(event), ChannelUnboundError);
}

TEST_F(GeneratorInitiatorConnectionEventChannel, EmitsSessionConnectedEvent) {
  bind_channel();
  const auto event = make_session_event<protocol::SessionConnectedEvent>();

  EXPECT_CALL(listener, on_event(A<const protocol::SessionConnectedEvent&>()))
      .Times(1);
  ASSERT_NO_THROW(emit_generator_initiator_event(event));
}

TEST_F(GeneratorInitiatorConnectionEventChannel,
       DoesNotEmitSessionConnectedEventToTradingSessionEventChannel) {
  bind_channel();
  bind_trading_channel();
  const auto event = make_session_event<protocol::SessionConnectedEvent>();

  EXPECT_CALL(listener, on_event(A<const protocol::SessionConnectedEvent&>()))
      .Times(1);
  EXPECT_CALL(trading_listener,
              on_event(A<const protocol::SessionConnectedEvent&>()))
      .Times(0);
  ASSERT_NO_THROW(emit_generator_initiator_event(event));
}

struct GeneratorInitiatorTerminationEventChannel : public Test {
  auto bind_channel() -> void {
    std::shared_ptr<GeneratorInitiatorTerminationEventListener>
        listener_pointer{std::addressof(listener), [](auto* /*pointer*/) {}};
    bind_generator_initiator_termination_event_channel(listener_pointer);
  }

  auto bind_trading_channel() -> void {
    std::shared_ptr<TradingSessionTerminationEventListener> listener_pointer{
        std::addressof(trading_listener), [](auto* /*pointer*/) {}};
    bind_trading_session_termination_event_channel(listener_pointer);
  }

  StrictMock<GeneratorInitiatorTerminationEventListenerMock> listener;
  StrictMock<TradingSessionTerminationEventListenerMock> trading_listener;

 private:
  auto TearDown() -> void override {
    release_generator_initiator_termination_event_channel();
    release_trading_session_termination_event_channel();
  }
};

TEST_F(GeneratorInitiatorTerminationEventChannel,
       ReportsChannelNotBoundWhileEmittingEvent) {
  const auto event = make_session_event<protocol::SessionTerminatedEvent>();

  ASSERT_THROW(emit_generator_initiator_event(event), ChannelUnboundError);
}

TEST_F(GeneratorInitiatorTerminationEventChannel,
       ReportsChannelNotBoundAfterRelease) {
  bind_channel();
  release_generator_initiator_termination_event_channel();
  const auto event = make_session_event<protocol::SessionTerminatedEvent>();

  ASSERT_THROW(emit_generator_initiator_event(event), ChannelUnboundError);
}

TEST_F(GeneratorInitiatorTerminationEventChannel, EmitsSessionTerminatedEvent) {
  bind_channel();
  const auto event = make_session_event<protocol::SessionTerminatedEvent>();

  EXPECT_CALL(listener, on_event(A<const protocol::SessionTerminatedEvent&>()))
      .Times(1);
  ASSERT_NO_THROW(emit_generator_initiator_event(event));
}

TEST_F(GeneratorInitiatorTerminationEventChannel,
       DoesNotEmitSessionTerminatedEventToTradingSessionEventChannel) {
  bind_channel();
  bind_trading_channel();
  const auto event = make_session_event<protocol::SessionTerminatedEvent>();

  EXPECT_CALL(listener, on_event(A<const protocol::SessionTerminatedEvent&>()))
      .Times(1);
  EXPECT_CALL(trading_listener,
              on_event(A<const protocol::SessionTerminatedEvent&>()))
      .Times(0);
  ASSERT_NO_THROW(emit_generator_initiator_event(event));
}

}  // namespace
}  // namespace simulator::middleware::test
