#ifndef SIMULATOR_HTTP_TESTS_TEST_UTILS_UTILS_HPP_
#define SIMULATOR_HTTP_TESTS_TEST_UTILS_UTILS_HPP_

#include <limits>
#include <type_traits>

namespace simulator::http::test::util {

template <typename E>
static constexpr auto invalid_enum_value() -> E {
  constexpr auto invalid_value =
      std::numeric_limits<std::underlying_type_t<E>>::max();
  return static_cast<E>(invalid_value);
}

}  // namespace simulator::http::test::util

#endif  // SIMULATOR_HTTP_TESTS_TEST_UTILS_UTILS_HPP_
