#ifndef SIMULATOR_MATCHING_ENGINE_IH_TOOLS_ORDER_RESOLVER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_TOOLS_ORDER_RESOLVER_HPP_

#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_updates.hpp"

namespace simulator::trading_system::matching_engine {

struct AmendLmtOrder;
struct CancelOrder;

}  // namespace simulator::trading_system::matching_engine

namespace simulator::trading_system::matching_engine {

[[nodiscard]]
auto find_target_limit_order(OrderPage& page, const LimitUpdate& update)
    -> LimitOrdersContainer::iterator;

[[nodiscard]]
auto find_target_limit_order(OrderPage& page, const OrderCancel& cancel)
    -> LimitOrdersContainer::iterator;

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_TOOLS_ORDER_RESOLVER_HPP_