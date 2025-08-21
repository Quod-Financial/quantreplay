#ifndef SIMULATOR_CORE_TESTS_TEST_UTILS_UTILS_HPP_
#define SIMULATOR_CORE_TESTS_TEST_UTILS_UTILS_HPP_

#include <limits>
#include <type_traits>

namespace simulator::core::test {

template <typename E>
static constexpr auto invalid_enum_value() -> E {
  constexpr auto invalid_value =
      std::numeric_limits<std::underlying_type_t<E>>::max();
  return static_cast<E>(invalid_value);
}

}  // namespace simulator::core::test

#endif  // SIMULATOR_CORE_TESTS_TEST_UTILS_UTILS_HPP_
