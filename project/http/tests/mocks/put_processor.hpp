#ifndef SIMULATOR_HTTP_TESTS_MOCKS_PUT_PROCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_PUT_PROCESSOR_HPP_

#include <gmock/gmock.h>

#include "ih/processors/put_processor.hpp"

namespace simulator::http::test::mock {

class PutProcessor : public http::PutProcessor {
 public:
  MOCK_METHOD(void,
              update_venue,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              update_listing,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              update_data_source,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              update_price_seed,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              update_settings,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));
};

}  // namespace simulator::http::test::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_PUT_PROCESSOR_HPP_