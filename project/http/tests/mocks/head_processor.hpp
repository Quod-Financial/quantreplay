#ifndef SIMULATOR_HTTP_TESTS_MOCKS_HEAD_PROCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_HEAD_PROCESSOR_HPP_

#include <gmock/gmock.h>

#include "ih/processors/head_processor.hpp"

namespace simulator::http::test::mock {

class HeadProcessor : public http::HeadProcessor {
 public:
  MOCK_METHOD(void,
              get_data_dictionaries,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));
};

}  // namespace simulator::http::test::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_HEAD_PROCESSOR_HPP_
