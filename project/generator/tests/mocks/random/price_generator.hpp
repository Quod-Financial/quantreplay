#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_PRICE_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_PRICE_GENERATOR_HPP_

#include <gmock/gmock.h>

#include <utility>

#include "data_layer/api/models/price_seed.hpp"
#include "ih/random/generators/price_generator.hpp"
#include "ih/random/values/event.hpp"

namespace simulator::generator::mock {

class PriceGenerator : public generator::random::PriceGenerator {
 public:
  using GenerationOutput = std::pair<double, GenerationDetails>;

  static auto make_output(double price) -> GenerationOutput {
    return std::make_pair(price, GenerationDetails{});
  }

  MOCK_METHOD(GenerationOutput,
              generate_px,
              (const random::PriceGenerationParams&,
               const MarketState&,
               const simulator::data_layer::PriceSeed&,
               random::Event),
              (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_PRICE_GENERATOR_HPP_
