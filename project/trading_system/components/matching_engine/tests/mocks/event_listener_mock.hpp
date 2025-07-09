#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_EVENT_LISTENER_MOCK_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_EVENT_LISTENER_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event.hpp"

namespace simulator::trading_system::matching_engine {

class EventListenerMock : public EventListener {
 public:
  MOCK_METHOD(void, on, (Event), (override));
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_EVENT_LISTENER_MOCK_HPP_
