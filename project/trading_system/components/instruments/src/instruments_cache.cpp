#include "ih/instruments_cache.hpp"

#include <functional>
#include <stdexcept>
#include <tl/expected.hpp>

#include "ih/instruments_container.hpp"
#include "ih/lookup/lookup.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::instrument {

Cache::Implementation::Implementation()
    : id_generation_context_(idgen::make_instrument_id_generation_ctx()) {}

auto Cache::Implementation::add_instrument(Instrument instrument) -> void {
  log::debug("adding instrument to cache");
  instrument.identifier = generate_new_id();

  auto added_instrument_iter = container_.emplace(instrument);
  if (added_instrument_iter == container_.end()) [[unlikely]] {
    log::err(
        "failed to add an instrument to the cache because another instrument "
        "with the same internal identifier already exists, "
        "this may indicate a bug in internal instrument identifier generation "
        "algorithm, kindly raise an issue, ignored instrument: {}",
        instrument);
    throw std::runtime_error{"internal identifier collision detected"};
  }

  log::info("cached instrument - {}", instrument);
}

auto Cache::Implementation::retrieve_instruments() const
    -> std::vector<Instrument> {
  return {container_.begin(), container_.end()};
}

auto Cache::Implementation::find_instrument(
    const InstrumentDescriptor& descriptor) const
    -> tl::expected<View, LookupError> {
  log::debug("looking for an instrument by descriptor, {}", descriptor);

  const auto lookup = lookup::Lookup::create(descriptor);
  if (lookup.has_value()) {
    return std::invoke(*lookup, container_);
  }

  log::debug("failed to create a lookup strategy for given descriptor: {}",
             descriptor);
  return tl::make_unexpected(lookup.error());
}

auto Cache::Implementation::find_instrument(
    const InstrumentSpecification& specification) const
    -> tl::expected<View, LookupError> {
  log::debug("looking for an instrument by instrument, {}", specification);

  const auto pred = [&specification](const Instrument& cached) -> bool {
    return cached.symbol == specification.symbol &&
           cached.price_currency == specification.price_currency &&
           cached.base_currency == specification.base_currency &&
           cached.security_exchange == specification.security_exchange &&
           cached.party_id == specification.party_id &&
           cached.cusip == specification.cusip &&
           cached.sedol == specification.sedol &&
           cached.isin == specification.isin &&
           cached.ric == specification.ric &&
           cached.exchange_id == specification.exchange_id &&
           cached.bloomberg_id == specification.bloomberg_id &&
           cached.party_role == specification.party_role &&
           cached.security_type == specification.security_type;
  };

  const auto iter = std::ranges::find_if(container_, pred);
  if (iter != container_.end()) {
    return View{*iter};
  }

  log::debug("the instrument was not found in the cache, {}", specification);
  return tl::make_unexpected(LookupError::InstrumentNotFound);
}

auto Cache::Implementation::generate_new_id() -> InstrumentId {
  const auto generated_id = idgen::generate_new_id(id_generation_context_);
  if (!generated_id.has_value()) [[unlikely]] {
    // Normally this code may never be executed, instrument id generation error
    // can happen only when all possible identifiers were generated previously.
    // It means that we have more instruments than std::uint64_t max value.
    log::err(
        "failed to generate a new internal instrument identifier, "
        "this may indicate a bug in internal instrument identifier "
        "generation algorithm, kindly raise an issue");
    throw std::runtime_error{"unable to generate new instrument identifier"};
  }
  return *generated_id;
}

}  // namespace simulator::trading_system::instrument