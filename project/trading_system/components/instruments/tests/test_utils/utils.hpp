#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_TEST_UTILS_UTILS_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_TEST_UTILS_UTILS_HPP_

#include "common/instrument.hpp"

namespace simulator::trading_system::instrument::test {

[[nodiscard]]
inline auto make_instrument(InstrumentId instrument_id) noexcept -> Instrument {
  Instrument instrument;
  instrument.identifier = instrument_id;
  return instrument;
}

}  // namespace simulator::trading_system::instrument::test

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_TEST_UTILS_UTILS_HPP_
