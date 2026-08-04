#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_DATA_MARKET_DATA_UPDATES_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_DATA_MARKET_DATA_UPDATES_HPP_

#include <fmt/base.h>

#include <optional>

#include "common/attributes.hpp"
#include "common/instrument_state.hpp"
#include "common/trade.hpp"
#include "core/domain/attributes.hpp"

namespace simulator::trading_system::matching_engine {

struct TradeResult {
  Price price;
  Quantity quantity;

  [[nodiscard]]
  auto operator==(const TradeResult&) const -> bool = default;
};

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

struct AuctionFinalPriceUpdate {
  // Default-constructed value is a no-op; the producer always sets the phase.
  TradingPhase auction_phase{TradingPhase::Option::Open};
  std::optional<TradeResult> clearing_value;

  [[nodiscard]]
  auto operator==(const AuctionFinalPriceUpdate&) const -> bool = default;
};

struct EarlyPriceUpdate {
  std::optional<TradeResult> early_value;

  [[nodiscard]]
  auto operator==(const EarlyPriceUpdate&) const -> bool = default;
};

struct AuctionIndicativeUpdate {
  struct IndicativePriceQuantity {
    std::optional<Price> price;
    Quantity quantity;

    [[nodiscard]]
    auto operator==(const IndicativePriceQuantity&) const -> bool = default;
  };

  struct Imbalance {
    Quantity size;
    TradeCondition side;

    [[nodiscard]]
    auto operator==(const Imbalance&) const -> bool = default;
  };

  TradingPhase auction_phase{TradingPhase::Option::Open};
  std::optional<IndicativePriceQuantity> price_qty;
  std::optional<Imbalance> imbalance;

  [[nodiscard]]
  auto operator==(const AuctionIndicativeUpdate&) const -> bool = default;
};

struct TzDayPassed {
  core::sys_us sys_tick_time;
};

}  // namespace simulator::trading_system::matching_engine

template <>
struct fmt::formatter<simulator::trading_system::matching_engine::TradeResult>
    : formatter<std::string_view> {
  using formattable = simulator::trading_system::matching_engine::TradeResult;

  auto format(const formattable& value, format_context& ctx) const
      -> format_context::iterator {
    return format_to(
        ctx.out(),
        R"({{ "TradeResult": {{ "price": {}, "quantity": {} }} }})",
        value.price,
        value.quantity);
  }
};

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
    simulator::trading_system::matching_engine::AuctionFinalPriceUpdate>
    : formatter<std::string_view> {
  using formattable =
      simulator::trading_system::matching_engine::AuctionFinalPriceUpdate;

  auto format(const formattable& event, format_context& ctx) const
      -> format_context::iterator {
    return format_to(
        ctx.out(),
        R"({{ "AuctionFinalPriceUpdate": {{ "auction_phase": "{}", "clearing_value": {} }} }})",
        event.auction_phase,
        event.clearing_value);
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
    return format_to(ctx.out(),
                     R"({{ "EarlyPriceUpdate": {{ "early_value": {} }} }})",
                     event.early_value);
  }
};

template <>
struct fmt::formatter<simulator::trading_system::matching_engine::
                          AuctionIndicativeUpdate::IndicativePriceQuantity>
    : formatter<std::string_view> {
  using formattable = simulator::trading_system::matching_engine::
      AuctionIndicativeUpdate::IndicativePriceQuantity;

  auto format(const formattable& value, format_context& ctx) const
      -> format_context::iterator {
    return format_to(
        ctx.out(),
        R"({{ "IndicativePriceQuantity": {{ "price": {}, "quantity": {} }} }})",
        value.price,
        value.quantity);
  }
};

template <>
struct fmt::formatter<simulator::trading_system::matching_engine::
                          AuctionIndicativeUpdate::Imbalance>
    : formatter<std::string_view> {
  using formattable = simulator::trading_system::matching_engine::
      AuctionIndicativeUpdate::Imbalance;

  auto format(const formattable& value, format_context& ctx) const
      -> format_context::iterator {
    return format_to(ctx.out(),
                     R"({{ "Imbalance": {{ "size": {}, "side": "{}" }} }})",
                     value.size,
                     value.side);
  }
};

template <>
struct fmt::formatter<
    simulator::trading_system::matching_engine::AuctionIndicativeUpdate>
    : formatter<std::string_view> {
  using formattable =
      simulator::trading_system::matching_engine::AuctionIndicativeUpdate;

  auto format(const formattable& event, format_context& ctx) const
      -> format_context::iterator {
    return format_to(
        ctx.out(),
        R"({{ "AuctionIndicativeUpdate": {{ "auction_phase": "{}", "price_qty": {}, "imbalance": {} }} }})",
        event.auction_phase,
        event.price_qty,
        event.imbalance);
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
