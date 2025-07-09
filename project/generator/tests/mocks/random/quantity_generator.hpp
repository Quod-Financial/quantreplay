#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_QUANTITY_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_QUANTITY_GENERATOR_HPP_

#include <gmock/gmock.h>

#include "ih/random/generators/quantity_generator.hpp"

namespace simulator::generator::mock {

class QuantityGenerator : public generator::random::QuantityGenerator {
 public:
  static auto make_output(double qty) -> GenerationOutput {
    return GenerationOutput{qty, 0};
  }

  MOCK_METHOD(GenerationOutput,
              generate_random_qty,
              (const random::QuantityGenerationParams&),
              (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_QUANTITY_GENERATOR_HPP_
