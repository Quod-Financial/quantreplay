#ifndef SIMULATOR_HTTP_TESTS_MOCKS_REDIRECTOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_REDIRECTOR_HPP_

#include <gmock/gmock.h>

#include <optional>
#include <utility>

#include "ih/redirect/redirector.hpp"
#include "ih/redirect/result.hpp"

namespace simulator::http::mock {

class Redirector : public redirect::Redirector {
 public:
  MOCK_METHOD(RedirectionResult,
              redirect,
              (const redirect::Request&),
              (const, noexcept, override));

  static auto make_output(redirect::Result result) -> RedirectionResult {
    return std::make_pair(std::make_optional(std::move(result)),
                          Redirector::Status::Success);
  }

  static auto make_output(Redirector::Status status) -> RedirectionResult {
    return std::make_pair(std::nullopt, status);
  }
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_REDIRECTOR_HPP_
