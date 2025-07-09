#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_ALGORITHMS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_ALGORITHMS_HPP_

#include <algorithm>

#include "core/common/unreachable.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/depth/depth_level.hpp"

namespace simulator::trading_system::matching_engine::mdata {

[[nodiscard]]
constexpr auto is_empty_quantity(const Quantity& quantity) -> bool {
  return quantity <= Quantity{0};
}

[[nodiscard]]
constexpr auto contains(const auto& collection, const MdEntryType type)
    -> bool {
  return std::ranges::find(collection, type) != std::end(collection);
}

[[nodiscard]]
constexpr auto contains(const auto& collection, const MdEntryType::Option type)
    -> bool {
  return contains(collection, static_cast<MdEntryType>(type));
}

[[nodiscard]]
constexpr auto contains(const auto& collection,
                        const std::optional<MdEntryType> type) -> bool {
  return type.has_value() ? contains(collection, *type) : false;
}

[[nodiscard]]
constexpr auto contains(const std::optional<auto>& collection,
                        const MdEntryType::Option type) -> bool {
  return collection.has_value() ? contains(*collection, type) : false;
}

[[nodiscard]]
constexpr auto to_entry_type(const Side side) -> MdEntryType {
  switch (static_cast<Side::Option>(side)) {
    case Side::Option::Buy:
      return MdEntryType::Option::Bid;
    case Side::Option::Sell:
    case Side::Option::SellShort:
    case Side::Option::SellShortExempt:
      return MdEntryType::Option::Offer;
  }
  core::unreachable();
}

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_ALGORITHMS_HPP_
