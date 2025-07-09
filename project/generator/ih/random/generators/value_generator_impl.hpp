#ifndef SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_VALUE_GENERATOR_IMPL_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_VALUE_GENERATOR_IMPL_HPP_

#include <memory>
#include <random>

#include "ih/random/generators/value_generator.hpp"

namespace simulator::generator::random {

class ValueGeneratorImpl final : public random::ValueGenerator {
 public:
  ValueGeneratorImpl();

  static auto create() -> std::shared_ptr<ValueGeneratorImpl>;

 private:
  auto generate_uniform(RandomInt min, RandomInt max) -> RandomInt override;

  auto generate_uniform(RandomUnsignedInt min, RandomUnsignedInt max)
      -> RandomUnsignedInt override;

  auto generate_uniform(RandomFloat min, RandomFloat max)
      -> RandomFloat override;

  std::mt19937 engine_;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_VALUE_GENERATOR_IMPL_HPP_
