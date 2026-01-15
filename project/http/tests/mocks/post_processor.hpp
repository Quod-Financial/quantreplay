#ifndef SIMULATOR_HTTP_TESTS_MOCKS_POST_PROCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_POST_PROCESSOR_HPP_

#include <gmock/gmock.h>

#include "ih/processors/post_processor.hpp"

namespace simulator::http::test::mock {

class PostProcessor : public http::PostProcessor {
 public:
  MOCK_METHOD(void,
              add_venue,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              add_listing,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              add_data_source,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              add_price_seed,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              sync_price_seeds,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              stop_order_gen,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              start_order_gen,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              halt_phase,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              resume_phase,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              handle_store_request,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              handle_recover_request,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              reset_app,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));
};

}  // namespace simulator::http::test::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_POST_PROCESSOR_HPP_