#include "ih/contexts/execution_id_context.hpp"

#include <cstdlib>

#include "idgen/execution_id.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::idgen {

ExecutionIdContext::ExecutionIdContext(
    std::unique_ptr<Implementation> impl) noexcept
    : impl_(std::move(impl)) {}

ExecutionIdContext::~ExecutionIdContext() noexcept = default;

ExecutionIdContext::ExecutionIdContext(ExecutionIdContext&&) noexcept = default;

auto ExecutionIdContext::implementation() & noexcept -> Implementation& {
  if (impl_) [[likely]] {
    return *impl_;
  }

  // This is a fatal idgen-internal error, cannot continue execution
  log::err(
      "CRITICAL: execution id generation context implementation "
      "has not been allocated/initialized, the module contains critical bug, "
      "can not continue program execution, aborting...");
  std::abort();
}

}  // namespace simulator::trading_system::idgen