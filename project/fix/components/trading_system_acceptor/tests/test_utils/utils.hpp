#ifndef SIMULATOR_TRADING_SYSTEM_ACCEPTOR_TESTS_TEST_UTILS_UTILS_HPP_
#define SIMULATOR_TRADING_SYSTEM_ACCEPTOR_TESTS_TEST_UTILS_UTILS_HPP_

#include <limits>
#include <type_traits>

namespace simulator::fix::trading_system_acceptor::test {

template <typename E>
static constexpr auto invalid_enum_value() -> E {
  constexpr auto invalid_value =
      std::numeric_limits<std::underlying_type_t<E>>::max();
  return static_cast<E>(invalid_value);
}

}  // namespace simulator::fix::trading_system_acceptor::test

#endif  // SIMULATOR_TRADING_SYSTEM_ACCEPTOR_TESTS_TEST_UTILS_UTILS_HPP_
