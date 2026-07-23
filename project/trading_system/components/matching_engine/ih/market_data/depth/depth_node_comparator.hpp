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

// A price-less (market) order is the most aggressive participant on its side of
// the book, so a nullopt price is treated as the best price by both comparators
// and therefore ranks ahead of every priced level.
//
// These comparators are consumed only as a std::upper_bound partition predicate
// in DepthSheet::apply, never as a strict-weak-ordering for sort/set, so
// compare(nullopt, nullopt) == true is intentional: it makes all market orders
// on a side aggregate into a single null-priced depth node.
class BidComparator final : public DepthNodeComparator {
  auto compare(const std::optional<Price> lhs,
               const std::optional<Price> rhs) const -> bool override {
    if (!lhs.has_value()) {
      return true;
    }
    return rhs.has_value() ? lhs >= rhs : false;
  }
};

class OfferComparator final : public DepthNodeComparator {
  auto compare(const std::optional<Price> lhs,
               const std::optional<Price> rhs) const -> bool override {
    if (!lhs.has_value()) {
      return true;
    }
    return rhs.has_value() ? lhs <= rhs : false;
  }
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_NODE_COMPARATOR_HPP_
