#ifndef SIMULATOR_GENERATOR_INITIATOR_TESTS_TEST_UTILS_UTILS_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_TESTS_TEST_UTILS_UTILS_HPP_

#include <limits>
#include <type_traits>

namespace simulator::fix::generator_initiator::test {

template <typename E>
static constexpr auto invalid_enum_value() -> E {
  constexpr auto invalid_value =
      std::numeric_limits<std::underlying_type_t<E>>::max();
  return static_cast<E>(invalid_value);
}

}  // namespace simulator::fix::generator_initiator::test

#endif  // SIMULATOR_GENERATOR_INITIATOR_TESTS_TEST_UTILS_UTILS_HPP_
