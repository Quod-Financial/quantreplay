#ifndef SIMULATOR_HTTP_TESTS_MOCKS_RESOLVER_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_RESOLVER_HPP_

#include <gmock/gmock.h>

#include <tl/expected.hpp>
#include <utility>

#include "ih/redirect/destination.hpp"
#include "ih/redirect/resolver.hpp"

namespace simulator::http::mock {

class Resolver : public redirect::Resolver {
 public:
  MOCK_METHOD((tl::expected<redirect::Destination, Error>),
              resolve_by_venue_id,
              (const std::string&),
              (const, noexcept, override));
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_RESOLVER_HPP_
