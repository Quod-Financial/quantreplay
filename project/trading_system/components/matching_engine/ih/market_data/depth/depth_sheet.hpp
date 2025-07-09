#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_SHEET_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_SHEET_HPP_

#include <ranges>
#include <vector>

#include "core/domain/attributes.hpp"
#include "ih/common/data/market_data_updates.hpp"
#include "ih/market_data/depth/depth_node.hpp"
#include "ih/market_data/depth/depth_node_comparator.hpp"
#include "ih/market_data/tools/market_entry_id_generator.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class DepthSheet {
 public:
  DepthSheet(std::unique_ptr<DepthNodeComparator> cmp,
             MarketEntryIdGenerator& idgen);

  DepthSheet() = delete;
  DepthSheet(const DepthSheet&) = delete;
  DepthSheet(DepthSheet&&) noexcept = default;
  ~DepthSheet() = default;

  auto operator=(const DepthSheet&) -> DepthSheet& = delete;
  auto operator=(DepthSheet&&) noexcept -> DepthSheet& = default;

  auto view() const&;

  auto partial_view(const PartyId& excluded_owner) const&;

  auto apply(const OrderAdded& action) -> void;

  auto apply(const OrderReduced& action) -> void;

  auto apply(const OrderRemoved& action) -> void;

  auto fold() -> void;

  static auto create_bid_sheet(MarketEntryIdGenerator& idgen) -> DepthSheet;

  static auto create_offer_sheet(MarketEntryIdGenerator& idgen) -> DepthSheet;

 private:
  std::vector<DepthNode> nodes_;
  std::unique_ptr<DepthNodeComparator> cmp_;
  gsl::not_null<MarketEntryIdGenerator*> idgen_;
};

inline auto DepthSheet::view() const& {
  return nodes_ | std::views::reverse |
         std::views::transform(
             [](const auto& node) { return node.full_level(); });
}

inline auto DepthSheet::partial_view(const PartyId& excluded_owner) const& {
  return nodes_ | std::views::reverse |
         std::views::transform([&](const auto& node) {
           return node.partial_level(excluded_owner);
         });
}

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_SHEET_HPP_
