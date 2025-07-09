#ifndef SIMULATOR_HTTP_TESTS_MOCKS_VENUE_ACCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_VENUE_ACCESSOR_HPP_

#include <gmock/gmock.h>

#include <tl/expected.hpp>

#include "data_layer/api/models/venue.hpp"
#include "ih/data_bridge/operation_failure.hpp"
#include "ih/data_bridge/venue_accessor.hpp"

namespace simulator::http::mock {

class VenueAccessor : public data_bridge::VenueAccessor {
  template <typename T>
  using Result = tl::expected<T, data_bridge::Failure>;
  using Venue = data_layer::Venue;
  using Patch = Venue::Patch;

 public:
  using VenueResult = Result<Venue>;
  using VenuesResult = Result<std::vector<Venue>>;
  using EmptyResult = Result<void>;

  MOCK_METHOD(VenueResult,
              select_single,
              (const std::string&),
              (const, noexcept, override));

  MOCK_METHOD(VenuesResult, select_all, (), (const, noexcept, override));

  MOCK_METHOD(EmptyResult, add, (Patch), (const, noexcept, override));

  MOCK_METHOD(EmptyResult,
              update,
              (Patch, const std::string&),
              (const, noexcept, override));
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_VENUE_ACCESSOR_HPP_
