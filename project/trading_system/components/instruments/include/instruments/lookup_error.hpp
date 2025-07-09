#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_LOOKUP_ERROR_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_LOOKUP_ERROR_HPP_

#include <cstdint>

namespace simulator::trading_system::instrument {

enum class LookupError : std::uint8_t {
  InstrumentNotFound,
  MalformedInstrumentDescriptor,
  AmbiguousInstrumentDescriptor,
};

}  // namespace simulator::trading_system::instrument

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_LOOKUP_ERROR_HPP_
