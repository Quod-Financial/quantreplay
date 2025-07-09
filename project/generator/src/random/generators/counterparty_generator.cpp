#include "ih/random/generators/counterparty_generator.hpp"

#include <utility>

#include "ih/random/generators/value_generator.hpp"

namespace simulator::generator::random {

CounterpartyGeneratorImpl::CounterpartyGeneratorImpl(
    std::uint32_t parties_count,
    std::shared_ptr<random::ValueGenerator> random_int_generator) noexcept
    : random_int_generator_{std::move(random_int_generator)},
      max_party_number_{parties_count} {
  assert(random_int_generator_);
}

auto CounterpartyGeneratorImpl::create(
    std::uint32_t parties_count,
    std::shared_ptr<random::ValueGenerator> random_int_generator)
    -> std::unique_ptr<CounterpartyGeneratorImpl> {
  return std::make_unique<CounterpartyGeneratorImpl>(
      parties_count, std::move(random_int_generator));
}

auto CounterpartyGeneratorImpl::generate_counterparty_number()
    -> std::uint32_t {
  constexpr std::uint32_t min_counterparty_number = 1;
  return random_int_generator_->generate_uniform_value(min_counterparty_number,
                                                       max_party_number_);
}

}  // namespace simulator::generator::random
