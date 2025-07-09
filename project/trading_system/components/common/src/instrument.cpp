#include "common/instrument.hpp"

#include <fmt/base.h>

#include <stdexcept>

#include "core/common/name.hpp"

auto fmt::formatter<simulator::trading_system::Instrument>::format(
    const formattable& instrument, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;

  return format_to(context.out(),
                   "Instrument={{ "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, "
                   "{}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, "
                   "{}={}, {}={}, {}={}"
                   " }}",
                   name_of(instrument.identifier),
                   instrument.identifier,
                   name_of(instrument.database_id),
                   instrument.database_id,
                   name_of(instrument.symbol),
                   instrument.symbol,
                   name_of(instrument.security_type),
                   instrument.security_type,
                   name_of(instrument.price_currency),
                   instrument.price_currency,
                   name_of(instrument.base_currency),
                   instrument.base_currency,
                   name_of(instrument.security_exchange),
                   instrument.security_exchange,
                   name_of(instrument.cusip),
                   instrument.cusip,
                   name_of(instrument.sedol),
                   instrument.sedol,
                   name_of(instrument.isin),
                   instrument.isin,
                   name_of(instrument.ric),
                   instrument.ric,
                   name_of(instrument.exchange_id),
                   instrument.exchange_id,
                   name_of(instrument.bloomberg_id),
                   instrument.bloomberg_id,
                   name_of(instrument.party_id),
                   instrument.party_id,
                   name_of(instrument.party_role),
                   instrument.party_role,
                   name_of(instrument.price_tick),
                   instrument.price_tick,
                   name_of(instrument.quantity_tick),
                   instrument.quantity_tick,
                   name_of(instrument.min_quantity),
                   instrument.min_quantity,
                   name_of(instrument.max_quantity),
                   instrument.max_quantity);
}