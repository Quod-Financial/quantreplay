#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_ORDER_LOOKUP_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_ORDER_LOOKUP_HPP_

#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_updates.hpp"

namespace simulator::trading_system::matching_engine {

[[nodiscard]]
auto find_target_limit_order(LimitOrdersContainer& orders,
                             const LimitUpdate& update)
    -> LimitOrdersContainer::iterator;

[[nodiscard]]
auto find_target_market_order(MarketOrdersContainer& orders,
                              const MarketUpdate& update)
    -> MarketOrdersContainer::iterator;

[[nodiscard]]
auto find_target_limit_order(LimitOrdersContainer& orders,
                             const OrderCancel& cancel)
    -> LimitOrdersContainer::iterator;

[[nodiscard]]
auto find_target_market_order(MarketOrdersContainer& orders,
                              const OrderCancel& cancel)
    -> MarketOrdersContainer::iterator;

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_ORDER_LOOKUP_HPP_
