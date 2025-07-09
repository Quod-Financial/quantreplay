#include "ih/orders/tools/order_book_state_converter.hpp"

#include "core/tools/overload.hpp"
#include "ih/orders/book/order_metadata.hpp"

namespace simulator::trading_system::matching_engine {

namespace {

auto store(const OrderAttributes& attributes,
           market_state::LimitOrder& order_state) {
  order_state.client_order_id = attributes.client_order_id();
  order_state.order_parties = attributes.order_parties();
  order_state.expire_time = attributes.expire_time();
  order_state.expire_date = attributes.expire_date();
  order_state.short_sale_exemption_reason =
      attributes.short_sale_exemption_reason();
  order_state.time_in_force = attributes.time_in_force();
}

auto store(const LimitOrder& order, market_state::LimitOrder& order_state) {
  store(order.attributes(), order_state);
  order_state.client_instrument_descriptor = order.instrument();
  order_state.order_id = order.id();
  order_state.order_time = order.time();
  order_state.side = order.side();
  order_state.order_status = order.status();
  const auto dispatcher = core::overload(
      [&](const protocol::fix::Session& session) {
        order_state.client_session = {market_state::SessionType::Fix, session};
      },
      [&](const protocol::generator::Session&) {
        order_state.client_session.type = market_state::SessionType::Generator;
      });
  std::visit(dispatcher, order.client_session().value);

  order_state.order_price = order.price();
  order_state.total_quantity = order.total_quantity();
  order_state.cum_executed_quantity = order.cum_executed_quantity();
}

auto store(OrderPage& page,
           std::vector<market_state::LimitOrder>& orders_state) {
  for (const auto& orders : page.limit_orders()) {
    market_state::LimitOrder order_state;
    store(orders, order_state);
    orders_state.push_back(std::move(order_state));
  }
}

}  // namespace

auto store_order_book_state(OrderBook& order_book,
                            market_state::OrderBook& state) -> void {
  store(order_book.buy_page(), state.buy_orders);
  store(order_book.sell_page(), state.sell_orders);
}

}  // namespace simulator::trading_system::matching_engine
