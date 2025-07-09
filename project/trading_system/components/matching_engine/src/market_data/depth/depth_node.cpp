#include "ih/market_data/depth/depth_node.hpp"

#include "ih/market_data/depth/depth_level.hpp"
#include "ih/market_data/tools/algorithms.hpp"

namespace simulator::trading_system::matching_engine::mdata {

DepthNode::DepthNode(MarketEntryId identifier, const OrderAdded& initial)
    : record_(std::move(identifier),
              initial.order_price,
              to_entry_type(initial.order_side)) {
  apply(initial);
}

auto DepthNode::empty() const -> bool {
  return is_empty_quantity(quantities_.full_quantity());
}

auto DepthNode::full_level() const -> DepthLevel {
  return produce_level(prev_quantities_.full_quantity(),
                       quantities_.full_quantity());
}

auto DepthNode::partial_level(const PartyId& excluded_owner) const
    -> DepthLevel {
  return produce_level(prev_quantities_.partial_quantity(excluded_owner),
                       quantities_.partial_quantity(excluded_owner));
}

auto DepthNode::apply(const OrderAdded& action) -> void {
  quantities_.apply(action);
}

auto DepthNode::apply(const OrderReduced& action) -> void {
  quantities_.apply(action);
}

auto DepthNode::apply(const OrderRemoved& action) -> void {
  quantities_.apply(action);
}

auto DepthNode::fold() -> void { prev_quantities_ = quantities_; }

auto DepthNode::produce_level(const Quantity previous,
                              const Quantity current) const -> DepthLevel {
  if (is_empty_quantity(previous)) {
    return is_empty_quantity(current)
               ? DepthLevel::make_invisible(record_)
               : DepthLevel::make_added(record_, current);
  }
  if (is_empty_quantity(current)) {
    return DepthLevel::make_removed(record_);
  }
  if (current == previous) {
    return DepthLevel::make_unchanged(record_, current);
  }
  return DepthLevel::make_changed(record_, current);
}

}  // namespace simulator::trading_system::matching_engine::mdata