#ifndef SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_GENERATION_ALGORITHM_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_GENERATION_ALGORITHM_HPP_

#include "ih/adaptation/generated_message.hpp"

namespace simulator::generator::random {

class GenerationAlgorithm {
 public:
  virtual ~GenerationAlgorithm() = default;

  // Fills the message with a generated data.
  // - `target_message` is considered as finished and valid when `generate`
  // returns `true`
  // - `target_message` content has to be ignored in case `false`
  // is returned
  virtual auto generate(GeneratedMessage& target_message) -> bool = 0;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_GENERATION_ALGORITHM_HPP_
