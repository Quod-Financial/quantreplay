#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_TESTS_TEST_UTILS_UTILS_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_TESTS_TEST_UTILS_UTILS_HPP_

#include <limits>
#include <type_traits>

#include "common/instrument.hpp"

namespace simulator::trading_system::instrument::test {

template <typename E>
static constexpr auto invalid_enum_value() -> E {
  constexpr auto invalid_value =
      std::numeric_limits<std::underlying_type_t<E>>::max();
  return static_cast<E>(invalid_value);
}

[[nodiscard]]
inline auto make_instrument(InstrumentId instrument_id) noexcept -> Instrument {
  Instrument instrument;
  instrument.identifier = instrument_id;
  return instrument;
}

}  // namespace simulator::trading_system::instrument::test

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_TESTS_TEST_UTILS_UTILS_HPP_
