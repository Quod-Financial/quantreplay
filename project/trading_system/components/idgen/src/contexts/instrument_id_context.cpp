#include "ih/contexts/instrument_id_context.hpp"

#include <cstdlib>

#include "idgen/instrument_id.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::idgen {

InstrumentIdContext::InstrumentIdContext(
    std::unique_ptr<Implementation> impl) noexcept
    : impl_(std::move(impl)) {}

InstrumentIdContext::~InstrumentIdContext() noexcept { impl_.reset(); }

auto InstrumentIdContext::implementation() & noexcept -> Implementation& {
  if (impl_) [[likely]] {
    return *impl_;
  }

  log::err(
      "CRITICAL: instrument id generation context implementation "
      "has not been allocated/initialized, the module contains critical bug, "
      "can not continue program execution, aborting...");

  // This is a fatal idgen-internal error, cannot continue execution
  std::abort();
}

}  // namespace simulator::trading_system::idgen