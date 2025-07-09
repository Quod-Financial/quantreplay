#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_PROVIDER_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_PROVIDER_HPP_

#include <gmock/gmock.h>

#include "ih/historical/data/provider.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/data/time.hpp"

namespace simulator::generator::mock {

class DataProvider : public historical::DataProvider {
 public:
  MOCK_METHOD(bool, is_empty, (), (const, noexcept, override));

  MOCK_METHOD(void, initialize_time_offset, (), (noexcept, override));

  MOCK_METHOD(void, add, (historical::Record), (override));

  MOCK_METHOD(void, pull_into, (historical::Action::Builder&), (override));

  auto assign_time_offset(historical::Duration offset) -> void {
    set_time_offset(offset);
  }
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_PROVIDER_HPP_
