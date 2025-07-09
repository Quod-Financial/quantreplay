#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TRADE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TRADE_HPP_

#include "common/attributes.hpp"
#include "core/domain/market_phase.hpp"
#include "core/tools/time.hpp"

namespace simulator::trading_system {

struct Trade {
  std::optional<BuyerId> buyer;
  std::optional<SellerId> seller;
  Price trade_price;
  Quantity traded_quantity;
  std::optional<AggressorSide> aggressor_side;
  core::sys_us trade_time;
  MarketPhase market_phase;

  [[nodiscard]]
  auto operator==(const Trade&) const -> bool = default;
};

}  // namespace simulator::trading_system

template <>
struct fmt::formatter<simulator::trading_system::Trade> {
  using formattable = simulator::trading_system::Trade;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const formattable& trade, FormatContext& ctx) const {
    return fmt::format_to(
        ctx.out(),
        R"({{ "buyer": {}, "seller": {}, "trade_price": {}, "traded_quantity": {}, "aggressor_side": "{}", "trade_time": "{}", "market_phase": {} }})",
        trade.buyer,
        trade.seller,
        trade.trade_price,
        trade.traded_quantity,
        trade.aggressor_side,
        trade.trade_time,
        trade.market_phase);
  }
};

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TRADE_HPP_
