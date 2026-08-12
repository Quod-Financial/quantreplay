#include "ih/orders/actions/trade_at_last_actions.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "ih/common/events/client_notification.hpp"
#include "ih/common/events/event.hpp"
#include "ih/orders/replies/modification_reply_builders.hpp"
#include "ih/orders/replies/placement_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

namespace trade_at_last {

auto acceptance_error(TimeInForce time_in_force,
                      OrderPrice price,
                      std::optional<Price> closing_price)
    -> std::optional<std::string_view> {
  if (time_in_force != TimeInForce::Option::Day) {
    return DayOrdersOnlyReject;
  }
  if (!closing_price.has_value()) {
    return ClosingPriceUnavailableReject;
  }
  if (static_cast<double>(price) != static_cast<double>(*closing_price)) {
    return PriceNotAtClosingPriceReject;
  }
  return std::nullopt;
}

auto reject_market_order(EventListener& event_listener,
                         MarketOrder& order) -> void {
  event_listener.on(Event(ClientNotification(
      prepare_placement_reject(order)
          .with_execution_id(order.make_execution_id())
          .with_reason(RejectText{std::string{LimitOrdersOnlyReject}})
          .build())));
}

}  // namespace trade_at_last

TradeAtLastPlacement::TradeAtLastPlacement(EventListener& event_listener,
                                           OrderBook& order_book,
                                           RegularMatcher& matcher,
                                           std::optional<Price> closing_price)
    : EventReporter(event_listener),
      order_book_(order_book),
      matcher_(matcher),
      closing_price_(closing_price) {}

auto TradeAtLastPlacement::operator()(LimitOrder order) -> OrderBookUpdates {
  log::debug("running trade-at-last limit order placement operation");

  if (const auto error = trade_at_last::acceptance_error(
          order.time_in_force(), order.price(), closing_price_)) {
    reject(order, *error);
    return {};
  }

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));

  matcher_.match(order);

  if (order.executed()) {
    return {};
  }

  order_book_.take_page(order.side()).trade_at_last_orders().emplace(order);
  emit(order::make_making_order_added_to_book_notification(order));

  return {OrderBookUpdate{.side = order.side(),
                          .action = OrderBookUpdate::Action::Add,
                          .price = order.price(),
                          .quantity = order.leaves_quantity()}};
}

auto TradeAtLastPlacement::reject(LimitOrder& order,
                                  std::string_view reason) -> void {
  emit(ClientNotification(prepare_placement_reject(order)
                              .with_execution_id(order.make_execution_id())
                              .with_reason(RejectText{std::string{reason}})
                              .build()));
}

TradeAtLastAmendment::TradeAtLastAmendment(EventListener& event_listener,
                                           RegularAmendment& amendment,
                                           std::optional<Price> closing_price)
    : EventReporter(event_listener),
      amendment_(amendment),
      closing_price_(closing_price) {}

auto TradeAtLastAmendment::operator()(LimitUpdate update) -> OrderBookUpdates {
  log::debug("running trade-at-last limit order amendment operation");

  if (const auto error = trade_at_last::acceptance_error(
          update.order_diff.attributes.time_in_force(),
          update.order_diff.price,
          closing_price_)) {
    emit(ClientNotification(prepare_modification_reject(update)
                                .with_reason(RejectText{std::string{*error}})
                                .build()));
    return {};
  }

  return amendment_(std::move(update));
}

}  // namespace simulator::trading_system::matching_engine
