#ifndef SIMULATOR_HTTP_TESTS_MOCKS_PRICE_SEED_ACCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_PRICE_SEED_ACCESSOR_HPP_

#include <gmock/gmock.h>

#include <cstdint>
#include <string>
#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/models/price_seed.hpp"
#include "ih/data_bridge/operation_failure.hpp"
#include "ih/data_bridge/price_seed_accessor.hpp"

namespace simulator::http::mock {

class PriceSeedAccessor : public data_bridge::PriceSeedAccessor {
  template <typename T>
  using Result = tl::expected<T, data_bridge::Failure>;
  using PriceSeed = data_layer::PriceSeed;
  using Patch = PriceSeed::Patch;

 public:
  using PriceSeedResult = Result<PriceSeed>;
  using PriceSeedsResult = Result<std::vector<PriceSeed>>;
  using EmptyResult = Result<void>;

  MOCK_METHOD(PriceSeedResult,
              select_single,
              (std::uint64_t),
              (const, noexcept, override));

  MOCK_METHOD(PriceSeedsResult, select_all, (), (const, noexcept, override));

  MOCK_METHOD(PriceSeedResult, add, (Patch), (const, noexcept, override));

  MOCK_METHOD(PriceSeedResult,
              update,
              (Patch, std::uint64_t),
              (const, noexcept, override));

  MOCK_METHOD(EmptyResult,
              drop_single,
              (std::uint64_t),
              (const, noexcept, override));

  MOCK_METHOD(EmptyResult, sync, (std::string), (const, noexcept, override));
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_PRICE_SEED_ACCESSOR_HPP_
