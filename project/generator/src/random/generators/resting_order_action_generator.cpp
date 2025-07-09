#include "ih/random/generators/resting_order_action_generator.hpp"

#include <cassert>
#include <utility>

#include "ih/random/generators/value_generator.hpp"
#include "ih/random/values/resting_order_action.hpp"

namespace simulator::generator::random {

RestingOrderActionGeneratorImpl::RestingOrderActionGeneratorImpl(
    std::shared_ptr<random::ValueGenerator> random_int_generator) noexcept
    : random_int_generator_{std::move(random_int_generator)} {
  assert(random_int_generator_);
}

auto RestingOrderActionGeneratorImpl::create(
    std::shared_ptr<random::ValueGenerator> random_int_generator)
    -> std::unique_ptr<RestingOrderActionGeneratorImpl> {
  return std::make_unique<RestingOrderActionGeneratorImpl>(
      std::move(random_int_generator));
}

auto RestingOrderActionGeneratorImpl::generate_integer()
    -> RestingOrderAction::RandomInteger {
  constexpr auto min = RestingOrderAction::min_random_integer();
  constexpr auto max = RestingOrderAction::max_random_integer();
  static_assert(min <= max);

  return random_int_generator_->generate_uniform_value(min, max);
}

}  // namespace simulator::generator::random
