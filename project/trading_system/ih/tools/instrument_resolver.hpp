#ifndef SIMULATOR_TRADING_SYSTEM_IH_TOOLS_INSTRUMENT_RESOLVER_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_TOOLS_INSTRUMENT_RESOLVER_HPP_

#include <memory>
#include <tl/expected.hpp>

#include "core/domain/instrument_descriptor.hpp"
#include "instruments/cache.hpp"
#include "instruments/lookup_error.hpp"
#include "instruments/view.hpp"

namespace simulator::trading_system {

struct InstrumentResolver {
  InstrumentResolver() = default;
  InstrumentResolver(const InstrumentResolver&) = default;
  InstrumentResolver(InstrumentResolver&&) noexcept = default;
  virtual ~InstrumentResolver() = default;

  auto operator=(const InstrumentResolver&) -> InstrumentResolver& = default;
  auto operator=(InstrumentResolver&&) noexcept
      -> InstrumentResolver& = default;

  [[nodiscard]]
  virtual auto resolve_instrument(const InstrumentDescriptor& descriptor) const
      -> tl::expected<instrument::View, instrument::LookupError> = 0;

  [[nodiscard]]
  virtual auto resolve_instrument(const Instrument& instrument) const
      -> tl::expected<instrument::View, instrument::LookupError> = 0;
};

[[nodiscard]]
auto create_cached_instrument_resolver(const instrument::Cache& cache)
    -> std::unique_ptr<InstrumentResolver>;

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_TOOLS_INSTRUMENT_RESOLVER_HPP_
