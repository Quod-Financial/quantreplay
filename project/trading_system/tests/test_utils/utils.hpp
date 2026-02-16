#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_TESTS_TEST_UTILS_UTILS_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_TESTS_TEST_UTILS_UTILS_HPP_

#include <limits>
#include <type_traits>

namespace simulator::trading_system::test {

template <typename E>
static constexpr auto invalid_enum_value() -> E {
  constexpr auto invalid_value =
      std::numeric_limits<std::underlying_type_t<E>>::max();
  return static_cast<E>(invalid_value);
}

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_TESTS_TEST_UTILS_UTILS_HPP_
