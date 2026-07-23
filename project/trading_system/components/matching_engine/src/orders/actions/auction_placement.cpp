#include "ih/orders/actions/auction_placement.hpp"

#include "ih/common/events/client_notification.hpp"
#include "ih/orders/replies/placement_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

AuctionPlacement::AuctionPlacement(EventListener& event_listener,
                                   OrderBook& order_book)
    : EventReporter(event_listener), order_book_(order_book) {}

auto AuctionPlacement::operator()(LimitOrder order) -> void {
  log::debug("running auction limit order placement operation");

  if (order.time_in_force() == TimeInForce::Option::ImmediateOrCancel ||
      order.time_in_force() == TimeInForce::Option::FillOrKill) {
    emit(ClientNotification(
        prepare_placement_reject(order)
            .with_execution_id(order.make_execution_id())
            .with_reason(RejectText{
                "immediate-or-cancel and fill-or-kill orders are not accepted "
                "during an auction"})
            .build()));
    return;
  }

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));

  order_book_.take_page(order.side()).limit_orders().emplace(order);
  emit(order::make_making_order_added_to_book_notification(order));
}

auto AuctionPlacement::operator()(MarketOrder order) -> void {
  log::debug("running auction market order placement operation");

  emit(ClientNotification(prepare_placement_confirmation(order)
                              .with_execution_id(order.make_execution_id())
                              .build()));

  order_book_.take_page(order.side()).market_orders().emplace(order);
  emit(order::make_making_order_added_to_book_notification(order));
}

}  // namespace simulator::trading_system::matching_engine
