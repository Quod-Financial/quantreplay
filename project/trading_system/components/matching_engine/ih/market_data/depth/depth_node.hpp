#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_NODE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_NODE_HPP_

#include "ih/common/data/market_data_updates.hpp"
#include "ih/market_data/depth/depth_level.hpp"
#include "ih/market_data/depth/depth_quantity_list.hpp"
#include "ih/market_data/depth/depth_record.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class DepthNode {
 public:
  DepthNode(MarketEntryId identifier, const OrderAdded& initial);

  auto id() const -> const MarketEntryId& { return record_.id(); }

  auto price() const -> std::optional<Price> { return record_.price(); }

  auto empty() const -> bool;

  auto full_level() const -> DepthLevel;

  auto partial_level(const PartyId& excluded_owner) const -> DepthLevel;

  auto apply(const OrderAdded& action) -> void;

  auto apply(const OrderReduced& action) -> void;

  auto apply(const OrderRemoved& action) -> void;

  auto fold() -> void;

 private:
  auto produce_level(Quantity previous, Quantity current) const -> DepthLevel;

  DepthRecord record_;
  DepthQuantityList prev_quantities_;
  DepthQuantityList quantities_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // DEPTH_NODE_HPP
