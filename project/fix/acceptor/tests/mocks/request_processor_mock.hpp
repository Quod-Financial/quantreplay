#ifndef SIMULATOR_ACCEPTOR_TESTS_MOCKS_REQUEST_PROCESSOR_MOCK_HPP_
#define SIMULATOR_ACCEPTOR_TESTS_MOCKS_REQUEST_PROCESSOR_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/processors/request_processor.hpp"

namespace simulator::fix::acceptor::test {

struct RequestProcessorMock : public RequestProcessor {
  MOCK_METHOD(void,
              process_fix_request,
              (const FIX::Message&, const FIX::SessionID&),
              (const, override));
};

}  // namespace simulator::fix::acceptor::test

#endif  // SIMULATOR_ACCEPTOR_TESTS_MOCKS_REQUEST_PROCESSOR_MOCK_HPP_