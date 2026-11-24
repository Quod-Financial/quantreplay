#ifndef SIMULATOR_HTTP_TESTS_MOCKS_QUOTE_CONTROLLER_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_QUOTE_CONTROLLER_HPP_

#include <gmock/gmock.h>

#include <optional>
#include <string>

#include "core/domain/attributes.hpp"
#include "ih/controllers/quote_controller.hpp"

namespace simulator::http::test::mock {

class QuoteController : public http::QuoteController {
 public:
  MOCK_METHOD(std::optional<std::string>,
              publication_venue_id,
              (const std::string& body),
              (const, override));

  MOCK_METHOD(Result,
              publish_quote,
              (const std::string& body),
              (const, override));

  MOCK_METHOD(Result,
              cancel_quote,
              (const QuoteId& quote_id),
              (const, override));
};

}  // namespace simulator::http::test::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_QUOTE_CONTROLLER_HPP_
