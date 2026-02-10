#ifndef SIMULATOR_HTTP_TESTS_MOCKS_APP_CONTROLLER_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_APP_CONTROLLER_HPP_

#include <gmock/gmock.h>

#include "http/ih/controllers/app_controller.hpp"

namespace simulator::http::mock {

class AppController : public http::AppController {
 public:
  MOCK_METHOD(Result, ready_to_reset, (), (const, override));

  MOCK_METHOD(void, reset_app_state, (), (const, override));
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_APP_CONTROLLER_HPP_
