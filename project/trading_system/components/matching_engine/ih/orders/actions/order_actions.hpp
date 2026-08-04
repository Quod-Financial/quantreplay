#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_ORDER_ACTIONS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_ORDER_ACTIONS_HPP_

#include <cstdint>
#include <optional>

#include "common/attributes.hpp"
#include "common/instrument_state.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"

namespace simulator::trading_system::matching_engine {

enum class OrderActionMode : std::uint8_t { Regular, AuctionCall };

auto place_limit_order(EventListener& event_listener,
                       OrderBook& order_book,
                       std::optional<PriceTick> price_tick,
                       LimitOrder order,
                       OrderActionMode mode) -> OrderBookUpdates;

auto place_market_order(EventListener& event_listener,
                        OrderBook& order_book,
                        std::optional<PriceTick> price_tick,
                        MarketOrder order,
                        OrderActionMode mode) -> OrderBookUpdates;

auto amend_limit_order(EventListener& event_listener,
                       OrderBook& order_book,
                       std::optional<PriceTick> price_tick,
                       LimitUpdate update,
                       OrderActionMode mode) -> OrderBookUpdates;

auto amend_market_order(EventListener& event_listener,
                        OrderBook& order_book,
                        std::optional<PriceTick> price_tick,
                        MarketUpdate update) -> OrderBookUpdates;

auto cancel_order(EventListener& event_listener,
                  OrderBook& order_book,
                  std::optional<PriceTick> price_tick,
                  const OrderCancel& cancel) -> OrderBookUpdates;

auto recover_order(EventListener& event_listener,
                   OrderBook& order_book,
                   market_state::LimitOrder order_state) -> void;

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_ORDER_ACTIONS_HPP_
