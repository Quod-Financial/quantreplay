#ifndef SIMULATOR_HTTP_TESTS_MOCKS_RESOLVER_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_RESOLVER_HPP_

#include <gmock/gmock.h>

#include <optional>
#include <utility>

#include "ih/redirect/destination.hpp"
#include "ih/redirect/resolver.hpp"

namespace simulator::http::mock {

class Resolver : public redirect::Resolver {
 public:
  MOCK_METHOD(ResolvingResult,
              resolve_by_venue_id,
              (const std::string&),
              (const, noexcept, override));

  static auto make_output(redirect::Destination destination)
      -> redirect::Resolver::ResolvingResult {
    return std::make_pair(std::make_optional(std::move(destination)),
                          redirect::Resolver::Status::Success);
  }

  static auto make_output(redirect::Resolver::Status status)
      -> redirect::Resolver::ResolvingResult {
    return std::make_pair(std::nullopt, status);
  }
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_RESOLVER_HPP_
