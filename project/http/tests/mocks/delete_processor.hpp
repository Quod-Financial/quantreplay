#ifndef SIMULATOR_HTTP_TESTS_MOCKS_DELETE_PROCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_DELETE_PROCESSOR_HPP_

#include <gmock/gmock.h>

#include "ih/processors/delete_processor.hpp"

namespace simulator::http::test::mock {

class DeleteProcessor : public http::DeleteProcessor {
 public:
  MOCK_METHOD(void,
              delete_price_seed,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));
};

}  // namespace simulator::http::test::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_DELETE_PROCESSOR_HPP_