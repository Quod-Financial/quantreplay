#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_ALGORITHM_GENERATION_ALGORITHM_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_ALGORITHM_GENERATION_ALGORITHM_HPP_

#include <gmock/gmock.h>

#include <cstdint>

#include "ih/adaptation/generated_message.hpp"
#include "ih/random/algorithm/generation_algorithm.hpp"

namespace simulator::generator::mock {

class GenerationAlgorithm : public generator::random::GenerationAlgorithm {
 public:
  MOCK_METHOD(bool,
              generate,
              (generator::GeneratedMessage & target_message),
              (override));

  MOCK_METHOD(void, reseed, (std::uint64_t seed), (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_ALGORITHM_GENERATION_ALGORITHM_HPP_
