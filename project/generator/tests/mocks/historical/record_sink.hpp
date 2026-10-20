#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_RECORD_SINK_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_RECORD_SINK_HPP_

#include <gmock/gmock.h>

#include "ih/historical/data/record.hpp"
#include "ih/historical/processing/record_sink.hpp"

namespace simulator::generator::mock {

class RecordSink : public historical::RecordSink {
 public:
  MOCK_METHOD(void, process, (historical::Record), (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_RECORD_SINK_HPP_
