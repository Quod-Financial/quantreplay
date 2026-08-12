#include "ih/orders/actions/market_amendment.hpp"

#include "ih/common/events/client_notification.hpp"
#include "ih/orders/replies/modification_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "ih/orders/tools/order_lookup.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

MarketAmendment::MarketAmendment(EventListener& event_listener,
                                 OrderBook& order_book,
                                 std::optional<PriceTick> price_tick)
    : EventReporter{event_listener},
      order_book_{order_book},
      price_tick_{price_tick} {}

auto MarketAmendment::operator()(MarketUpdate update) -> OrderBookUpdates {
  log::debug("running market order amendment operation");

  const Side side = update.order_side;
  return amend_order(std::move(update), order_book_.take_page(side));
}

auto MarketAmendment::amend_order(MarketUpdate update, OrderPage& page)
    -> OrderBookUpdates {
  auto& orders = page.market_orders();
  const auto order_it = find_target_market_order(orders, update);
  if (order_it == orders.end()) {
    emit(ClientNotification(prepare_modification_reject(update)
                                .with_reason(RejectText{"order not found"})
                                .build()));
    return {};
  }

  if (static_cast<double>(update.order_diff.quantity) <=
      static_cast<double>(order_it->cum_executed_quantity())) {
    emit(ClientNotification(prepare_modification_reject(update)
                                .with_order_status(order_it->status())
                                .with_reason(RejectText{"invalid quantity"})
                                .build()));
    return {};
  }

  if (order_it->time_in_force() !=
      update.order_diff.attributes.time_in_force()) {
    emit(ClientNotification(
        prepare_modification_reject(update)
            .with_order_status(order_it->status())
            .with_reason(RejectText{"time in force can not be changed"})
            .build()));
    return {};
  }

  const OrderBookUpdate removal{.side = order_it->side(),
                                .action = OrderBookUpdate::Action::Remove,
                                .price = std::nullopt,
                                .quantity = order_it->leaves_quantity()};

  MarketOrder order = *order_it;
  orders.erase(order_it);
  emit(order::make_making_order_removed_from_book_notification(order));

  order.amend(std::move(update.order_diff));
  emit(ClientNotification(
      prepare_modification_confirmation(order, price_tick_)
          .with_execution_id(order.make_execution_id())
          .with_orig_client_order_id(update.orig_client_order_id)
          .build()));

  orders.emplace(order);
  emit(order::make_making_order_added_to_book_notification(order));

  return {removal,
          OrderBookUpdate{.side = order.side(),
                          .action = OrderBookUpdate::Action::Add,
                          .price = std::nullopt,
                          .quantity = order.leaves_quantity()}};
}

}  // namespace simulator::trading_system::matching_engine
