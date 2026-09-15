#include <gmock/gmock.h>
#include <quickfix/SessionID.h>

#include <memory>

#include "ih/processors/session_event_processor.hpp"
#include "mocks/generator_initiator_event_listener_mock.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix::generator_initiator::test {
namespace {

using namespace testing;  // NOLINT

struct InitiatorSessionEventProcessor : public Test {
  [[nodiscard]]
  static auto expected_session() -> protocol::Session {
    return protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{"SenderCompID"},
                               protocol::fix::TargetCompId{"TargetCompID"}}};
  }

  auto bind_channels() -> void {
    middleware::bind_generator_initiator_connection_event_channel(
        std::shared_ptr<middleware::GeneratorInitiatorConnectionEventListener>{
            std::addressof(connection_listener), [](auto* /*pointer*/) {}});
    middleware::bind_generator_initiator_termination_event_channel(
        std::shared_ptr<middleware::GeneratorInitiatorTerminationEventListener>{
            std::addressof(termination_listener), [](auto* /*pointer*/) {}});
  }

  StrictMock<GeneratorInitiatorConnectionEventListenerMock> connection_listener;
  StrictMock<GeneratorInitiatorTerminationEventListenerMock>
      termination_listener;
  SessionEventProcessor processor;
  FIX::SessionID fix_session{"FIXT.1.1", "SenderCompID", "TargetCompID"};

 private:
  auto TearDown() -> void override {
    middleware::release_generator_initiator_connection_event_channel();
    middleware::release_generator_initiator_termination_event_channel();
  }
};

TEST_F(InitiatorSessionEventProcessor, EmitsSessionConnectionEvent) {
  bind_channels();

  EXPECT_CALL(connection_listener,
              on_event(Field(&protocol::SessionConnectedEvent::session,
                             Eq(expected_session()))))
      .Times(1);
  processor.process_session_connection(fix_session);
}

TEST_F(InitiatorSessionEventProcessor, EmitsSessionDisconnectionEvent) {
  bind_channels();

  EXPECT_CALL(termination_listener,
              on_event(Field(&protocol::SessionTerminatedEvent::session,
                             Eq(expected_session()))))
      .Times(1);
  processor.process_session_disconnection(fix_session);
}

}  // namespace
}  // namespace simulator::fix::generator_initiator::test
