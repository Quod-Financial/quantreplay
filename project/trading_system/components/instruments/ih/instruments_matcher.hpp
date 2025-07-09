#ifndef SIMULATOR_INSTRUMENTS_IH_INSTRUMENTS_MATCHER_HPP_
#define SIMULATOR_INSTRUMENTS_IH_INSTRUMENTS_MATCHER_HPP_

#include "common/instrument.hpp"
#include "instruments/matcher.hpp"

namespace simulator::trading_system::instrument {

class Matcher::Implementation {
 public:
  explicit Implementation(const Instrument& instrument);

  auto operator()(const InstrumentDescriptor& descriptor) const
      -> tl::expected<void, LookupError>;

 private:
  Instrument instrument_;
};

}  // namespace simulator::trading_system::instrument

#endif  // SIMULATOR_INSTRUMENTS_IH_INSTRUMENTS_MATCHER_HPP_
