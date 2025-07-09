#ifndef SIMULATOR_MIDDLEWARE_TESTS_TEST_UTILS_PROTOCOL_UTILS_HPP_
#define SIMULATOR_MIDDLEWARE_TESTS_TEST_UTILS_PROTOCOL_UTILS_HPP_

#include "protocol/types/session.hpp"

namespace simulator::middleware::test {

template <typename Message>
auto make_app_message() -> Message {
  return Message{protocol::Session{protocol::generator::Session{}}};
}

template <typename Event>
auto make_session_event() -> Event {
  return Event{protocol::Session{protocol::generator::Session{}}};
}

}  // namespace simulator::middleware::test

#endif  // SIMULATOR_MIDDLEWARE_TESTS_TEST_UTILS_PROTOCOL_UTILS_HPP_