#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_BOOK_UPDATE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_BOOK_UPDATE_HPP_

#include <cstdint>
#include <optional>
#include <vector>

#include "core/domain/attributes.hpp"

namespace simulator::trading_system::matching_engine {

// To report amendment, use a Remove followed by an Add
struct OrderBookUpdate {
  enum class Action : std::uint8_t { Add, Remove };

  Side side;
  Action action;
  std::optional<OrderPrice> price;
  LeavesQuantity quantity;

  auto operator==(const OrderBookUpdate&) const -> bool = default;
};

using OrderBookUpdates = std::vector<OrderBookUpdate>;

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_BOOK_UPDATE_HPP_
