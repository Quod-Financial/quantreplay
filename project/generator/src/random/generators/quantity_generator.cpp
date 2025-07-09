#include "ih/random/generators/quantity_generator.hpp"

#include <memory>
#include <utility>

#include "core/tools/numeric.hpp"
#include "ih/constants.hpp"
#include "ih/random/generators/value_generator.hpp"

namespace simulator::generator::random {

QuantityGeneratorImpl::QuantityGeneratorImpl(
    std::shared_ptr<random::ValueGenerator> random_int_generator) noexcept
    : random_int_generator_{std::move(random_int_generator)} {}

auto QuantityGeneratorImpl::create(
    std::shared_ptr<random::ValueGenerator> random_int_generator)
    -> std::unique_ptr<QuantityGeneratorImpl> {
  return std::make_unique<QuantityGeneratorImpl>(
      std::move(random_int_generator));
}

auto QuantityGeneratorImpl::generate_random_qty(
    const QuantityGenerationParams& params)
    -> QuantityGenerator::GenerationOutput {
  constexpr int random_min = 0;

  double multiplier = params.get_multiplier();
  if (core::equal(multiplier, 0.)) {
    multiplier = constant::DefaultListingQtyMultiple;
  }
  const auto min_qty = params.get_minimal_quantity() / multiplier;
  const auto max_qty = params.get_maximal_quantity() / multiplier;

  assert(min_qty >= 0);
  assert(max_qty >= 0);
  assert(min_qty <= max_qty);
  const auto random_max = static_cast<int>(max_qty - min_qty);
  const int random_number =
      random_int_generator_->generate_uniform_value(random_min, random_max);

  GenerationOutput output{};
  output.random_number = random_number;

  const double random_qty = (random_number + min_qty) * multiplier;
  if (!core::equal(random_qty, 0.)) {
    output.generated_quantity = random_qty;
  } else {
    // random_number and min_qty are equal to zero in case random_qty is zero.
    // We use multiplier in this case as it cannot not be zero by definition.
    output.generated_quantity = multiplier;
  }

  return output;
}

}  // namespace simulator::generator::random
