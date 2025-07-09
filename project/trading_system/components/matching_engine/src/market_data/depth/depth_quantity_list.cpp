#include "ih/market_data/depth/depth_quantity_list.hpp"

#include <fmt/format.h>

#include <numeric>
#include <ranges>
#include <stdexcept>
#include <string>

namespace simulator::trading_system::matching_engine::mdata {

auto DepthQuantityList::full_quantity() const -> Quantity {
  if (!cached_total_quantity_) {
    cached_total_quantity_ = std::accumulate(components_.begin(),
                                             components_.end(),
                                             Quantity{0},
                                             accumulate_function());
  }
  return *cached_total_quantity_;
}

auto DepthQuantityList::partial_quantity(const PartyId& excluded_owner) const
    -> Quantity {
  auto range = std::views::filter(
      components_, [hash = hash(excluded_owner)](const Component& comp) {
        return comp.order_owner_hash != hash;
      });

  return std::accumulate(
      range.begin(), range.end(), Quantity{0}, accumulate_function());
}

auto DepthQuantityList::apply(const OrderAdded& action) -> void {
  const auto maybe_duplicated_iter =
      std::ranges::find_if(components_, order_predicate(action.order_id));

  if (maybe_duplicated_iter != std::end(components_)) [[unlikely]] {
    throw std::logic_error(fmt::format(
        "DepthQuantityList::apply: unable to add an order with duplicated "
        "id: {} to the depth quantity list",
        action.order_id));
  }

  components_.emplace_back(
      Component{.order_id = action.order_id,
                .order_quantity = action.order_quantity,
                .order_owner_hash = hash(action.order_owner)});
  cached_total_quantity_.reset();
}

auto DepthQuantityList::apply(const OrderReduced& action) -> void {
  const auto iter =
      std::ranges::find_if(components_, order_predicate(action.order_id));

  if (iter == std::end(components_)) [[unlikely]] {
    throw std::logic_error(fmt::format(
        "DepthQuantityList::apply: unable to reduce an order with id: {} from "
        "the depth quantity list",
        action.order_id));
  }

  if (action.order_quantity <= Quantity{0}) {
    *iter = components_.back();
    components_.pop_back();
  } else {
    iter->order_quantity = action.order_quantity;
  }
  cached_total_quantity_.reset();
}

auto DepthQuantityList::apply(const OrderRemoved& action) -> void {
  const auto iter =
      std::ranges::find_if(components_, order_predicate(action.order_id));

  if (iter == std::end(components_)) [[unlikely]] {
    throw std::logic_error(fmt::format(
        "DepthQuantityList::apply: unable to remove an order with id: {} from "
        "the depth quantity list",
        action.order_id));
  }

  *iter = components_.back();
  components_.pop_back();
  cached_total_quantity_.reset();
}

auto DepthQuantityList::hash(const PartyId& owner) -> std::size_t {
  return std::hash<std::string>()(static_cast<const std::string&>(owner));
}

auto DepthQuantityList::hash(const std::optional<PartyId>& owner)
    -> std::optional<std::size_t> {
  std::optional<std::size_t> result;
  if (owner.has_value()) {
    result = hash(*owner);
  }
  return result;
}

}  // namespace simulator::trading_system::matching_engine::mdata