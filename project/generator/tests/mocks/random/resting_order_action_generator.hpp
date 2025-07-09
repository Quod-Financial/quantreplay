#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_RESTING_ORDER_ACTION_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_RESTING_ORDER_ACTION_GENERATOR_HPP_

#include <gmock/gmock.h>

#include "ih/random/generators/resting_order_action_generator.hpp"
#include "ih/random/values/resting_order_action.hpp"

namespace simulator::generator::mock {

class RestingOrderActionGenerator
    : public generator::random::RestingOrderActionGenerator {
 public:
  MOCK_METHOD(random::RestingOrderAction::RandomInteger,
              generate_integer,
              (),
              (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_RESTING_ORDER_ACTION_GENERATOR_HPP_
