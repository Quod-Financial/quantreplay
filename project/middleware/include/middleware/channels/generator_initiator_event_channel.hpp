#ifndef SIMULATOR_MIDDLEWARE_CHANNELS_GENERATOR_INITIATOR_EVENT_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_CHANNELS_GENERATOR_INITIATOR_EVENT_CHANNEL_HPP_

#include <memory>

#include "middleware/channels/detail/receiver.hpp"
#include "protocol/app/session_connected_event.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::middleware {

struct GeneratorInitiatorConnectionEventListener : public detail::Receiver {
  virtual auto on_event(const protocol::SessionConnectedEvent& event)
      -> void = 0;
};

struct GeneratorInitiatorTerminationEventListener : public detail::Receiver {
  virtual auto on_event(const protocol::SessionTerminatedEvent& event)
      -> void = 0;
};

auto bind_generator_initiator_connection_event_channel(
    std::shared_ptr<GeneratorInitiatorConnectionEventListener> listener)
    -> void;

auto release_generator_initiator_connection_event_channel() noexcept -> void;

auto bind_generator_initiator_termination_event_channel(
    std::shared_ptr<GeneratorInitiatorTerminationEventListener> listener)
    -> void;

auto release_generator_initiator_termination_event_channel() noexcept -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_CHANNELS_GENERATOR_INITIATOR_EVENT_CHANNEL_HPP_
