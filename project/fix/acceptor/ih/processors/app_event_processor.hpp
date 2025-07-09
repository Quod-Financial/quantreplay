#ifndef SIMULATOR_ACCEPTOR_IH_PROCESSORS_APP_EVENT_PROCESSOR_HPP_
#define SIMULATOR_ACCEPTOR_IH_PROCESSORS_APP_EVENT_PROCESSOR_HPP_

#include <quickfix/SessionID.h>

#include "common/session_conversion.hpp"
#include "ih/processors/event_processor.hpp"
#include "middleware/routing/trading_session_event_channel.hpp"

namespace simulator::fix::acceptor {

class AppEventProcessor : public EventProcessor {
 public:
  auto process_session_disconnection(const FIX::SessionID& fix_session) const
      -> void override {
    middleware::emit_trading_session_event(
        protocol::SessionTerminatedEvent{decode_session(fix_session)});
  }
};

}  // namespace simulator::fix::acceptor

#endif  // SIMULATOR_ACCEPTOR_IH_PROCESSORS_APP_EVENT_PROCESSOR_HPP_