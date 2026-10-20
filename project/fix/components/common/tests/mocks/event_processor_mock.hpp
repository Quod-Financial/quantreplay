#ifndef SIMULATOR_FIX_COMMON_TESTS_MOCKS_EVENT_PROCESSOR_MOCK_HPP_
#define SIMULATOR_FIX_COMMON_TESTS_MOCKS_EVENT_PROCESSOR_MOCK_HPP_

#include <gmock/gmock.h>

#include "common/processors/event_processor.hpp"

namespace simulator::fix::test {

struct EventProcessorMock : public EventProcessor {
  MOCK_METHOD(void,
              process_session_connection,
              (const FIX::SessionID&),
              (const, override));

  MOCK_METHOD(void,
              process_session_disconnection,
              (const FIX::SessionID&),
              (const, override));
};

}  // namespace simulator::fix::test

#endif  // SIMULATOR_FIX_COMMON_TESTS_MOCKS_EVENT_PROCESSOR_MOCK_HPP_