#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_NODE_COMPARATOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_NODE_COMPARATOR_HPP_

#include <optional>

#include "core/domain/attributes.hpp"
#include "ih/market_data/depth/depth_node.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class DepthNodeComparator {
 public:
  DepthNodeComparator() = default;
  DepthNodeComparator(const DepthNodeComparator&) = default;
  DepthNodeComparator(DepthNodeComparator&&) noexcept = default;
  virtual ~DepthNodeComparator() = default;

  auto operator=(const DepthNodeComparator&) -> DepthNodeComparator& = default;
  auto operator=(DepthNodeComparator&&) noexcept
      -> DepthNodeComparator& = default;

  auto operator()(const DepthNode& lhs, const DepthNode& rhs) const -> bool {
    return compare(lhs.price(), rhs.price());
  }

  auto operator()(const std::optional<Price> lhs,
                  const std::optional<Price> rhs) const -> bool {
    return compare(lhs, rhs);
  }

 private:
  virtual auto compare(std::optional<Price> lhs, std::optional<Price> rhs) const
      -> bool = 0;
};

class BidComparator final : public DepthNodeComparator {
  auto compare(const std::optional<Price> lhs,
               const std::optional<Price> rhs) const -> bool override {
    return lhs.has_value() ? lhs >= rhs : rhs.has_value();
  }
};

class OfferComparator final : public DepthNodeComparator {
  auto compare(const std::optional<Price> lhs,
               const std::optional<Price> rhs) const -> bool override {
    return rhs.has_value() ? lhs <= rhs : lhs.has_value();
  }
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_NODE_COMPARATOR_HPP_
