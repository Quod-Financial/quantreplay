#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_VIEW_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_VIEW_HPP_

#include "common/instrument.hpp"

namespace simulator::trading_system::instrument {

class View {
 public:
  View() = delete;

  // NOLINTNEXTLINE(google-explicit-constructor)
  View(const Instrument& instrument) : instrument_ptr_{&instrument} {}

  auto instrument() const -> const Instrument& { return *instrument_ptr_; }

 private:
  const Instrument* instrument_ptr_ = nullptr;
};

}  // namespace simulator::trading_system::instrument

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_VIEW_HPP_
