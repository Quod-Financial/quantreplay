#ifndef SIMULATOR_INSTRUMENTS_IH_LOOKUP_MATCHERS_HPP_
#define SIMULATOR_INSTRUMENTS_IH_LOOKUP_MATCHERS_HPP_

#include "common/instrument.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "ih/lookup/match_rate.hpp"

namespace simulator::trading_system::instrument::lookup {

[[nodiscard]]
auto match_symbol(const InstrumentDescriptor& descriptor,
                  const Instrument& instrument) -> MatchRate;

[[nodiscard]]
auto match_sedol_id(const InstrumentDescriptor& descriptor,
                    const Instrument& instrument) -> MatchRate;

[[nodiscard]]
auto match_cusip_id(const InstrumentDescriptor& descriptor,
                    const Instrument& instrument) -> MatchRate;

[[nodiscard]]
auto match_isin_id(const InstrumentDescriptor& descriptor,
                   const Instrument& instrument) -> MatchRate;

[[nodiscard]]
auto match_ric_id(const InstrumentDescriptor& descriptor,
                  const Instrument& instrument) -> MatchRate;

[[nodiscard]]
auto match_exchange_id(const InstrumentDescriptor& descriptor,
                       const Instrument& instrument) -> MatchRate;

[[nodiscard]]
auto match_bloomberg_id(const InstrumentDescriptor& descriptor,
                        const Instrument& instrument) -> MatchRate;

[[nodiscard]]
auto match_security_type(const InstrumentDescriptor& descriptor,
                         const Instrument& instrument) -> MatchRate;

[[nodiscard]]
auto match_security_exchange(const InstrumentDescriptor& descriptor,
                             const Instrument& instrument) -> MatchRate;

[[nodiscard]]
auto match_currency(const InstrumentDescriptor& descriptor,
                    const Instrument& instrument) -> MatchRate;

[[nodiscard]]
auto match_party(const InstrumentDescriptor& descriptor,
                 const Instrument& instrument) -> MatchRate;

[[nodiscard]]
constexpr auto make_matcher(auto... matchers) {
  static_assert(sizeof...(matchers) > 0);
  return [matchers...](const InstrumentDescriptor& descriptor,
                       const Instrument& instrument) {
    return (matchers(descriptor, instrument) + ...);
  };
}

}  // namespace simulator::trading_system::instrument::lookup

#endif  // SIMULATOR_INSTRUMENTS_IH_LOOKUP_MATCHERS_HPP_
