#ifndef SIMULATOR_GENERATOR_INITIATOR_IH_PROCESSORS_SESSION_EVENT_PROCESSOR_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_IH_PROCESSORS_SESSION_EVENT_PROCESSOR_HPP_

#include <quickfix/SessionID.h>

#include "common/processors/event_processor.hpp"
#include "common/session_conversion.hpp"
#include "middleware/routing/generator_initiator_event_channel.hpp"

namespace simulator::fix::generator_initiator {

class SessionEventProcessor final : public EventProcessor {
 public:
  auto process_session_connection(const FIX::SessionID& fix_session) const
      -> void override {
    middleware::emit_generator_initiator_event(
        protocol::SessionConnectedEvent{decode_session(fix_session)});
  }

  auto process_session_disconnection(const FIX::SessionID& fix_session) const
      -> void override {
    middleware::emit_generator_initiator_event(
        protocol::SessionTerminatedEvent{decode_session(fix_session)});
  }
};

}  // namespace simulator::fix::generator_initiator

#endif  // SIMULATOR_GENERATOR_INITIATOR_IH_PROCESSORS_SESSION_EVENT_PROCESSOR_HPP_
