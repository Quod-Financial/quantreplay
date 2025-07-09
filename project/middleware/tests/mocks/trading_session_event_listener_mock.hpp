#ifndef SIMULATOR_MIDDLEWARE_TESTS_MOCKS_TRADING_SESSION_EVENT_LISTENER_MOCK_HPP_
#define SIMULATOR_MIDDLEWARE_TESTS_MOCKS_TRADING_SESSION_EVENT_LISTENER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/trading_session_event_channel.hpp"

namespace simulator::middleware::test {

struct TradingSessionEventListenerMock : public TradingSessionEventListener {
  MOCK_METHOD(void,
              on_event,
              (const protocol::SessionTerminatedEvent&),
              (override));
};

}  // namespace simulator::middleware::test

#endif  // SIMULATOR_MIDDLEWARE_TESTS_MOCKS_TRADING_SESSION_EVENT_LISTENER_MOCK_HPP_