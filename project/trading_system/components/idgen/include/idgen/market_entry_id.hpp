#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_MARKET_ENTRY_ID_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_MARKET_ENTRY_ID_HPP_

#include <memory>

#include "common/attributes.hpp"
#include "idgen/errors.hpp"

namespace simulator::trading_system::idgen {

struct MarketEntryIdContext {
  struct Implementation;

  explicit MarketEntryIdContext(std::unique_ptr<Implementation> impl) noexcept;

  MarketEntryIdContext(const MarketEntryIdContext&) = delete;
  MarketEntryIdContext(MarketEntryIdContext&&) noexcept;

  auto operator=(const MarketEntryIdContext&) -> MarketEntryIdContext& = delete;
  auto operator=(MarketEntryIdContext&&) noexcept -> MarketEntryIdContext&;

  ~MarketEntryIdContext() noexcept;

  [[nodiscard]]
  auto implementation() & noexcept -> Implementation&;

 private:
  std::unique_ptr<Implementation> impl_;
};

// Initialize new market entry identifier generation context.
[[nodiscard]]
auto make_market_entry_id_generation_ctx() -> MarketEntryIdContext;

// Generate new market entry identifier.
[[nodiscard]]
auto generate_new_id(MarketEntryIdContext& ctx) -> MarketEntryId;

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_MARKET_ENTRY_ID_HPP_