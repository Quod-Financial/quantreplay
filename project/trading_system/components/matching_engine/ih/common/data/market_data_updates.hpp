#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_DATA_MARKET_DATA_UPDATES_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_DATA_MARKET_DATA_UPDATES_HPP_

#include <fmt/base.h>

#include <optional>

#include "common/attributes.hpp"
#include "common/instrument_state.hpp"
#include "common/trade.hpp"
#include "core/domain/attributes.hpp"

namespace simulator::trading_system::matching_engine {

struct OrderAdded {
  std::optional<PartyId> order_owner;
  std::optional<Price> order_price;
  Quantity order_quantity;
  OrderId order_id;
  Side order_side;
  OrderType order_type;
};

struct OrderReduced {
  std::optional<Price> order_price;
  Quantity order_quantity;
  OrderId order_id;
  Side order_side;

  [[nodiscard]]
  auto operator==(const OrderReduced& other) const -> bool = default;
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

struct AuctionPricesUpdate {
  // Default-constructed value is a no-op; the producer always sets the phase.
  TradingPhase auction_phase{TradingPhase::Option::Open};
  std::optional<Price> clearing_price;
  std::optional<Quantity> clearing_quantity;

  [[nodiscard]]
  auto operator==(const AuctionPricesUpdate&) const -> bool = default;
};

struct EarlyPriceUpdate {
  std::optional<Price> early_price;
  std::optional<Quantity> early_quantity;

  [[nodiscard]]
  auto operator==(const EarlyPriceUpdate&) const -> bool = default;
};

struct TzDayPassed {
  core::sys_us sys_tick_time;
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
        R"({{ "OrderAdded": {{ "order_id": {}, "order_price": {}, "order_quantity": {}, "order_side": "{}", "order_type": "{}", "order_owner": {} }} }})",
        event.order_id,
        event.order_price,
        event.order_quantity,
        event.order_side,
        event.order_type,
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

template <>
struct fmt::formatter<
    simulator::trading_system::matching_engine::AuctionPricesUpdate>
    : formatter<std::string_view> {
  using formattable =
      simulator::trading_system::matching_engine::AuctionPricesUpdate;

  auto format(const formattable& event, format_context& ctx) const
      -> format_context::iterator {
    return format_to(
        ctx.out(),
        R"({{ "AuctionPricesUpdate": {{ "auction_phase": "{}", "clearing_price": {}, "clearing_quantity": {} }} }})",
        event.auction_phase,
        event.clearing_price,
        event.clearing_quantity);
  }
};

template <>
struct fmt::formatter<
    simulator::trading_system::matching_engine::EarlyPriceUpdate>
    : formatter<std::string_view> {
  using formattable =
      simulator::trading_system::matching_engine::EarlyPriceUpdate;

  auto format(const formattable& event, format_context& ctx) const
      -> format_context::iterator {
    return format_to(
        ctx.out(),
        R"({{ "EarlyPriceUpdate": {{ "early_price": {}, "early_quantity": {} }} }})",
        event.early_price,
        event.early_quantity);
  }
};

template <>
struct fmt::formatter<simulator::trading_system::matching_engine::TzDayPassed>
    : formatter<std::string_view> {
  using formattable = simulator::trading_system::matching_engine::TzDayPassed;

  auto format(const formattable& event, format_context& ctx) const
      -> format_context::iterator {
    return format_to(ctx.out(),
                     R"({{ "TzDayPassed": {{ "sys_tick_time": "{}" }} }})",
                     event.sys_tick_time);
  }
};

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_DATA_MARKET_DATA_UPDATES_HPP_
