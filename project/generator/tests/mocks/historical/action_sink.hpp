#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_ACTION_SINK_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_ACTION_SINK_HPP_

#include <gmock/gmock.h>

#include "ih/historical/data/record.hpp"
#include "ih/historical/processing/action_sink.hpp"

namespace simulator::generator::mock {

class ActionSink : public historical::ActionSink {
 public:
  MOCK_METHOD(void, process, (historical::Action), (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_ACTION_SINK_HPP_
