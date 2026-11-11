#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_PLACEMENT_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_PLACEMENT_HPP_

#include <gsl/pointers>
#include <optional>
#include <string_view>

#include "core/domain/attributes.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"

namespace simulator::trading_system::matching_engine {

namespace auction {

constexpr std::string_view ImmediateOrdersNotAcceptedReject =
    "immediate-or-cancel and fill-or-kill orders are not accepted during an "
    "auction";

[[nodiscard]]
auto acceptance_error(TimeInForce time_in_force)
    -> std::optional<std::string_view>;

[[nodiscard]]
auto rest_in_book(EventListener& event_listener,
                  OrderBook& order_book,
                  const LimitOrder& order) -> OrderBookUpdate;

[[nodiscard]]
auto rest_in_book(EventListener& event_listener,
                  OrderBook& order_book,
                  const MarketOrder& order) -> OrderBookUpdate;

}  // namespace auction

class AuctionPlacement : private EventReporter {
 public:
  AuctionPlacement(EventListener& event_listener, OrderBook& order_book);

  AuctionPlacement(const AuctionPlacement&) = default;
  AuctionPlacement(AuctionPlacement&&) = default;
  ~AuctionPlacement() override = default;

  auto operator=(const AuctionPlacement&) -> AuctionPlacement& = delete;
  auto operator=(AuctionPlacement&&) -> AuctionPlacement& = delete;

  auto operator()(LimitOrder order) -> OrderBookUpdates;

  auto operator()(MarketOrder order) -> OrderBookUpdates;

 private:
  gsl::not_null<OrderBook*> order_book_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_PLACEMENT_HPP_
