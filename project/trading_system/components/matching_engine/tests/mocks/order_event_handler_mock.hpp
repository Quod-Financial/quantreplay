#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_ORDER_EVENT_HANDLER_MOCK_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_ORDER_EVENT_HANDLER_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/common/abstractions/order_event_handler.hpp"

namespace simulator::trading_system::matching_engine {

class OrderEventHandlerMock : public OrderEventHandler {
 public:
  MOCK_METHOD(void, handle, (const event::Tick& tick), (override));
  MOCK_METHOD(void,
              handle,
              (const event::PhaseTransition& phase_transition),
              (override));
  MOCK_METHOD(void,
              handle_disconnection,
              (const protocol::Session& session),
              (override));
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_ORDER_EVENT_HANDLER_MOCK_HPP_
