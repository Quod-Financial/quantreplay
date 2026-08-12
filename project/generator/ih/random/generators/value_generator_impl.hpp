#ifndef SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_VALUE_GENERATOR_IMPL_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_VALUE_GENERATOR_IMPL_HPP_

#include <memory>
#include <random>

#include "ih/random/generators/value_generator.hpp"

namespace simulator::generator::random {

class ValueGeneratorImpl final : public random::ValueGenerator {
 public:
  ValueGeneratorImpl();

  [[nodiscard]] static auto create() -> std::shared_ptr<ValueGeneratorImpl>;

  auto reseed(std::uint64_t seed) -> void override;

 private:
  auto generate_uniform(RandomInt min, RandomInt max) -> RandomInt override;

  auto generate_uniform(RandomUnsignedInt min, RandomUnsignedInt max)
      -> RandomUnsignedInt override;

  auto generate_uniform(RandomFloat min, RandomFloat max)
      -> RandomFloat override;

  std::mt19937_64 engine_;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_VALUE_GENERATOR_IMPL_HPP_
