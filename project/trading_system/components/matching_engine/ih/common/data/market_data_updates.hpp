#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_DATA_MARKET_DATA_UPDATES_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_DATA_MARKET_DATA_UPDATES_HPP_

#include <fmt/base.h>

#include "common/attributes.hpp"
#include "common/market_state/snapshot.hpp"
#include "common/trade.hpp"
#include "core/domain/attributes.hpp"

namespace simulator::trading_system::matching_engine {

struct OrderAdded {
  std::optional<PartyId> order_owner;
  std::optional<Price> order_price;
  Quantity order_quantity;
  OrderId order_id;
  Side order_side;
};

struct OrderReduced {
  std::optional<Price> order_price;
  Quantity order_quantity;
  OrderId order_id;
  Side order_side;
};

struct OrderRemoved {
  std::optional<Price> order_price;
  OrderId order_id;
  Side order_side;
};

struct LastTradeRecover {
  std::optional<Trade> trade;
};

struct InstrumentInfoRecover {
  std::optional<market_state::InstrumentInfo> info;
};

}  // namespace simulator::trading_system::matching_engine

template <>
struct fmt::formatter<simulator::trading_system::matching_engine::OrderAdded>
    : formatter<std::string_view> {
  using formattable = simulator::trading_system::matching_engine::OrderAdded;

  auto format(const formattable& event, format_context& ctx) const
      -> format_context::iterator {
    return format_to(
        ctx.out(),
        R"({{ "OrderAdded": {{ "order_id": {}, "order_price": {}, "order_quantity": {}, "order_side": "{}", "order_owner": {} }} }})",
        event.order_id,
        event.order_price,
        event.order_quantity,
        event.order_side,
        event.order_owner);
  }
};

template <>
struct fmt::formatter<simulator::trading_system::matching_engine::OrderReduced>
    : formatter<std::string_view> {
  using formattable = simulator::trading_system::matching_engine::OrderReduced;

  auto format(const formattable& event, format_context& ctx) const
      -> format_context::iterator {
    return format_to(
        ctx.out(),
        R"({{ "OrderReduced": {{ "order_id": {}, "order_price": {}, "order_quantity": {}, "order_side": "{}" }} }})",
        event.order_id,
        event.order_price,
        event.order_quantity,
        event.order_side);
  }
};

template <>
struct fmt::formatter<simulator::trading_system::matching_engine::OrderRemoved>
    : formatter<std::string_view> {
  using formattable = simulator::trading_system::matching_engine::OrderRemoved;

  auto format(const formattable& event, format_context& ctx) const
      -> format_context::iterator {
    return format_to(
        ctx.out(),
        R"({{ "OrderRemoved": {{ "order_id": {}, "order_price": {}, "order_side": "{}" }} }})",
        event.order_id,
        event.order_price,
        event.order_side);
  }
};

template <>
struct fmt::formatter<
    simulator::trading_system::matching_engine::LastTradeRecover>
    : formatter<std::string_view> {
  using formattable =
      simulator::trading_system::matching_engine::LastTradeRecover;

  auto format(const formattable& event, format_context& ctx) const
      -> format_context::iterator {
    return format_to(ctx.out(),
                     R"({{ "LastTradeRecover": {{ "trade": {} }} }})",
                     event.trade);
  }
};

template <>
struct fmt::formatter<
    simulator::trading_system::matching_engine::InstrumentInfoRecover>
    : formatter<std::string_view> {
  using formattable =
      simulator::trading_system::matching_engine::InstrumentInfoRecover;

  auto format(const formattable& event, format_context& ctx) const
      -> format_context::iterator {
    return format_to(ctx.out(),
                     R"({{ "InstrumentInfoRecover": {{ "info": {} }} }})",
                     event.info);
  }
};

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_DATA_MARKET_DATA_UPDATES_HPP_
