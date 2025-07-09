#include "ih/random/generators/value_generator_impl.hpp"

#include <cassert>
#include <memory>
#include <random>

#include "ih/random/generators/value_generator.hpp"

namespace simulator::generator::random {

ValueGeneratorImpl::ValueGeneratorImpl() : engine_{std::random_device{}()} {}

auto ValueGeneratorImpl::create() -> std::shared_ptr<ValueGeneratorImpl> {
  return std::make_shared<ValueGeneratorImpl>();
}

auto ValueGeneratorImpl::generate_uniform(RandomInt min, RandomInt max)
    -> ValueGeneratorImpl::RandomInt {
  using Distribution = std::uniform_int_distribution<RandomInt>;

  assert(min <= max);
  return Distribution{min, max}(engine_);
}

auto ValueGeneratorImpl::generate_uniform(RandomUnsignedInt min,
                                          RandomUnsignedInt max)
    -> ValueGeneratorImpl::RandomUnsignedInt {
  using Distribution = std::uniform_int_distribution<RandomUnsignedInt>;

  assert(min <= max);
  return Distribution{min, max}(engine_);
}

auto ValueGeneratorImpl::generate_uniform(RandomFloat min, RandomFloat max)
    -> ValueGeneratorImpl::RandomFloat {
  using Distribution = std::uniform_real_distribution<RandomFloat>;

  assert(min <= max);
  return Distribution{min, max}(engine_);
}

}  // namespace simulator::generator::random
