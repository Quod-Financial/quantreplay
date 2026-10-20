#ifndef SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_GENERATOR_INITIATOR_EVENT_LISTENER_MOCK_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_GENERATOR_INITIATOR_EVENT_LISTENER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/generator_initiator_event_channel.hpp"

namespace simulator::fix::generator_initiator::test {

struct GeneratorInitiatorConnectionEventListenerMock
    : public middleware::GeneratorInitiatorConnectionEventListener {
  MOCK_METHOD(void,
              on_event,
              (const protocol::SessionConnectedEvent&),
              (override));
};

struct GeneratorInitiatorTerminationEventListenerMock
    : public middleware::GeneratorInitiatorTerminationEventListener {
  MOCK_METHOD(void,
              on_event,
              (const protocol::SessionTerminatedEvent&),
              (override));
};

}  // namespace simulator::fix::generator_initiator::test

#endif  // SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_GENERATOR_INITIATOR_EVENT_LISTENER_MOCK_HPP_
