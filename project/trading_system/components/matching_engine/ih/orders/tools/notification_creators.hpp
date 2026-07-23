#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_NOTIFICATION_CREATORS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_NOTIFICATION_CREATORS_HPP_

#include <optional>

#include "common/trade.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/market_phase.hpp"
#include "core/domain/party.hpp"
#include "core/tools/time.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"

namespace simulator::trading_system::matching_engine::order {

[[nodiscard]]
auto make_making_order_added_to_book_notification(const LimitOrder& order)
    -> OrderBookNotification;

[[nodiscard]]
auto make_making_order_added_to_book_notification(const MarketOrder& order)
    -> OrderBookNotification;

[[nodiscard]]
auto make_making_order_removed_from_book_notification(const LimitOrder& order)
    -> OrderBookNotification;

[[nodiscard]]
auto make_making_order_removed_from_book_notification(const MarketOrder& order)
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

[[nodiscard]]
auto make_buyer_id(const std::optional<Party>& owner) -> std::optional<BuyerId>;

[[nodiscard]]
auto make_seller_id(const std::optional<Party>& owner)
    -> std::optional<SellerId>;

template <typename BuyOrder, typename SellOrder>
[[nodiscard]]
auto make_auction_trade_notification(const BuyOrder& buy_order,
                                     const SellOrder& sell_order,
                                     ExecutionPrice trade_price,
                                     ExecutedQuantity traded_quantity,
                                     MarketPhase market_phase)
    -> OrderBookNotification {
  return OrderBookNotification(
      Trade{.buyer = make_buyer_id(buy_order.owner()),
            .seller = make_seller_id(sell_order.owner()),
            .trade_price = static_cast<Price>(trade_price),
            .traded_quantity = static_cast<Quantity>(traded_quantity),
            .aggressor_side = std::nullopt,
            .trade_time = core::get_current_system_time(),
            .market_phase = market_phase});
}

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_TOOLS_NOTIFICATION_CREATORS_HPP_
