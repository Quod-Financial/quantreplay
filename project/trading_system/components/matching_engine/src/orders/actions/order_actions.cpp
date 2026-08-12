#include "ih/orders/actions/order_actions.hpp"

#include "ih/orders/actions/auction_placement.hpp"
#include "ih/orders/actions/cancellation.hpp"
#include "ih/orders/actions/limit_order_recover.hpp"
#include "ih/orders/actions/market_amendment.hpp"
#include "ih/orders/actions/regular_amendment.hpp"
#include "ih/orders/actions/regular_placement.hpp"
#include "ih/orders/actions/trade_at_last_actions.hpp"
#include "ih/orders/matchers/no_cross_matcher.hpp"
#include "ih/orders/matchers/regular_order_matcher.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

auto place_limit_order(EventListener& event_listener,
                       OrderBook& order_book,
                       std::optional<PriceTick> price_tick,
                       LimitOrder order,
                       const OrderActionContext& context) -> OrderBookUpdates {
  if (context.mode == OrderActionMode::AuctionCall) {
    AuctionPlacement operation{event_listener, order_book};
    log::debug("executing auction limit order placement operation");
    return operation(std::move(order));
  }

  if (context.mode == OrderActionMode::TradeAtLast) {
    RegularOrderMatcher matcher{event_listener,
                                order_book,
                                price_tick,
                                context.market_phase,
                                LimitOrderQueue::TradeAtLast};
    TradeAtLastPlacement operation{
        event_listener, order_book, matcher, context.closing_price};
    log::debug("executing trade-at-last limit order placement operation");
    return operation(std::move(order));
  }

  RegularOrderMatcher matcher{event_listener,
                              order_book,
                              price_tick,
                              context.market_phase,
                              LimitOrderQueue::Regular};
  RegularPlacement operation{event_listener, order_book, matcher};
  log::debug("executing limit order placement operation");
  return operation(std::move(order));
}

auto place_market_order(EventListener& event_listener,
                        OrderBook& order_book,
                        std::optional<PriceTick> price_tick,
                        MarketOrder order,
                        const OrderActionContext& context) -> OrderBookUpdates {
  if (context.mode == OrderActionMode::AuctionCall) {
    AuctionPlacement operation{event_listener, order_book};
    log::debug("executing auction market order placement operation");
    return operation(std::move(order));
  }

  if (context.mode == OrderActionMode::TradeAtLast) {
    log::debug("rejecting market order during the trade-at-last phase");
    trade_at_last::reject_market_order(event_listener, order);
    return {};
  }

  RegularOrderMatcher matcher{event_listener,
                              order_book,
                              price_tick,
                              context.market_phase,
                              LimitOrderQueue::Regular};
  RegularPlacement operation{event_listener, order_book, matcher};
  log::debug("executing market order placement operation");
  return operation(std::move(order));
}

auto amend_limit_order(EventListener& event_listener,
                       OrderBook& order_book,
                       std::optional<PriceTick> price_tick,
                       LimitUpdate update,
                       const OrderActionContext& context) -> OrderBookUpdates {
  if (context.mode == OrderActionMode::AuctionCall) {
    NoCrossMatcher matcher;
    RegularAmendment operation{event_listener,
                               order_book,
                               matcher,
                               price_tick,
                               LimitOrderQueue::Regular};
    log::debug("executing auction limit order amendment action");
    return operation(std::move(update));
  }

  if (context.mode == OrderActionMode::TradeAtLast) {
    // A trade-at-last order rests only after it has emptied the opposite
    // queue, so an amendment has nothing to cross with.
    NoCrossMatcher matcher;
    RegularAmendment amendment{event_listener,
                               order_book,
                               matcher,
                               price_tick,
                               LimitOrderQueue::TradeAtLast};
    TradeAtLastAmendment operation{
        event_listener, amendment, context.closing_price};
    log::debug("executing trade-at-last limit order amendment action");
    return operation(std::move(update));
  }

  RegularOrderMatcher matcher{event_listener,
                              order_book,
                              price_tick,
                              context.market_phase,
                              LimitOrderQueue::Regular};
  RegularAmendment operation{event_listener,
                             order_book,
                             matcher,
                             price_tick,
                             LimitOrderQueue::Regular};
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
                  const OrderCancel& cancel,
                  const OrderActionContext& context) -> OrderBookUpdates {
  const auto queue = context.mode == OrderActionMode::TradeAtLast
                         ? LimitOrderQueue::TradeAtLast
                         : LimitOrderQueue::Regular;
  Cancellation operation{event_listener, order_book, price_tick, queue};
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
