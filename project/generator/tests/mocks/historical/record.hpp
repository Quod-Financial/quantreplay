#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_RECORD_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_RECORD_HPP_

#include <gmock/gmock.h>

#include "ih/historical/data/record.hpp"

namespace simulator::generator::mock {

class RecordBuilder : public historical::Record::Builder {
 public:
  MOCK_METHOD(historical::Record::Builder&,
              with_instrument,
              (std::string instrument),
              (noexcept, override));

  MOCK_METHOD(historical::Record::Builder&,
              with_received_time,
              (historical::Timepoint received_time),
              (noexcept, override));

  MOCK_METHOD(historical::Record::Builder&,
              with_message_time,
              (historical::Timepoint message_time),
              (noexcept, override));

  MOCK_METHOD(historical::Record::Builder&,
              with_source_name,
              (std::string source_name),
              (noexcept, override));

  MOCK_METHOD(historical::Record::Builder&,
              with_source_connection,
              (std::string source_conn),
              (noexcept, override));

  MOCK_METHOD(historical::Record::Builder&,
              with_source_row,
              (std::uint64_t source_row),
              (noexcept, override));

  MOCK_METHOD(historical::Record::Builder&,
              add_level,
              (std::uint64_t index, historical::Level level),
              (override));

  MOCK_METHOD(historical::Record, construct, (), (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_RECORD_HPP_
