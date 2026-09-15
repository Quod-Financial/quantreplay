#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_PROCESSOR_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_PROCESSOR_HPP_

#include <gmock/gmock.h>

#include "ih/historical/data/record.hpp"
#include "ih/historical/processor.hpp"

namespace simulator::generator::mock {

class Processor : public historical::Processor {
 public:
  MOCK_METHOD(void, process, (historical::Action), (override));
  MOCK_METHOD(void, process, (historical::Record), (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_HISTORICAL_PROCESSOR_HPP_
