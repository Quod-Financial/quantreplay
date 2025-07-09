#include "ih/contexts/market_entry_id_context.hpp"

#include "idgen/market_entry_id.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::idgen {

MarketEntryIdContext::MarketEntryIdContext(
    std::unique_ptr<Implementation> impl) noexcept
    : impl_(std::move(impl)) {}

MarketEntryIdContext::MarketEntryIdContext(MarketEntryIdContext&&) noexcept =
    default;

auto MarketEntryIdContext::operator=(MarketEntryIdContext&&) noexcept
    -> MarketEntryIdContext& = default;

MarketEntryIdContext::~MarketEntryIdContext() noexcept = default;

auto MarketEntryIdContext::implementation() & noexcept
    -> MarketEntryIdContext::Implementation& {
  if (impl_) [[likely]] {
    return *impl_;
  }

  // This is a fatal idgen-internal error, cannot continue execution
  log::err(
      "CRITICAL: market entry id generation context implementation "
      "has not been allocated/initialized, the module contains critical bug, "
      "can not continue program execution, aborting...");
  std::abort();
}

}  // namespace simulator::trading_system::idgen
