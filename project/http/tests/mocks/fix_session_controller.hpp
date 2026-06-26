#ifndef SIMULATOR_HTTP_TESTS_MOCKS_FIX_SESSION_CONTROLLER_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_FIX_SESSION_CONTROLLER_HPP_

#include <gmock/gmock.h>

#include <string>
#include <unordered_map>

#include "ih/controllers/fix_session_controller.hpp"
#include "protocol/app/session_connected_event.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::http::mock {

class FixSessionController : public http::FixSessionController {
 public:
  using Sessions = std::unordered_map<std::string, FixSessionInfo>;

  MOCK_METHOD(void,
              handle,
              (const protocol::SessionConnectedEvent&),
              (override));

  MOCK_METHOD(void,
              handle,
              (const protocol::SessionTerminatedEvent&),
              (override));

  MOCK_METHOD(Sessions, sessions, (), (const, override));
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_FIX_SESSION_CONTROLLER_HPP_
