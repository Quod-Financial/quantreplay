#include "ih/orders/actions/limit_to_market_amendment.hpp"

#include <utility>

#include "ih/orders/actions/order_type_conversion.hpp"
#include "ih/orders/actions/taker_cancellation.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

LimitToMarketAmendment::LimitToMarketAmendment(
    EventListener& event_listener,
    OrderBook& order_book,
    RegularMatcher& matcher,
    std::optional<PriceTick> price_tick)
    : EventReporter{event_listener},
      order_book_{&order_book},
      matcher_{&matcher},
      price_tick_{price_tick} {}

auto LimitToMarketAmendment::operator()(MarketUpdate update)
    -> OrderBookUpdates {
  log::debug("running limit to market order amendment operation");

  auto& orders = order_book_->take_page(update.order_side).limit_orders();
  OrderTypeConversion conversion{listener(), price_tick_};
  auto converted = conversion.to_market_order(std::move(update), orders);
  if (!converted.has_value()) {
    return {};
  }

  execute(converted->order);
  return {converted->removal};
}

auto LimitToMarketAmendment::execute(MarketOrder& order) -> void {
  if (!matcher_->has_facing_orders(order)) {
    cancel_without_facing_orders(listener(), order, price_tick_);
    return;
  }
  matcher_->match(order);
}

}  // namespace simulator::trading_system::matching_engine
