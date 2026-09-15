#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_CONTEXT_COMPONENT_CONTEXT_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_CONTEXT_COMPONENT_CONTEXT_HPP_

#include <gmock/gmock.h>

#include <cstddef>
#include <string>

#include "data_layer/api/models/venue.hpp"
#include "ih/context/component_context.hpp"

namespace simulator::generator::mock {

class ComponentContext : public generator::ComponentContext {
 public:
  MOCK_METHOD(bool, is_component_running, (), (const, noexcept, override));

  MOCK_METHOD(const simulator::data_layer::Venue&,
              get_venue,
              (),
              (const, noexcept, override));

  MOCK_METHOD(std::string, generate_identifier, (), (noexcept, override));

  MOCK_METHOD(std::size_t,
              next_generated_order_message_number,
              (),
              (noexcept, override));

  MOCK_METHOD(void, call_on_launch, (const OnStartupCallback&), (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_CONTEXT_COMPONENT_CONTEXT_HPP_
