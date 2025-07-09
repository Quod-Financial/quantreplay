#include "ih/contexts/order_id_context.hpp"

#include <cstdlib>

#include "idgen/order_id.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::idgen {

OrderIdContext::OrderIdContext(std::unique_ptr<Implementation> impl) noexcept
    : impl_(std::move(impl)) {}

OrderIdContext::~OrderIdContext() noexcept = default;

auto OrderIdContext::implementation() & noexcept -> Implementation& {
  if (impl_) [[likely]] {
    return *impl_;
  }

  // This is a fatal idgen-internal error, cannot continue execution
  log::err(
      "CRITICAL: order id generation context implementation has not "
      "been allocated/initialized, the module contains critical bug, can not "
      "continue program execution, aborting...");
  std::abort();
}

}  // namespace simulator::trading_system::idgen