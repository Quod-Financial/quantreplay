#include "ih/instruments_matcher.hpp"

#include "ih/lookup/match_rate.hpp"
#include "ih/lookup/strategies.hpp"
#include "instruments/matcher.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::instrument {

namespace {

template <typename StrategyType>
auto match(const InstrumentDescriptor& descriptor, const Instrument& instrument)
    -> tl::expected<void, LookupError> {
  return StrategyType::create(descriptor)
      .map([&](const auto& strategy) { return strategy(instrument); })
      .and_then([&](const auto& rate) -> tl::expected<void, LookupError> {
        if (rate > lookup::MatchRate::Unmatchable) {
          return {};
        }

        log::debug("InstrumentDescriptor does not match the Instrument");
        return tl::make_unexpected(LookupError::InstrumentNotFound);
      });
}

}  // namespace

Matcher::Matcher(std::unique_ptr<Implementation> impl) noexcept
    : impl_{std::move(impl)} {}

Matcher::Matcher(Matcher&&) noexcept = default;
Matcher::~Matcher() noexcept = default;

auto Matcher::create(const Instrument& instrument) -> Matcher {
  return Matcher{std::make_unique<Implementation>(instrument)};
}

auto Matcher::operator()(const InstrumentDescriptor& descriptor) const
    -> tl::expected<void, LookupError> {
  return impl_->operator()(descriptor);
}

Matcher::Implementation::Implementation(const Instrument& instrument)
    : instrument_{instrument} {}

auto Matcher::Implementation::operator()(const InstrumentDescriptor& descriptor)
    const -> tl::expected<void, LookupError> {
  log::debug("matching InstrumentDescriptor {} with the Instrument {}",
             descriptor,
             instrument_);
  if (const auto id_source = descriptor.security_id_source) {
    switch (static_cast<SecurityIdSource::Option>(*id_source)) {
      case SecurityIdSource::Option::Sedol:
        return match<lookup::SedolIdLookup>(descriptor, instrument_);
      case SecurityIdSource::Option::Cusip:
        return match<lookup::CusipIdLookup>(descriptor, instrument_);
      case SecurityIdSource::Option::Isin:
        return match<lookup::IsinIdLookup>(descriptor, instrument_);
      case SecurityIdSource::Option::Ric:
        return match<lookup::RicIdLookup>(descriptor, instrument_);
      case SecurityIdSource::Option::ExchangeSymbol:
        return match<lookup::ExchangeIdLookup>(descriptor, instrument_);
      case SecurityIdSource::Option::BloombergSymbol:
        return match<lookup::BloombergIdLookup>(descriptor, instrument_);
    }
  }

  if (descriptor.symbol.has_value()) {
    return match<lookup::SymbolLookup>(descriptor, instrument_);
  }

  log::debug(
      "lookup strategy was not chosen because InstrumentDescriptor is not "
      "valid");
  return tl::make_unexpected(LookupError::MalformedInstrumentDescriptor);
}

}  // namespace simulator::trading_system::instrument
