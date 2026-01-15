#ifndef SIMULATOR_HTTP_TESTS_MOCKS_GET_PROCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_GET_PROCESSOR_HPP_

#include <gmock/gmock.h>

#include "ih/processors/get_processor.hpp"

namespace simulator::http::test::mock {

class GetProcessor : public http::GetProcessor {
 public:
  MOCK_METHOD(void,
              get_venue,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_venues,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_listing,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_listings,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_data_source,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_data_sources,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_price_seed,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_price_seeds,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_status,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_venue_status,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_venue_statuses,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_settings,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(void,
              get_order_gen_status,
              (const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response),
              (override));

  MOCK_METHOD(std::string,
              get_venue_status_str,
              (const data_layer::Venue& venue,
               bool send_response_code,
               bool& available),
              (const, override));
};

}  // namespace simulator::http::test::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_GET_PROCESSOR_HPP_