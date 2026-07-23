#include <cstdlib>
#include <stdexcept>
#include <tl/expected.hpp>
#include <utility>

#include "common/instrument.hpp"
#include "data_layer/api/exceptions/exceptions.hpp"
#include "ih/instruments_cache.hpp"
#include "instruments/cache.hpp"
#include "instruments/lookup_error.hpp"
#include "instruments/sources.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::instrument {

Cache::Cache(std::unique_ptr<Implementation> impl) noexcept
    : impl_(std::move(impl)) {}

Cache::Cache(Cache&&) noexcept = default;

Cache::~Cache() noexcept = default;

auto Cache::operator=(Cache&&) noexcept -> Cache& = default;

auto Cache::create() -> Cache {
  try {
    Cache cache{std::make_unique<Implementation>()};
    log::info("created instruments cache");
    return cache;
  } catch (const std::exception& exception) {
    log::err("failed to create instruments cache implementation: {}",
             exception.what());
  } catch (...) {
    log::err(
        "failed to create instruments cache implementation: "
        "unknown error occurred");
  }
  throw std::runtime_error{"failed to create instruments cache"};
}

auto Cache::find(const InstrumentDescriptor& descriptor) const
    -> tl::expected<View, LookupError> {
  return impl().find_instrument(descriptor);
}

auto Cache::find(const InstrumentSpecification& specification) const
    -> tl::expected<View, LookupError> {
  return impl().find_instrument(specification);
}

auto Cache::retrieve_instruments() const -> std::vector<Instrument> {
  log::debug("retrieving instruments from cache");
  return impl().retrieve_instruments();
}

auto Cache::load(const DatabaseSource& source) -> void {
  load_instruments(source);
  log::info("loaded instruments from database source into cache");
}

auto Cache::load(const MemorySource& source) -> void {
  load_instruments(source);
  log::info("loaded instruments from memory source into cache");
}

template <typename SourceType>
auto Cache::load_instruments(const SourceType& source) -> void {
  log::debug("loading instruments into cache from a source");
  auto& cache = impl();
  try {
    source.load_instruments([&](Instrument instrument) {
      cache.add_instrument(std::move(instrument));
    });
    return;
  } catch (const data_layer::ConnectionFailure&) {
    // preserve the type for retry
    throw;
  } catch (const std::exception& exception) {
    log::err("an error occurred while loading instruments into cache: {}",
             exception.what());
  } catch (...) {
    log::err("an unknown error occurred while loading instruments into cache");
  }
  throw std::runtime_error{"failed to load instruments into cache"};
}

auto Cache::impl() const noexcept -> Implementation& {
  if (impl_) [[likely]] {
    return *impl_;
  }

  log::err(
      "CRITICAL: instruments cache implementation "
      "has not been allocated/initialized, the module contains critical bug, "
      "can not continue program execution, aborting...");

  // This is a fatal instruments module internal error,
  // cannot continue execution
  std::abort();
}

}  // namespace simulator::trading_system::instrument

auto fmt::formatter<
    simulator::trading_system::instrument::Cache::InstrumentSpecification>::
    format(const formattable& spec, format_context& ctx) const
    -> format_context::iterator {
  return format_to(
      ctx.out(),
      R"({{ "symbol": {}, "price_currency": {}, "base_currency": {}, )"
      R"("security_exchange": {}, party_id: {}, cusip: {}, sedol: {}, )"
      R"("isin": {}, "ric": {}, "exchange_id": {}, "bloomberg_id": {}, )"
      R"("party_role": {}, "security_type": {} }})",
      spec.symbol,
      spec.price_currency,
      spec.base_currency,
      spec.security_exchange,
      spec.party_id,
      spec.cusip,
      spec.sedol,
      spec.isin,
      spec.ric,
      spec.exchange_id,
      spec.bloomberg_id,
      spec.party_role,
      spec.security_type);
}
