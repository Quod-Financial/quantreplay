#ifndef SIMULATOR_HTTP_TESTS_MOCKS_GENERATOR_CONTROLLER_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_GENERATOR_CONTROLLER_HPP_

#include <gmock/gmock.h>

#include "ih/controllers/generator_controller.hpp"

namespace simulator::http::test::mock {

class GeneratorController : public http::GeneratorController {
 public:
  MOCK_METHOD(Result,
              start_generation,
              (const std::string& body),
              (const, override));

  MOCK_METHOD(Result, stop_generation, (), (const, override));
};

}  // namespace simulator::http::test::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_GENERATOR_CONTROLLER_HPP_
