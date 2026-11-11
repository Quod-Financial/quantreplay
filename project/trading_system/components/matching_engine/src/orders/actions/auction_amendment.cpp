#include "ih/orders/actions/auction_amendment.hpp"

#include <optional>
#include <utility>

#include "ih/orders/actions/auction_placement.hpp"
#include "ih/orders/actions/market_amendment.hpp"
#include "ih/orders/actions/order_type_conversion.hpp"
#include "ih/orders/actions/regular_amendment.hpp"
#include "ih/orders/matchers/no_cross_matcher.hpp"
#include "ih/orders/tools/order_lookup.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {
namespace {

[[nodiscard]]
auto holds_target(LimitOrdersContainer& orders, const LimitUpdate& update)
    -> bool {
  return find_target_limit_order(orders, update) != orders.end();
}

[[nodiscard]]
auto holds_target(MarketOrdersContainer& orders, const MarketUpdate& update)
    -> bool {
  return find_target_market_order(orders, update) != orders.end();
}

}  // namespace

AuctionAmendment::AuctionAmendment(EventListener& event_listener,
                                   OrderBook& order_book,
                                   std::optional<PriceTick> price_tick)
    : EventReporter{event_listener},
      order_book_{&order_book},
      price_tick_{price_tick} {}

auto AuctionAmendment::operator()(LimitUpdate update) -> OrderBookUpdates {
  OrderPage& page = order_book_->take_page(update.order_side);
  if (!holds_target(page.limit_orders(), update)) {
    return rest_as_limit_order(std::move(update), page);
  }

  log::debug("running auction limit order amendment operation");
  NoCrossMatcher matcher;
  RegularAmendment amendment{listener(),
                             *order_book_,
                             matcher,
                             price_tick_,
                             LimitOrderQueue::Regular};
  return amendment(std::move(update));
}

auto AuctionAmendment::operator()(MarketUpdate update) -> OrderBookUpdates {
  OrderPage& page = order_book_->take_page(update.order_side);
  if (!holds_target(page.market_orders(), update)) {
    return rest_as_market_order(std::move(update), page);
  }

  log::debug("running auction market order amendment operation");
  MarketAmendment amendment{listener(), *order_book_, price_tick_};
  return amendment(std::move(update));
}

auto AuctionAmendment::rest_as_limit_order(LimitUpdate update, OrderPage& page)
    -> OrderBookUpdates {
  log::debug("running auction market to limit order amendment operation");

  OrderTypeConversion conversion{listener(), price_tick_};
  auto converted = conversion.to_auction_limit_order(std::move(update),
                                                     page.market_orders());
  if (!converted.has_value()) {
    return {};
  }

  return {
      converted->removal,
      auction::rest_in_book(listener(), *order_book_, converted->order)};
}

auto AuctionAmendment::rest_as_market_order(MarketUpdate update,
                                            OrderPage& page)
    -> OrderBookUpdates {
  log::debug("running auction limit to market order amendment operation");

  OrderTypeConversion conversion{listener(), price_tick_};
  auto converted =
      conversion.to_market_order(std::move(update), page.limit_orders());
  if (!converted.has_value()) {
    return {};
  }

  return {
      converted->removal,
      auction::rest_in_book(listener(), *order_book_, converted->order)};
}

}  // namespace simulator::trading_system::matching_engine
