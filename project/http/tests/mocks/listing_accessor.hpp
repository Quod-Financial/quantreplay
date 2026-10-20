#ifndef SIMULATOR_HTTP_TESTS_MOCKS_LISTING_ACCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_LISTING_ACCESSOR_HPP_

#include <gmock/gmock.h>

#include <cstdint>
#include <string>
#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/models/listing.hpp"
#include "ih/data_bridge/listing_accessor.hpp"
#include "ih/data_bridge/operation_failure.hpp"

namespace simulator::http::mock {

class ListingAccessor : public data_bridge::ListingAccessor {
  template <typename T>
  using Result = tl::expected<T, data_bridge::Failure>;
  using Listing = data_layer::Listing;
  using Patch = Listing::Patch;

 public:
  using ListingResult = Result<Listing>;
  using ListingsResult = Result<std::vector<Listing>>;
  using EmptyResult = Result<void>;

  MOCK_METHOD(ListingResult,
              select_single,
              (std::uint64_t),
              (const, noexcept, override));

  MOCK_METHOD(ListingResult,
              select_single,
              (const std::string&),
              (const, noexcept, override));

  MOCK_METHOD(ListingsResult, select_all, (), (const, noexcept, override));

  MOCK_METHOD(EmptyResult, add, (Patch), (const, noexcept, override));

  MOCK_METHOD(EmptyResult,
              update,
              (Patch, std::uint64_t),
              (const, noexcept, override));

  MOCK_METHOD(EmptyResult,
              update,
              (Patch, const std::string&),
              (const, noexcept, override));
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_LISTING_ACCESSOR_HPP_
