#ifndef SIMULATOR_HTTP_TESTS_MOCKS_REDIRECTION_PROCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_REDIRECTION_PROCESSOR_HPP_

#include <gmock/gmock.h>

#include <optional>
#include <string>

#include "ih/redirect/redirection_processor.hpp"

namespace simulator::http::test::mock {

class RedirectionProcessor : public redirect::RedirectionProcessor {
 public:
  MOCK_METHOD(redirect::Result,
              redirect_to_venue,
              (const std::string& venue_id,
               Pistache::Http::Method method,
               const std::string& url,
               std::optional<std::string> body),
              (const, override));
};

}  // namespace simulator::http::test::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_REDIRECTION_PROCESSOR_HPP_