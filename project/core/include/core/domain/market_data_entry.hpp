#ifndef SIMULATOR_CORE_DOMAIN_MARKET_DATA_ENTRY_HPP_
#define SIMULATOR_CORE_DOMAIN_MARKET_DATA_ENTRY_HPP_

#include <fmt/base.h>

#include <iosfwd>
#include <optional>

#include "core/common/name.hpp"
#include "core/common/std_formatter.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/market_phase.hpp"

namespace simulator {

struct MarketDataEntry {
  std::optional<MarketEntryId> id;
  std::optional<BuyerId> buyer_id;
  std::optional<SellerId> seller_id;
  std::optional<MarketEntryTime> time;
  std::optional<Price> price;
  std::optional<Quantity> quantity;
  std::optional<MarketPhase> phase;
  std::optional<AggressorSide> aggressor_side;
  std::optional<MarketEntryAction> action;
  std::optional<MdEntryType> type;

  [[nodiscard]]
  consteval static auto name() {
    return core::Name{.singular = "MarketDataEntry",
                      .plural = "MarketDataEntries"};
  }
};

auto operator<<(std::ostream& ostream, const MarketDataEntry& entry)
    -> std::ostream&;

}  // namespace simulator

template <>
struct fmt::formatter<simulator::MarketDataEntry> {
  using formattable = simulator::MarketDataEntry;

  constexpr auto parse(format_parse_context& context) {
    return context.begin();
  }

  auto format(const formattable& entry, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_CORE_DOMAIN_MARKET_DATA_ENTRY_HPP_
