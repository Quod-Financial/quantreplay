#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_ORDER_BOOK_SIDE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_ORDER_BOOK_SIDE_HPP_

#include <cstdint>

namespace simulator::trading_system::matching_engine::order {

enum class OrderBookSide : std::uint8_t { Buy, Sell };

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_ORDER_BOOK_SIDE_HPP_
