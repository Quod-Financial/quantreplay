#include "ih/market_data/depth/depth_sheet.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "ih/market_data/depth/depth_node_comparator.hpp"

namespace simulator::trading_system::matching_engine::mdata {

DepthSheet::DepthSheet(std::unique_ptr<DepthNodeComparator> cmp,
                       MarketEntryIdGenerator& idgen)
    : cmp_(std::move(cmp)), idgen_(&idgen) {}

auto DepthSheet::apply(const OrderAdded& action) -> void {
  const auto iter = std::ranges::upper_bound(
      nodes_,
      action.order_price,
      [this](auto new_price, const auto& node) -> bool {
        return (*cmp_)(node.price(), new_price);
      });

  if (iter != nodes_.end() && iter->price() == action.order_price) {
    iter->apply(action);
  } else {
    nodes_.emplace(iter, std::invoke(*idgen_), action);
  }
}

auto DepthSheet::apply(const OrderReduced& action) -> void {
  const auto iter = std::ranges::find_if(nodes_, [&](const auto& node) {
    return node.price() == action.order_price;
  });

  if (iter != nodes_.end()) [[likely]] {
    iter->apply(action);
    return;
  }

  throw std::logic_error(
      fmt::format("DepthSheet::apply - can not apply OrderReduced action, "
                  "price '{}' was not found",
                  action.order_price));
}

auto DepthSheet::apply(const OrderRemoved& action) -> void {
  const auto iter = std::ranges::find_if(nodes_, [&](const auto& node) {
    return node.price() == action.order_price;
  });

  if (iter != nodes_.end()) [[likely]] {
    iter->apply(action);
    return;
  }

  throw std::logic_error(
      fmt::format("DepthSheet::apply - can not apply OrderRemoved action, "
                  "price '{}' was not found",
                  action.order_price));
}

auto DepthSheet::fold() -> void {
  std::erase_if(nodes_, [](const auto& node) { return node.empty(); });
  std::ranges::for_each(nodes_, [](auto& node) { node.fold(); });
}

auto DepthSheet::create_bid_sheet(MarketEntryIdGenerator& idgen) -> DepthSheet {
  return {std::make_unique<BidComparator>(), idgen};
}

auto DepthSheet::create_offer_sheet(MarketEntryIdGenerator& idgen)
    -> DepthSheet {
  return {std::make_unique<OfferComparator>(), idgen};
}

}  // namespace simulator::trading_system::matching_engine::mdata