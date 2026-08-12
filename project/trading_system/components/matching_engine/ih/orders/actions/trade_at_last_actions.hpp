#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_TRADE_AT_LAST_ACTIONS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_TRADE_AT_LAST_ACTIONS_HPP_

#include <optional>
#include <string_view>

#include "core/domain/attributes.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/actions/regular_amendment.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "ih/orders/matchers/order_matcher.hpp"

namespace simulator::trading_system::matching_engine {

namespace trade_at_last {

constexpr std::string_view LimitOrdersOnlyReject =
    "only limit orders are accepted during the trade-at-last phase";

constexpr std::string_view DayOrdersOnlyReject =
    "only day orders are accepted during the trade-at-last phase";

constexpr std::string_view ClosingPriceUnavailableReject =
    "no closing price is available, orders are not accepted during the "
    "trade-at-last phase";

constexpr std::string_view PriceNotAtClosingPriceReject =
    "order price must be equal to the closing price during the trade-at-last "
    "phase";

[[nodiscard]]
auto acceptance_error(TimeInForce time_in_force,
                      OrderPrice price,
                      std::optional<Price> closing_price)
    -> std::optional<std::string_view>;

// Market orders can neither rest nor cross during the phase, so they are
// rejected on arrival without reaching the book.
auto reject_market_order(EventListener& event_listener,
                         MarketOrder& order) -> void;

}  // namespace trade_at_last

class TradeAtLastPlacement : private EventReporter {
 public:
  TradeAtLastPlacement(EventListener& event_listener,
                       OrderBook& order_book,
                       RegularMatcher& matcher,
                       std::optional<Price> closing_price);

  TradeAtLastPlacement(const TradeAtLastPlacement&) = default;
  TradeAtLastPlacement(TradeAtLastPlacement&&) = default;
  ~TradeAtLastPlacement() override = default;

  auto operator=(const TradeAtLastPlacement&) -> TradeAtLastPlacement& = delete;
  auto operator=(TradeAtLastPlacement&&) -> TradeAtLastPlacement& = delete;

  auto operator()(LimitOrder order) -> OrderBookUpdates;

 private:
  auto reject(LimitOrder& order, std::string_view reason) -> void;

  OrderBook& order_book_;
  RegularMatcher& matcher_;
  std::optional<Price> closing_price_;
};

// Applies the trade-at-last acceptance rules to the amended order values and
// hands an accepted amendment over to the trade-at-last queue.
class TradeAtLastAmendment : private EventReporter {
 public:
  TradeAtLastAmendment(EventListener& event_listener,
                       RegularAmendment& amendment,
                       std::optional<Price> closing_price);

  TradeAtLastAmendment(const TradeAtLastAmendment&) = default;
  TradeAtLastAmendment(TradeAtLastAmendment&&) = default;
  ~TradeAtLastAmendment() override = default;

  auto operator=(const TradeAtLastAmendment&) -> TradeAtLastAmendment& = delete;
  auto operator=(TradeAtLastAmendment&&) -> TradeAtLastAmendment& = delete;

  auto operator()(LimitUpdate update) -> OrderBookUpdates;

 private:
  RegularAmendment& amendment_;
  std::optional<Price> closing_price_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_TRADE_AT_LAST_ACTIONS_HPP_
