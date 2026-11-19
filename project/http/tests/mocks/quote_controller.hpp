#ifndef SIMULATOR_HTTP_TESTS_MOCKS_QUOTE_CONTROLLER_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_QUOTE_CONTROLLER_HPP_

#include <gmock/gmock.h>

#include <string>

#include "ih/controllers/quote_controller.hpp"

namespace simulator::http::test::mock {

class QuoteController : public http::QuoteController {
 public:
  MOCK_METHOD(Result,
              publish_quote,
              (const std::string& body),
              (const, override));
};

}  // namespace simulator::http::test::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_QUOTE_CONTROLLER_HPP_
