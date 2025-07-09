#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_ORDER_TEST_TOOLS_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_ORDER_TEST_TOOLS_HPP_

#include <optional>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_metadata.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine {

// NOLINTBEGIN(*-magic-numbers)

class OrderBuilder {
 public:
  auto build_order_attributes() const -> OrderAttributes;

  auto build_order_record() const -> OrderRecord;

  auto build_limit_order() const -> LimitOrder;

  auto build_market_order() const -> MarketOrder;

  auto with_order_id(OrderId identifier) -> OrderBuilder &;

  auto with_side(Side side) -> OrderBuilder &;

  auto with_time_in_force(TimeInForce time_in_force) -> OrderBuilder &;

  auto with_short_sell_exemption_reason(ShortSaleExemptionReason reason)
      -> OrderBuilder &;

  auto with_client_session(protocol::Session session) -> OrderBuilder &;

  auto with_instrument(InstrumentDescriptor instrument) -> OrderBuilder &;

  auto with_order_parties(std::vector<Party> parties) -> OrderBuilder &;

  auto with_client_order_id(ClientOrderId client_order_id) -> OrderBuilder &;

  auto with_order_price(OrderPrice price) -> OrderBuilder &;

  auto with_order_quantity(OrderQuantity quantity) -> OrderBuilder &;

  auto with_expire_time(ExpireTime time) -> OrderBuilder &;

  auto with_expire_date(ExpireDate date) -> OrderBuilder &;

  auto with_order_time(OrderTime time) -> OrderBuilder &;

 private:
  InstrumentDescriptor instrument_;
  protocol::Session session_{protocol::generator::Session{}};
  std::optional<ClientOrderId> client_order_id_;
  std::vector<Party> parties_;
  std::optional<OrderTime> order_time_;
  std::optional<ExpireTime> expire_time_;
  std::optional<ExpireDate> expire_date_;
  std::optional<ShortSaleExemptionReason> short_sale_exemption_reason_;
  OrderPrice price_{42};
  OrderQuantity quantity_{420};
  OrderId order_id_{4221};
  Side side_{Side::Option::Buy};
  TimeInForce time_in_force_{TimeInForce::Option::Day};
};

// NOLINTEND(*-magic-numbers)

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_ORDER_TEST_TOOLS_HPP_