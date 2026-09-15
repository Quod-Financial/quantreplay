#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_REQUIREMENT_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_REQUIREMENT_HPP_

#include <algorithm>

#include "core/domain/attributes.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata {

constexpr MarketDepth AllDepthLevels{0};
constexpr MarketDepth TopOfTheBookDepth{1};

[[nodiscard]]
inline auto merge_depth(MarketDepth lhs,
                        MarketDepth rhs) noexcept -> MarketDepth {
  if (lhs == AllDepthLevels || rhs == AllDepthLevels) {
    return AllDepthLevels;
  }
  return std::max(lhs, rhs);
}

// Identifies a market data subscription, which is unique per session and
// symbol.
struct SubscriptionKey {
  protocol::Session session;
  Symbol symbol;

  [[nodiscard]]
  auto operator==(const SubscriptionKey&) const -> bool = default;
};

struct SubscriptionRequirement {
  protocol::Session session;
  Symbol symbol;
  MarketDepth depth{AllDepthLevels};

  [[nodiscard]]
  auto operator==(const SubscriptionRequirement&) const -> bool = default;
};

// Levels the feed reports without a party are derived under the counterparty.
struct OrderDerivationRequirement {
  protocol::Session session;
  Symbol symbol;
  MarketDepth depth{AllDepthLevels};
  PartyId counterparty;

  [[nodiscard]]
  auto operator==(const OrderDerivationRequirement&) const -> bool = default;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_REQUIREMENT_HPP_
