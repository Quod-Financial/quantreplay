#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_REQUIREMENT_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_REQUIREMENT_HPP_

#include <algorithm>
#include <cstdint>
#include <optional>

#include "core/domain/attributes.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata {

using DatasourceId = std::uint64_t;

constexpr MarketDepth AllDepthLevels{0};
constexpr MarketDepth TopOfTheBookDepth{1};

struct RandomPriceSourceKey {
  DatasourceId datasource_id{};
  Symbol symbol;

  [[nodiscard]]
  auto operator==(const RandomPriceSourceKey&) const -> bool = default;

  [[nodiscard]]
  auto operator<=>(const RandomPriceSourceKey&) const = default;
};

struct SubscriptionRequirement {
  protocol::Session session;
  Symbol symbol;
  DatasourceId datasource_id{};
  MarketDepth depth{AllDepthLevels};
  // Set only for a datasource whose market data is turned into orders.
  std::optional<PartyId> order_counterparty;
  MarketDataUpdateType update_type{MarketDataUpdateType::Option::Snapshot};

  [[nodiscard]]
  auto operator==(const SubscriptionRequirement&) const -> bool = default;
};

// Levels the feed reports without a party are derived under the counterparty.
struct OrderDerivationRequirement {
  Symbol symbol;
  MarketDepth depth{AllDepthLevels};
  PartyId counterparty;

  [[nodiscard]]
  auto operator==(const OrderDerivationRequirement&) const -> bool = default;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_REQUIREMENT_HPP_
