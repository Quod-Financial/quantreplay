#ifndef SIMULATOR_MIDDLEWARE_ROUTING_TRADING_SESSION_EVENT_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_ROUTING_TRADING_SESSION_EVENT_CHANNEL_HPP_

#include "protocol/app/session_connected_event.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::middleware {

auto emit_trading_session_event(const protocol::SessionConnectedEvent& event)
    -> void;

auto emit_trading_session_event(const protocol::SessionTerminatedEvent& event)
    -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_ROUTING_TRADING_SESSION_EVENT_CHANNEL_HPP_