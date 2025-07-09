#include "ih/lookup/matchers.hpp"

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>

#include "ih/lookup/currency_category.hpp"

namespace simulator::trading_system::instrument::lookup {

namespace {

template <typename T>
constexpr auto missing(const std::optional<T>& attributes) -> bool {
  return !attributes.has_value();
}

auto match_security_id(const InstrumentDescriptor& descriptor,
                       const Instrument& instrument,
                       const auto Instrument::*security_id_field) -> MatchRate {
  if (missing(descriptor.security_id)) {
    return MatchRate::Unmatchable;
  }
  if (missing(instrument.*security_id_field)) {
    return MatchRate::NoMatch;
  }

  const bool equal =
      static_cast<const std::string&>(*(instrument.*security_id_field)) ==
      static_cast<const std::string&>(*descriptor.security_id);

  return equal ? MatchRate::Match : MatchRate::NoMatch;
}

constexpr auto get_match_currency(const Instrument& instrument)
    -> std::optional<std::string_view> {
  const auto category = determine_currency_category(instrument);
  if (category == CurrencyCategory::PriceCurrency &&
      instrument.price_currency.has_value()) {
    return static_cast<const std::string&>(*instrument.price_currency);
  }
  if (category == CurrencyCategory::BaseCurrency &&
      instrument.base_currency.has_value()) {
    return static_cast<const std::string&>(*instrument.base_currency);
  }
  return std::nullopt;
}

}  // namespace

auto match_symbol(const InstrumentDescriptor& descriptor,
                  const Instrument& instrument) -> MatchRate {
  if (missing(descriptor.symbol)) {
    return MatchRate::Unmatchable;
  }
  return instrument.symbol == *descriptor.symbol ? MatchRate::Match
                                                 : MatchRate::NoMatch;
}

auto match_sedol_id(const InstrumentDescriptor& descriptor,
                    const Instrument& instrument) -> MatchRate {
  return match_security_id(descriptor, instrument, &Instrument::sedol);
}

auto match_cusip_id(const InstrumentDescriptor& descriptor,
                    const Instrument& instrument) -> MatchRate {
  return match_security_id(descriptor, instrument, &Instrument::cusip);
}

auto match_isin_id(const InstrumentDescriptor& descriptor,
                   const Instrument& instrument) -> MatchRate {
  return match_security_id(descriptor, instrument, &Instrument::isin);
}

auto match_ric_id(const InstrumentDescriptor& descriptor,
                  const Instrument& instrument) -> MatchRate {
  return match_security_id(descriptor, instrument, &Instrument::ric);
}

auto match_exchange_id(const InstrumentDescriptor& descriptor,
                       const Instrument& instrument) -> MatchRate {
  return match_security_id(descriptor, instrument, &Instrument::exchange_id);
}

auto match_bloomberg_id(const InstrumentDescriptor& descriptor,
                        const Instrument& instrument) -> MatchRate {
  return match_security_id(descriptor, instrument, &Instrument::bloomberg_id);
}

auto match_security_type(const InstrumentDescriptor& descriptor,
                         const Instrument& instrument) -> MatchRate {
  if (missing(descriptor.security_type)) {
    return MatchRate::Unmatchable;
  }
  return instrument.security_type == descriptor.security_type
             ? MatchRate::Match
             : MatchRate::NoMatch;
}

auto match_security_exchange(const InstrumentDescriptor& descriptor,
                             const Instrument& instrument) -> MatchRate {
  if (missing(descriptor.security_exchange)) {
    return MatchRate::Unmatchable;
  }
  return instrument.security_exchange == descriptor.security_exchange
             ? MatchRate::Match
             : MatchRate::NoMatch;
}

auto match_currency(const InstrumentDescriptor& descriptor,
                    const Instrument& instrument) -> MatchRate {
  if (missing(descriptor.currency)) {
    return MatchRate::Unmatchable;
  }

  if (const auto currency = get_match_currency(instrument)) {
    return *currency == static_cast<const std::string&>(*descriptor.currency)
               ? MatchRate::Match
               : MatchRate::NoMatch;
  }

  return MatchRate::NoMatch;
}

auto match_party(const InstrumentDescriptor& descriptor,
                 const Instrument& instrument) -> MatchRate {
  if (missing(instrument.party_id) || missing(instrument.party_role)) {
    return MatchRate::Unmatchable;
  }

  const auto matches = [&](const Party& party) {
    return party.role() == *instrument.party_role &&
           party.party_id() == *instrument.party_id;
  };

  return std::ranges::any_of(descriptor.parties, matches)
             ? MatchRate::Match
             : MatchRate::Unmatchable;
}

}  // namespace simulator::trading_system::instrument::lookup
