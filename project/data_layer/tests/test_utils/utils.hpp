#ifndef SIMULATOR_DATA_LAYER_TESTS_TEST_UTILS_UTILS_HPP_
#define SIMULATOR_DATA_LAYER_TESTS_TEST_UTILS_UTILS_HPP_

#include <limits>
#include <type_traits>

namespace simulator::data_layer::test {

template <typename E>
static constexpr auto invalid_enum_value() -> E {
  constexpr auto invalid_value =
      std::numeric_limits<std::underlying_type_t<E>>::max();
  return static_cast<E>(invalid_value);
}

}  // namespace simulator::data_layer::test

#endif  // SIMULATOR_DATA_LAYER_TESTS_TEST_UTILS_UTILS_HPP_
