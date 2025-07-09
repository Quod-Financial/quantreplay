#include "ih/tools/instrument_resolver.hpp"

#include <functional>

#include "log/logging.hpp"

namespace simulator::trading_system {
namespace {

struct CachedInstrumentResolver final : InstrumentResolver {
  explicit CachedInstrumentResolver(const instrument::Cache& cache) noexcept
      : cache_{cache} {}

  auto resolve_instrument(const InstrumentDescriptor& descriptor) const
      -> tl::expected<instrument::View, instrument::LookupError> override {
    return cache_.get().find(descriptor);
  }

  auto resolve_instrument(const Instrument& instrument) const
      -> tl::expected<instrument::View, instrument::LookupError> override {
    return cache_.get().find(instrument);
  }

 private:
  std::reference_wrapper<const instrument::Cache> cache_;
};

}  // namespace

auto create_cached_instrument_resolver(const instrument::Cache& cache)
    -> std::unique_ptr<InstrumentResolver> {
  log::debug("creating cached instrument resolver");
  return std::make_unique<CachedInstrumentResolver>(cache);
}

}  // namespace simulator::trading_system
