#include "ih/orders/actions/limit_order_recover.hpp"

#include "core/tools/numeric.hpp"
#include "ih/orders/tools/notification_creators.hpp"

namespace simulator::trading_system::matching_engine {

namespace {

auto convert_attribute(market_state::LimitOrder& order_state)
    -> OrderAttributes {
  OrderAttributes attributes;
  if (auto&& value = order_state.client_order_id) {
    attributes.set_client_order_id(std::move(*value));
  }
  attributes.set_time_in_force(order_state.time_in_force);
  if (const auto value = order_state.expire_time) {
    attributes.set_expire_time(*value);
  }
  if (const auto value = order_state.expire_date) {
    attributes.set_expire_date(*value);
  }
  if (const auto value = order_state.short_sale_exemption_reason) {
    attributes.set_short_sale_exemption_reason(*value);
  }
  attributes.set_order_parties(std::move(order_state.order_parties));
  return attributes;
}

auto convert_session(market_state::Session&& session)
    -> protocol::Session {
  if (session.type == market_state::SessionType::Fix &&
      session.fix_session.has_value()) {
    return protocol::Session{std::move(session.fix_session.value())};
  }
  return protocol::Session{protocol::generator::Session{}};
}

auto convert_order(market_state::LimitOrder&& order_state) -> LimitOrder {
  OrderRecord record{order_state.order_id,
                     order_state.side,
                     convert_session(std::move(order_state.client_session)),
                     std::move(order_state.client_instrument_descriptor),
                     convert_attribute(order_state)};
  record.set_order_time(order_state.order_time);
  record.set_order_status(order_state.order_status);

  LimitOrder order{OrderPrice{order_state.order_price},
                   OrderQuantity{order_state.total_quantity},
                   std::move(record)};

  const auto executed_qty = order_state.cum_executed_quantity.value();
  if (!core::equal(executed_qty, 0.0)) {
    order.execute(ExecutedQuantity{executed_qty});
  }

  return order;
}

}  // namespace

LimitOrderRecover::LimitOrderRecover(EventListener& event_listener,
                                     OrderBook& order_book)
    : EventReporter{event_listener}, order_book_{order_book} {}

auto LimitOrderRecover::operator()(market_state::LimitOrder order_state)
    -> void {
  auto order = convert_order(std::move(order_state));
  const auto side = order.side();
  order_book_.take_page(side).limit_orders().emplace(std::move(order));

  emit(order::make_making_order_added_to_book_notification(order));
}

}  // namespace simulator::trading_system::matching_engine
