#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_COUNTERPARTY_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_COUNTERPARTY_GENERATOR_HPP_

#include <gmock/gmock.h>

#include <memory>

#include "ih/random/generators/counterparty_generator.hpp"

namespace simulator::generator::mock {

class CounterpartyGenerator : public generator::random::CounterpartyGenerator {
 public:
  MOCK_METHOD(unsigned int, generate_counterparty_number, (), (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_COUNTERPARTY_GENERATOR_HPP_
