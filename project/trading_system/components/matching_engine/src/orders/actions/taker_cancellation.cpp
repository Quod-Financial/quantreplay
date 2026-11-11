#include "ih/orders/actions/taker_cancellation.hpp"

#include <optional>

#include "ih/common/events/client_notification.hpp"
#include "ih/common/events/event.hpp"
#include "ih/orders/replies/cancellation_reply_builders.hpp"

namespace simulator::trading_system::matching_engine {
namespace {

template <typename Order>
auto cancel_taker(EventListener& event_listener,
                  Order& order,
                  std::optional<PriceTick> price_tick) -> void {
  order.cancel();
  event_listener.on(Event(ClientNotification(
      prepare_cancellation_confirmation(order, price_tick)
          .with_leaving_quantity(LeavesQuantity{0})
          .with_execution_id(order.make_execution_id())
          .with_client_order_id(order.client_order_id())
          .with_cancellation_text(CancellationText{"no facing orders found"})
          .build())));
}

}  // namespace

auto cancel_without_facing_orders(EventListener& event_listener,
                                  LimitOrder& order,
                                  std::optional<PriceTick> price_tick) -> void {
  cancel_taker(event_listener, order, price_tick);
}

auto cancel_without_facing_orders(EventListener& event_listener,
                                  MarketOrder& order,
                                  std::optional<PriceTick> price_tick) -> void {
  cancel_taker(event_listener, order, price_tick);
}

}  // namespace simulator::trading_system::matching_engine
