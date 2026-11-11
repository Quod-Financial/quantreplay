#include "ih/orders/actions/auction_placement.hpp"

#include <optional>
#include <string>
#include <string_view>

#include "ih/common/events/client_notification.hpp"
#include "ih/common/events/event.hpp"
#include "ih/orders/replies/placement_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

namespace auction {

auto acceptance_error(TimeInForce time_in_force)
    -> std::optional<std::string_view> {
  if (time_in_force == TimeInForce::Option::ImmediateOrCancel ||
      time_in_force == TimeInForce::Option::FillOrKill) {
    return ImmediateOrdersNotAcceptedReject;
  }
  return std::nullopt;
}

auto rest_in_book(EventListener& event_listener,
                  OrderBook& order_book,
                  const LimitOrder& order) -> OrderBookUpdate {
  order_book.take_page(order.side()).limit_orders().emplace(order);
  event_listener.on(
      Event(order::make_making_order_added_to_book_notification(order)));

  return OrderBookUpdate{.side = order.side(),
                         .action = OrderBookUpdate::Action::Add,
                         .price = order.price(),
                         .quantity = order.leaves_quantity()};
}

auto rest_in_book(EventListener& event_listener,
                  OrderBook& order_book,
                  const MarketOrder& order) -> OrderBookUpdate {
  order_book.take_page(order.side()).market_orders().emplace(order);
  event_listener.on(
      Event(order::make_making_order_added_to_book_notification(order)));

  return OrderBookUpdate{.side = order.side(),
                         .action = OrderBookUpdate::Action::Add,
                         .price = std::nullopt,
                         .quantity = order.leaves_quantity()};
}

}  // namespace auction

AuctionPlacement::AuctionPlacement(EventListener& event_listener,
                                   OrderBook& order_book)
    : EventReporter(event_listener), order_book_(&order_book) {}

auto AuctionPlacement::operator()(LimitOrder order) -> OrderBookUpdates {
  log::debug("running auction limit order placement operation");

  if (const auto error = auction::acceptance_error(order.time_in_force())) {
    emit(ClientNotification(prepare_placement_reject(order)
                                .with_execution_id(order.make_execution_id())
                                .with_reason(RejectText{std::string{*error}})
                                .build()));
    return {};
  }

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));

  return {auction::rest_in_book(listener(), *order_book_, order)};
}

auto AuctionPlacement::operator()(MarketOrder order) -> OrderBookUpdates {
  log::debug("running auction market order placement operation");

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));

  return {auction::rest_in_book(listener(), *order_book_, order)};
}

}  // namespace simulator::trading_system::matching_engine
