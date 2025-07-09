#include "ih/lookup/strategies.hpp"

#include <optional>
#include <tl/expected.hpp>

#include "ih/lookup/matchers.hpp"

namespace simulator::trading_system::instrument::lookup {

namespace {

template <typename T>
constexpr auto missing(const std::optional<T>& attribute) -> bool {
  return !attribute.has_value();
}

}  // namespace

SymbolLookup::SymbolLookup(const InstrumentDescriptor& descriptor)
    : descriptor_(&descriptor) {}

auto SymbolLookup::operator()(const Instrument& instrument) const
    -> MatchRate {
  constexpr auto matcher = make_matcher(match_symbol,
                                        match_security_type,
                                        match_currency,
                                        match_security_exchange,
                                        match_party);
  return matcher(*descriptor_, instrument);
}

auto SymbolLookup::create(const InstrumentDescriptor& descriptor)
    -> tl::expected<SymbolLookup, LookupError> {
  if (missing(descriptor.symbol)) [[unlikely]] {
    return tl::unexpected(LookupError::MalformedInstrumentDescriptor);
  }
  return SymbolLookup(descriptor);
}

SedolIdLookup::SedolIdLookup(const InstrumentDescriptor& descriptor)
    : descriptor_(&descriptor) {}

auto SedolIdLookup::operator()(const Instrument& instrument) const
    -> MatchRate {
  constexpr auto matcher = make_matcher(match_sedol_id,
                                        match_security_type,
                                        match_currency,
                                        match_security_exchange,
                                        match_symbol,
                                        match_party);
  return matcher(*descriptor_, instrument);
}

auto SedolIdLookup::create(const InstrumentDescriptor& descriptor)
    -> tl::expected<SedolIdLookup, LookupError> {
  if (descriptor.security_id_source != SecurityIdSource::Option::Sedol ||
      missing(descriptor.security_id)) {
    return tl::unexpected(LookupError::MalformedInstrumentDescriptor);
  }
  return SedolIdLookup(descriptor);
}

CusipIdLookup::CusipIdLookup(const InstrumentDescriptor& descriptor)
    : descriptor_(&descriptor) {}

auto CusipIdLookup::operator()(const Instrument& instrument) const
    -> MatchRate {
  constexpr auto matcher = make_matcher(match_cusip_id,
                                        match_security_type,
                                        match_currency,
                                        match_security_exchange,
                                        match_symbol,
                                        match_party);
  return matcher(*descriptor_, instrument);
}

auto CusipIdLookup::create(const InstrumentDescriptor& descriptor)
    -> tl::expected<CusipIdLookup, LookupError> {
  if (descriptor.security_id_source != SecurityIdSource::Option::Cusip ||
      missing(descriptor.security_id)) {
    return tl::unexpected(LookupError::MalformedInstrumentDescriptor);
  }
  return CusipIdLookup(descriptor);
}

IsinIdLookup::IsinIdLookup(const InstrumentDescriptor& descriptor)
    : descriptor_(&descriptor) {}

auto IsinIdLookup::operator()(const Instrument& instrument) const
    -> MatchRate {
  constexpr auto matcher = make_matcher(match_isin_id,
                                        match_security_type,
                                        match_currency,
                                        match_security_exchange,
                                        match_symbol,
                                        match_party);
  return matcher(*descriptor_, instrument);
}

auto IsinIdLookup::create(const InstrumentDescriptor& descriptor)
    -> tl::expected<IsinIdLookup, LookupError> {
  if (descriptor.security_id_source != SecurityIdSource::Option::Isin ||
      missing(descriptor.security_id)) {
    return tl::unexpected(LookupError::MalformedInstrumentDescriptor);
  }
  if (missing(descriptor.security_type)) {
    return tl::unexpected(LookupError::MalformedInstrumentDescriptor);
  }
  if (missing(descriptor.currency)) {
    return tl::unexpected(LookupError::MalformedInstrumentDescriptor);
  }
  if (missing(descriptor.security_exchange)) {
    return tl::unexpected(LookupError::MalformedInstrumentDescriptor);
  }
  return IsinIdLookup(descriptor);
}

RicIdLookup::RicIdLookup(const InstrumentDescriptor& descriptor)
    : descriptor_(&descriptor) {}

auto RicIdLookup::operator()(const Instrument& instrument) const
    -> MatchRate {
  constexpr auto matcher = make_matcher(match_ric_id,
                                        match_security_type,
                                        match_currency,
                                        match_security_exchange,
                                        match_symbol,
                                        match_party);
  return matcher(*descriptor_, instrument);
}

auto RicIdLookup::create(const InstrumentDescriptor& descriptor)
    -> tl::expected<RicIdLookup, LookupError> {
  if (descriptor.security_id_source != SecurityIdSource::Option::Ric ||
      missing(descriptor.security_id)) {
    return tl::unexpected(LookupError::MalformedInstrumentDescriptor);
  }
  return RicIdLookup(descriptor);
}

ExchangeIdLookup::ExchangeIdLookup(const InstrumentDescriptor& descriptor)
    : descriptor_(&descriptor) {}

auto ExchangeIdLookup::operator()(const Instrument& instrument) const
    -> MatchRate {
  constexpr auto matcher = make_matcher(match_exchange_id,
                                        match_security_type,
                                        match_currency,
                                        match_security_exchange,
                                        match_symbol,
                                        match_party);
  return matcher(*descriptor_, instrument);
}

auto ExchangeIdLookup::create(const InstrumentDescriptor& descriptor)
    -> tl::expected<ExchangeIdLookup, LookupError> {
  if (missing(descriptor.security_id) ||
      descriptor.security_id_source !=
          SecurityIdSource::Option::ExchangeSymbol) {
    return tl::unexpected(LookupError::MalformedInstrumentDescriptor);
  }
  return ExchangeIdLookup(descriptor);
}

BloombergIdLookup::BloombergIdLookup(const InstrumentDescriptor& descriptor)
    : descriptor_(&descriptor) {}

auto BloombergIdLookup::operator()(const Instrument& instrument) const
    -> MatchRate {
  constexpr auto matcher = make_matcher(match_bloomberg_id,
                                        match_security_type,
                                        match_currency,
                                        match_security_exchange,
                                        match_symbol,
                                        match_party);
  return matcher(*descriptor_, instrument);
}

auto BloombergIdLookup::create(const InstrumentDescriptor& descriptor)
    -> tl::expected<BloombergIdLookup, LookupError> {
  if (missing(descriptor.security_id) ||
      descriptor.security_id_source !=
          SecurityIdSource::Option::BloombergSymbol) {
    return tl::unexpected(LookupError::MalformedInstrumentDescriptor);
  }
  return BloombergIdLookup(descriptor);
}

}  // namespace simulator::trading_system::instrument::lookup