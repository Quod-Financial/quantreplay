#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_QUANTITY_LIST_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_QUANTITY_LIST_HPP_

#include <cstddef>
#include <optional>
#include <vector>

#include "common/attributes.hpp"
#include "core/domain/attributes.hpp"
#include "ih/common/data/market_data_updates.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class DepthQuantityList {
  struct Component {
    OrderId order_id;
    Quantity order_quantity;
    std::optional<std::size_t> order_owner_hash;
  };

 public:
  auto full_quantity() const -> Quantity;

  auto partial_quantity(const PartyId& excluded_owner) const -> Quantity;

  auto apply(const OrderAdded& action) -> void;

  auto apply(const OrderReduced& action) -> void;

  auto apply(const OrderRemoved& action) -> void;

 private:
  static auto hash(const PartyId& owner) -> std::size_t;

  static auto hash(const std::optional<PartyId>& owner)
      -> std::optional<std::size_t>;

  constexpr static auto order_predicate(const OrderId& order_id) {
    return [order_id](const Component& component) {
      return component.order_id == order_id;
    };
  }

  constexpr static auto accumulate_function() {
    return [](const Quantity result, const Component& component) {
      return Quantity{static_cast<double>(result) +
                      static_cast<double>(component.order_quantity)};
    };
  }

  std::vector<Component> components_;
  mutable std::optional<Quantity> cached_total_quantity_{0};
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_QUANTITY_LIST_HPP_
