#include "ih/random/generators/event_generator.hpp"

#include <cassert>
#include <memory>

#include "ih/random/generators/value_generator.hpp"
#include "ih/random/values/event.hpp"

namespace simulator::generator::random {

EventGeneratorImpl::EventGeneratorImpl(
    std::shared_ptr<random::ValueGenerator> random_int_generator) noexcept
    : random_int_generator_{std::move(random_int_generator)} {
  assert(random_int_generator_);
}

auto EventGeneratorImpl::create(
    std::shared_ptr<random::ValueGenerator> random_int_generator)
    -> std::unique_ptr<EventGeneratorImpl> {
  return std::make_unique<EventGeneratorImpl>(std::move(random_int_generator));
}

auto EventGeneratorImpl::generate_choice_integer() -> Event::RandomInteger {
  constexpr auto min = Event::min_random_integer();
  constexpr auto max = Event::max_random_integer();
  static_assert(min <= max);

  return random_int_generator_->generate_uniform_value(min, max);
}

}  // namespace simulator::generator::random
