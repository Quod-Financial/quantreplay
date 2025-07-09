#ifndef SIMULATOR_ACCEPTOR_TESTS_MOCKS_EVENT_PROCESSOR_MOCK_HPP_
#define SIMULATOR_ACCEPTOR_TESTS_MOCKS_EVENT_PROCESSOR_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/processors/event_processor.hpp"

namespace simulator::fix::acceptor::test {

struct EventProcessorMock : public EventProcessor {
  MOCK_METHOD(void,
              process_session_disconnection,
              (const FIX::SessionID&),
              (const, override));
};

}  // namespace simulator::fix::acceptor::test

#endif  // SIMULATOR_ACCEPTOR_TESTS_MOCKS_EVENT_PROCESSOR_MOCK_HPP_