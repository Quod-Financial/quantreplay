#ifndef SIMULATOR_TRADING_SYSTEM_ACCEPTOR_IH_PROCESSORS_APP_EVENT_PROCESSOR_HPP_
#define SIMULATOR_TRADING_SYSTEM_ACCEPTOR_IH_PROCESSORS_APP_EVENT_PROCESSOR_HPP_

#include <quickfix/SessionID.h>

#include "common/processors/event_processor.hpp"
#include "common/session_conversion.hpp"
#include "middleware/routing/trading_session_event_channel.hpp"

namespace simulator::fix::trading_system_acceptor {

class AppEventProcessor : public EventProcessor {
 public:
  auto process_session_connection(const FIX::SessionID& fix_session) const
      -> void override {
    middleware::emit_trading_session_event(
        protocol::SessionConnectedEvent{decode_session(fix_session)});
  }

  auto process_session_disconnection(const FIX::SessionID& fix_session) const
      -> void override {
    middleware::emit_trading_session_event(
        protocol::SessionTerminatedEvent{decode_session(fix_session)});
  }
};

}  // namespace simulator::fix::trading_system_acceptor

#endif  // SIMULATOR_TRADING_SYSTEM_ACCEPTOR_IH_PROCESSORS_APP_EVENT_PROCESSOR_HPP_