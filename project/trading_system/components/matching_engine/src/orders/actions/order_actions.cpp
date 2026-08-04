#include "ih/orders/actions/order_actions.hpp"

#include "ih/orders/actions/auction_placement.hpp"
#include "ih/orders/actions/cancellation.hpp"
#include "ih/orders/actions/limit_order_recover.hpp"
#include "ih/orders/actions/market_amendment.hpp"
#include "ih/orders/actions/regular_amendment.hpp"
#include "ih/orders/actions/regular_placement.hpp"
#include "ih/orders/matchers/no_cross_matcher.hpp"
#include "ih/orders/matchers/regular_order_matcher.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

auto place_limit_order(EventListener& event_listener,
                       OrderBook& order_book,
                       std::optional<PriceTick> price_tick,
                       LimitOrder order,
                       OrderActionMode mode) -> OrderBookUpdates {
  if (mode == OrderActionMode::AuctionCall) {
    AuctionPlacement operation{event_listener, order_book};
    log::debug("executing auction limit order placement operation");
    return operation(std::move(order));
  }

  RegularOrderMatcher matcher{event_listener, order_book, price_tick};
  RegularPlacement operation{event_listener, order_book, matcher};
  log::debug("executing limit order placement operation");
  return operation(std::move(order));
}

auto place_market_order(EventListener& event_listener,
                        OrderBook& order_book,
                        std::optional<PriceTick> price_tick,
                        MarketOrder order,
                        OrderActionMode mode) -> OrderBookUpdates {
  if (mode == OrderActionMode::AuctionCall) {
    AuctionPlacement operation{event_listener, order_book};
    log::debug("executing auction market order placement operation");
    return operation(std::move(order));
  }

  RegularOrderMatcher matcher{event_listener, order_book, price_tick};
  RegularPlacement operation{event_listener, order_book, matcher};
  log::debug("executing market order placement operation");
  return operation(std::move(order));
}

auto amend_limit_order(EventListener& event_listener,
                       OrderBook& order_book,
                       std::optional<PriceTick> price_tick,
                       LimitUpdate update,
                       OrderActionMode mode) -> OrderBookUpdates {
  if (mode == OrderActionMode::AuctionCall) {
    NoCrossMatcher matcher;
    RegularAmendment operation{event_listener, order_book, matcher, price_tick};
    log::debug("executing auction limit order amendment action");
    return operation(std::move(update));
  }

  RegularOrderMatcher matcher{event_listener, order_book, price_tick};
  RegularAmendment operation{event_listener, order_book, matcher, price_tick};
  log::debug("executing limit order amendment action");
  return operation(std::move(update));
}

auto amend_market_order(EventListener& event_listener,
                        OrderBook& order_book,
                        std::optional<PriceTick> price_tick,
                        MarketUpdate update) -> OrderBookUpdates {
  MarketAmendment operation{event_listener, order_book, price_tick};
  log::debug("executing market order amendment action");
  return operation(std::move(update));
}

auto cancel_order(EventListener& event_listener,
                  OrderBook& order_book,
                  std::optional<PriceTick> price_tick,
                  const OrderCancel& cancel) -> OrderBookUpdates {
  Cancellation operation{event_listener, order_book, price_tick};
  log::debug("executing order cancellation action");
  return operation(cancel);
}

auto recover_order(EventListener& event_listener,
                   OrderBook& order_book,
                   market_state::LimitOrder order_state) -> void {
  LimitOrderRecover operation{event_listener, order_book};
  log::debug("executing order recovering");
  operation(std::move(order_state));
}

}  // namespace simulator::trading_system::matching_engine
