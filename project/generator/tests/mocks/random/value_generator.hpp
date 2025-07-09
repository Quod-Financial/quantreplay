#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_VALUE_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_VALUE_GENERATOR_HPP_

#include <gmock/gmock.h>

#include "ih/random/generators/value_generator.hpp"

namespace simulator::generator::mock {

class ValueGenerator : public generator::random::ValueGenerator {
 public:
  MOCK_METHOD(RandomInt, generate_uniform, (RandomInt, RandomInt), (override));

  MOCK_METHOD(RandomUnsignedInt,
              generate_uniform,
              (RandomUnsignedInt, RandomUnsignedInt),
              (override));

  MOCK_METHOD(RandomFloat,
              generate_uniform,
              (RandomFloat, RandomFloat),
              (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_VALUE_GENERATOR_HPP_
