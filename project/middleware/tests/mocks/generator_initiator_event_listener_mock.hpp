#ifndef SIMULATOR_MIDDLEWARE_TESTS_MOCKS_GENERATOR_INITIATOR_EVENT_LISTENER_MOCK_HPP_
#define SIMULATOR_MIDDLEWARE_TESTS_MOCKS_GENERATOR_INITIATOR_EVENT_LISTENER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/generator_initiator_event_channel.hpp"

namespace simulator::middleware::test {

struct GeneratorInitiatorConnectionEventListenerMock
    : public GeneratorInitiatorConnectionEventListener {
  MOCK_METHOD(void,
              on_event,
              (const protocol::SessionConnectedEvent&),
              (override));
};

struct GeneratorInitiatorTerminationEventListenerMock
    : public GeneratorInitiatorTerminationEventListener {
  MOCK_METHOD(void,
              on_event,
              (const protocol::SessionTerminatedEvent&),
              (override));
};

}  // namespace simulator::middleware::test

#endif  // SIMULATOR_MIDDLEWARE_TESTS_MOCKS_GENERATOR_INITIATOR_EVENT_LISTENER_MOCK_HPP_
