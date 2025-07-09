#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_PERSISTENT_STATE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_PERSISTENT_STATE_HPP_

#include "common/market_state/snapshot.hpp"
#include "ih/orders/book/order_book.hpp"

namespace simulator::trading_system::matching_engine {

auto store_order_book_state(OrderBook& order_book,
                            market_state::OrderBook& state) -> void;

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_PERSISTENT_STATE_HPP_
