#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_DATA_ACCESS_ADAPTER_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_DATA_ACCESS_ADAPTER_HPP_

#include <gmock/gmock.h>

#include <cassert>
#include <deque>

#include "ih/historical/adapters/data_access_adapter.hpp"
#include "ih/historical/data/record.hpp"

namespace simulator::generator::mock {

class DataAccessAdapter : public historical::DataAccessAdapter {
 public:
  MOCK_METHOD(bool, has_next_record, (), (const, noexcept, override));

  MOCK_METHOD(void,
              parse_next_record,
              (historical::Record::Builder&),
              (override));
};

}  // namespace simulator::generator::mock

namespace simulator::generator::fake {

class DataAccessAdapter : public historical::DataAccessAdapter {
 public:
  [[nodiscard]]
  auto has_next_record() const noexcept -> bool override {
    return !assigned_records_.empty();
  }

  auto parse_next_record(historical::Record::Builder& builder)
      -> void override {
    assert(has_next_record());
    builder = std::move(assigned_records_.front());
    assigned_records_.pop_front();
  }

  auto push_record_builder(historical::Record::Builder builder) -> void {
    assigned_records_.emplace_back(std::move(builder));
  }

 private:
  std::deque<historical::Record::Builder> assigned_records_;
};

}  // namespace simulator::generator::fake

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_DATA_ACCESS_ADAPTER_HPP_
