#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_TAKER_CANCELLATION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_TAKER_CANCELLATION_HPP_

#include <optional>

#include "common/attributes.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"

namespace simulator::trading_system::matching_engine {

auto cancel_without_facing_orders(EventListener& event_listener,
                                  LimitOrder& order,
                                  std::optional<PriceTick> price_tick) -> void;

auto cancel_without_facing_orders(EventListener& event_listener,
                                  MarketOrder& order,
                                  std::optional<PriceTick> price_tick) -> void;

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_TAKER_CANCELLATION_HPP_
