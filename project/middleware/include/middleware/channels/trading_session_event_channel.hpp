#ifndef SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_SESSION_EVENT_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_SESSION_EVENT_CHANNEL_HPP_

#include <memory>

#include "middleware/channels/detail/receiver.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::middleware {

struct TradingSessionEventListener : public detail::Receiver {
  virtual auto on_event(const protocol::SessionTerminatedEvent& event)
      -> void = 0;
};

auto bind_trading_session_event_channel(
    std::shared_ptr<TradingSessionEventListener> listener) -> void;

auto release_trading_session_event_channel() noexcept -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_SESSION_EVENT_CHANNEL_HPP_