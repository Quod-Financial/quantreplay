#include <utility>

#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
#include "protocol/types/session.hpp"
#include "tools/order_test_tools.hpp"

namespace simulator::trading_system::matching_engine {

// NOLINTBEGIN(*magic-numbers*)

auto OrderBuilder::build_order_attributes() const -> OrderAttributes {
  OrderAttributes attributes;
  attributes.set_order_parties(parties_);
  attributes.set_time_in_force(time_in_force_);
  if (client_order_id_) {
    attributes.set_client_order_id(*client_order_id_);
  }
  if (short_sale_exemption_reason_) {
    attributes.set_short_sale_exemption_reason(*short_sale_exemption_reason_);
  }
  if (expire_time_) {
    attributes.set_expire_time(*expire_time_);
  }
  if (expire_date_) {
    attributes.set_expire_date(*expire_date_);
  }
  return attributes;
}

auto OrderBuilder::build_order_record() const -> OrderRecord {
  OrderRecord record{
      order_id_, side_, session_, instrument_, build_order_attributes()};
  if (order_time_.has_value()) {
    record.set_order_time(*order_time_);
  }

  return record;
}

auto OrderBuilder::build_limit_order() const -> LimitOrder {
  return LimitOrder{price_, quantity_, build_order_record()};
}

auto OrderBuilder::build_market_order() const -> MarketOrder {
  return MarketOrder{quantity_, build_order_record()};
}

auto OrderBuilder::with_order_id(OrderId identifier) -> OrderBuilder& {
  order_id_ = identifier;
  return *this;
}

auto OrderBuilder::with_side(Side side) -> OrderBuilder& {
  side_ = side;
  return *this;
}

auto OrderBuilder::with_time_in_force(TimeInForce time_in_force)
    -> OrderBuilder& {
  time_in_force_ = time_in_force;
  return *this;
}

auto OrderBuilder::with_short_sell_exemption_reason(
    ShortSaleExemptionReason reason) -> OrderBuilder& {
  short_sale_exemption_reason_ = reason;
  return *this;
}

auto OrderBuilder::with_client_session(protocol::Session session)
    -> OrderBuilder& {
  std::swap(session_, session);
  return *this;
}

auto OrderBuilder::with_instrument(InstrumentDescriptor instrument)
    -> OrderBuilder& {
  std::swap(instrument_, instrument);
  return *this;
}

auto OrderBuilder::with_order_parties(std::vector<Party> parties)
    -> OrderBuilder& {
  std::swap(parties_, parties);
  return *this;
}

auto OrderBuilder::with_client_order_id(ClientOrderId client_order_id)
    -> OrderBuilder& {
  client_order_id_ = std::move(client_order_id);
  return *this;
}

auto OrderBuilder::with_order_price(OrderPrice price) -> OrderBuilder& {
  price_ = price;
  return *this;
}

auto OrderBuilder::with_order_quantity(OrderQuantity quantity)
    -> OrderBuilder& {
  quantity_ = quantity;
  return *this;
}

auto OrderBuilder::with_expire_time(ExpireTime time) -> OrderBuilder& {
  expire_time_ = time;
  return *this;
}

auto OrderBuilder::with_expire_date(ExpireDate date) -> OrderBuilder& {
  expire_date_ = date;
  return *this;
}

auto OrderBuilder::with_order_time(OrderTime time) -> OrderBuilder& {
  order_time_ = time;
  return *this;
}

// NOLINTEND(*magic-numbers*)

}  // namespace simulator::trading_system::matching_engine