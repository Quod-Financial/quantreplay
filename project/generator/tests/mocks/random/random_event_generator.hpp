#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_RANDOM_EVENT_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_RANDOM_EVENT_GENERATOR_HPP_

#include <gmock/gmock.h>

#include "ih/random/generators/event_generator.hpp"
#include "ih/random/values/event.hpp"

namespace simulator::generator::mock {

class EventGenerator : public generator::random::EventGenerator {
 public:
  MOCK_METHOD(random::Event::RandomInteger,
              generate_choice_integer,
              (),
              (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_RANDOM_RANDOM_EVENT_GENERATOR_HPP_
