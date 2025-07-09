#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_NOTIFICATION_CREATORS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_NOTIFICATION_CREATORS_HPP_

#include "core/domain/attributes.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"

namespace simulator::trading_system::matching_engine::order {

[[nodiscard]]
auto make_making_order_added_to_book_notification(const LimitOrder& order)
    -> OrderBookNotification;

[[nodiscard]]
auto make_making_order_removed_from_book_notification(const LimitOrder& order)
    -> OrderBookNotification;

[[nodiscard]]
auto make_making_order_reduced_notification(const LimitOrder& maker)
    -> OrderBookNotification;

[[nodiscard]]
auto make_trade_notification(const LimitOrder& taker,
                             const LimitOrder& maker,
                             ExecutionPrice trade_price,
                             ExecutedQuantity traded_quantity)
    -> OrderBookNotification;

[[nodiscard]]
auto make_trade_notification(const MarketOrder& taker,
                             const LimitOrder& maker,
                             ExecutionPrice trade_price,
                             ExecutedQuantity traded_quantity)
    -> OrderBookNotification;

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_NOTIFICATION_CREATORS_HPP_
