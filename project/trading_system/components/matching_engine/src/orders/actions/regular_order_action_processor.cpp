#include "ih/orders/actions/regular_order_action_processor.hpp"

#include <functional>

#include "ih/orders/actions/cancellation.hpp"
#include "ih/orders/actions/limit_order_recover.hpp"
#include "ih/orders/actions/regular_amendment.hpp"
#include "ih/orders/actions/regular_placement.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/matchers/regular_order_matcher.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

RegularOrderActionProcessor::RegularOrderActionProcessor(
    EventListener& event_listener, OrderBook& order_book)
    : event_listener_(event_listener), order_book_(order_book) {}

auto RegularOrderActionProcessor::place_limit_order(LimitOrder order) -> void {
  RegularOrderMatcher matcher(event_listener_, order_book_);
  RegularPlacement operation(event_listener_, order_book_, matcher);

  log::debug(
      "regular order action processor is executing limit order placement "
      "operation");

  std::invoke(operation, std::move(order));
}

auto RegularOrderActionProcessor::place_market_order(MarketOrder order)
    -> void {
  RegularOrderMatcher matcher(event_listener_, order_book_);
  RegularPlacement operation(event_listener_, order_book_, matcher);

  log::debug(
      "regular order action processor is executing market order placement "
      "operation");

  std::invoke(operation, std::move(order));
}

auto RegularOrderActionProcessor::amend_limit_order(LimitUpdate update)
    -> void {
  RegularOrderMatcher matcher(event_listener_, order_book_);
  RegularAmendment operation(event_listener_, order_book_, matcher);

  log::debug(
      "regular order action processor is executing limit order amendment "
      "action");

  std::invoke(operation, std::move(update));
}

auto RegularOrderActionProcessor::cancel_order(const OrderCancel& cancel)
    -> void {
  Cancellation operation(event_listener_, order_book_);

  log::debug(
      "regular order action processor is executing order cancellation action");

  std::invoke(operation, cancel);
}

auto RegularOrderActionProcessor::recover_order(
    market_state::LimitOrder order_state) -> void {
  LimitOrderRecover operation{event_listener_, order_book_};

  log::debug("regular order action processor is executing order recovering");

  std::invoke(operation, std::move(order_state));
}

}  // namespace simulator::trading_system::matching_engine