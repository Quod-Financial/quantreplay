#include <gmock/gmock.h>

#include <memory>

#include "ih/tools/instrument_resolver.hpp"
#include "instruments/sources.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace ::testing;  // NOLINT

class TradingSystemCachedInstrumentResolver : public Test {
 public:
  auto resolve(const InstrumentDescriptor& descriptor) {
    return resolver_->resolve_instrument(descriptor);
  }

 private:
  static auto create_instruments_source() -> instrument::MemorySource {
    instrument::MemorySource source;
    return source
        .add_instrument([] {
          Instrument instrument;
          instrument.symbol = Symbol{"UNIQUE_SYMBOL"};
          return instrument;
        }())
        .add_instrument([] {
          Instrument instrument;
          instrument.symbol = Symbol{"AMBIGUOUS_SYMBOL"};
          return instrument;
        }())
        .add_instrument([] {
          Instrument instrument;
          instrument.symbol = Symbol{"AMBIGUOUS_SYMBOL"};
          return instrument;
        }());
  }

  auto SetUp() -> void override { cache_.load(create_instruments_source()); }

  instrument::Cache cache_ = instrument::Cache::create();
  std::unique_ptr<InstrumentResolver> resolver_ =
      create_cached_instrument_resolver(cache_);
};

TEST_F(TradingSystemCachedInstrumentResolver, ResolvesUniqueInstrument) {
  const auto instrument_view = resolve({.security_id = std::nullopt,
                                        .symbol = Symbol{"UNIQUE_SYMBOL"},
                                        .currency = std::nullopt,
                                        .security_exchange = std::nullopt,
                                        .parties = {},
                                        .requester_instrument_id = std::nullopt,
                                        .security_type = std::nullopt,
                                        .security_id_source = std::nullopt});

  ASSERT_TRUE(instrument_view.has_value());
  EXPECT_EQ(instrument_view->instrument().symbol, Symbol{"UNIQUE_SYMBOL"});
}

TEST_F(TradingSystemCachedInstrumentResolver,
       FailsToResolveAmbiguousInstrument) {
  const auto instrument_view = resolve({.security_id = std::nullopt,
                                        .symbol = Symbol{"AMBIGUOUS_SYMBOL"},
                                        .currency = std::nullopt,
                                        .security_exchange = std::nullopt,
                                        .parties = {},
                                        .requester_instrument_id = std::nullopt,
                                        .security_type = std::nullopt,
                                        .security_id_source = std::nullopt});

  ASSERT_FALSE(instrument_view.has_value());
}

TEST_F(TradingSystemCachedInstrumentResolver,
       FailsToResolveNonExistentInstrument) {
  const auto instrument_view = resolve({.security_id = std::nullopt,
                                        .symbol = Symbol{"NON_EXISTENT_SYMBOL"},
                                        .currency = std::nullopt,
                                        .security_exchange = std::nullopt,
                                        .parties = {},
                                        .requester_instrument_id = std::nullopt,
                                        .security_type = std::nullopt,
                                        .security_id_source = std::nullopt});

  ASSERT_FALSE(instrument_view.has_value());
}

}  // namespace
}  // namespace simulator::trading_system::test