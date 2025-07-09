#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_INSTRUMENT_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_INSTRUMENT_HPP_

#include <fmt/base.h>

#include <optional>

#include "common/attributes.hpp"

namespace simulator::trading_system {

struct Instrument {
  std::optional<Symbol> symbol;
  std::optional<PriceCurrency> price_currency;
  std::optional<BaseCurrency> base_currency;
  std::optional<SecurityExchange> security_exchange;
  std::optional<PartyId> party_id;
  std::optional<CusipId> cusip;
  std::optional<SedolId> sedol;
  std::optional<IsinId> isin;
  std::optional<RicId> ric;
  std::optional<ExchangeId> exchange_id;
  std::optional<BloombergId> bloomberg_id;

  std::optional<DatabaseId> database_id;
  std::optional<PriceTick> price_tick;
  std::optional<QuantityTick> quantity_tick;
  std::optional<MinQuantity> min_quantity;
  std::optional<MaxQuantity> max_quantity;

  InstrumentId identifier{0};

  std::optional<PartyRole> party_role;
  std::optional<SecurityType> security_type;

  [[nodiscard]]
  auto operator==(const Instrument&) const -> bool = default;
};

}  // namespace simulator::trading_system

template <>
struct fmt::formatter<simulator::trading_system::Instrument> {
  using formattable = simulator::trading_system::Instrument;

  constexpr static auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& instrument, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_INSTRUMENT_HPP_