#ifndef SIMULATOR_FIX_COMMON_TESTS_MOCKS_MESSAGE_PROCESSOR_MOCK_HPP_
#define SIMULATOR_FIX_COMMON_TESTS_MOCKS_MESSAGE_PROCESSOR_MOCK_HPP_

#include <gmock/gmock.h>

#include "common/processors/message_processor.hpp"

namespace simulator::fix::test {

struct MessageProcessorMock : public MessageProcessor {
  MOCK_METHOD(void,
              process_message,
              (const FIX::Message&, const FIX::SessionID&),
              (const, override));
};

}  // namespace simulator::fix::test

#endif  // SIMULATOR_FIX_COMMON_TESTS_MOCKS_MESSAGE_PROCESSOR_MOCK_HPP_
