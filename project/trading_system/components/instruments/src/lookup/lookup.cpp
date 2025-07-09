#include "ih/lookup/lookup.hpp"

#include <tl/expected.hpp>
#include <utility>
#include <variant>

#include "core/domain/instrument_descriptor.hpp"
#include "ih/lookup/match_rate.hpp"
#include "ih/lookup/strategies.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::instrument::lookup {

template <typename StrategyType>
Lookup::Lookup(StrategyType&& strategy)
    : strategy_(std::forward<StrategyType>(strategy)) {}

template <typename StrategyType>
auto Lookup::create_lookup(const InstrumentDescriptor& descriptor)
    -> tl::expected<Lookup, LookupError> {
  return StrategyType::create(descriptor).map([](auto&& lookup) {
    return Lookup{std::forward<decltype(lookup)>(lookup)};
  });
}

auto Lookup::operator()(const Container& container) const
    -> tl::expected<View, LookupError> {
  log::debug("looking for an instrument by descriptor");

  const auto calculate_rate = [this](const Instrument& instrument) {
    return std::visit(
        [&](const auto& strategy) { return strategy(instrument); }, strategy_);
  };

  struct {
    const Instrument* best_match = nullptr;
    const Instrument* ambiguous_match = nullptr;
    MatchRate rate = MatchRate::Unmatchable;
  } ctx;

  for (const auto& instrument : container) {
    if (const auto rate = calculate_rate(instrument);
        rate > MatchRate::Unmatchable) {
      if (rate > ctx.rate) {
        ctx.rate = rate;
        ctx.best_match = &instrument;
        ctx.ambiguous_match = nullptr;
      } else if (rate == ctx.rate) {
        ctx.ambiguous_match = &instrument;
      }
    }
  }

  if (ctx.best_match == nullptr) {
    log::debug("no instrument found for a given descriptor");
    return tl::make_unexpected(LookupError::InstrumentNotFound);
  }

  if (ctx.ambiguous_match != nullptr) {
    log::debug(
        "multiple instruments found for a given descriptor: 1 - {}; 2 - {}",
        *ctx.best_match,
        *ctx.ambiguous_match);
    return tl::make_unexpected(LookupError::AmbiguousInstrumentDescriptor);
  }

  log::debug("instrument found - {}", *ctx.best_match);
  return View{*ctx.best_match};
}

auto Lookup::create(const InstrumentDescriptor& descriptor)
    -> tl::expected<Lookup, LookupError> {
  if (const auto id_source = descriptor.security_id_source) {
    switch (static_cast<SecurityIdSource::Option>(*id_source)) {
      case SecurityIdSource::Option::Cusip:
        return create_lookup<CusipIdLookup>(descriptor);
      case SecurityIdSource::Option::Sedol:
        return create_lookup<SedolIdLookup>(descriptor);
      case SecurityIdSource::Option::Isin:
        return create_lookup<IsinIdLookup>(descriptor);
      case SecurityIdSource::Option::Ric:
        return create_lookup<RicIdLookup>(descriptor);
      case SecurityIdSource::Option::ExchangeSymbol:
        return create_lookup<ExchangeIdLookup>(descriptor);
      case SecurityIdSource::Option::BloombergSymbol:
        return create_lookup<BloombergIdLookup>(descriptor);
    }
    return tl::make_unexpected(LookupError::MalformedInstrumentDescriptor);
  }
  if (descriptor.symbol.has_value()) {
    return create_lookup<SymbolLookup>(descriptor);
  }
  return tl::make_unexpected(LookupError::MalformedInstrumentDescriptor);
}

}  // namespace simulator::trading_system::instrument::lookup