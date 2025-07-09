#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <initializer_list>

#include "common/instrument.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "instruments/cache.hpp"
#include "instruments/lookup_error.hpp"
#include "instruments/sources.hpp"

namespace simulator::trading_system::instrument::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*-magic-numbers)

struct Instruments : public Test {
  auto fill_cache(std::initializer_list<Instrument> instruments) -> void {
    instrument::MemorySource source;
    std::ranges::for_each(instruments, [&](const auto& instrument) {
      source.add_instrument(instrument);
    });
    cache.load(source);
  }

  InstrumentDescriptor descriptor;  // NOLINT:(*non-private-member*)
  Cache cache = Cache::create();    // NOLINT:(*non-private-member*)
};

TEST_F(Instruments, RetrieveInstrumentsFromCache) {
  fill_cache({[] {
                Instrument instrument;
                instrument.symbol = Symbol{"AAPL"};
                return instrument;
              }(),
              [] {
                Instrument instrument;
                instrument.symbol = Symbol{"TSLA"};
                return instrument;
              }()});

  ASSERT_THAT(cache.retrieve_instruments(),
              UnorderedElementsAre(Field(&Instrument::symbol, Symbol{"AAPL"}),
                                   Field(&Instrument::symbol, Symbol{"TSLA"})));
}

TEST_F(Instruments, SuccessfullyResolveInstrumentByDescriptor) {
  descriptor.symbol = Symbol{"TSLA"};
  descriptor.currency = Currency{"USD"};
  descriptor.security_exchange = SecurityExchange{"XNAS"};
  fill_cache({[] {
                Instrument instrument;
                instrument.symbol = Symbol{"TSLA"};
                instrument.security_type = SecurityType::Option::CommonStock;
                instrument.security_exchange = SecurityExchange{"XNAS"};
                instrument.price_currency = PriceCurrency{"USD"};
                return instrument;
              }(),
              [] {
                Instrument instrument;
                instrument.symbol = Symbol{"AAPL"};
                instrument.security_type = SecurityType::Option::CommonStock;
                instrument.security_exchange = SecurityExchange{"XLON"};
                instrument.price_currency = PriceCurrency{"GBP"};
                return instrument;
              }()});

  const auto view = cache.find(descriptor);

  ASSERT_TRUE(view.has_value());
  EXPECT_EQ(view->instrument().symbol, Symbol{"TSLA"});
  EXPECT_EQ(view->instrument().security_exchange, SecurityExchange{"XNAS"});
  EXPECT_EQ(view->instrument().price_currency, PriceCurrency{"USD"});
  EXPECT_EQ(view->instrument().security_type,
            SecurityType::Option::CommonStock);
}

TEST_F(Instruments, SuccessfullyResolveInstrumentByInstrument) {
  fill_cache({[] {
                Instrument instrument;
                instrument.symbol = Symbol{"TSLA"};
                instrument.database_id = DatabaseId{3};
                return instrument;
              }(),
              [] {
                Instrument instrument;
                instrument.symbol = Symbol{"AAPL"};
                instrument.database_id = DatabaseId{2};
                return instrument;
              }()});

  Instrument instrument;
  instrument.symbol = Symbol{"AAPL"};

  const auto view = cache.find(instrument);

  ASSERT_TRUE(view.has_value());
  EXPECT_EQ(view->instrument().database_id, DatabaseId{2});
}

TEST_F(Instruments, FailToResolveInstrumentFromEmptyCache) {
  descriptor.symbol = Symbol{"AAPL"};

  const auto view = cache.find(descriptor);

  ASSERT_FALSE(view.has_value());
  EXPECT_EQ(view.error(), instrument::LookupError::InstrumentNotFound);
}

TEST_F(Instruments, FailToResolveInstrumentByInstrumentFromEmptyCache) {
  Instrument instrument;
  instrument.symbol = Symbol{"AAPL"};

  const auto view = cache.find(instrument);

  ASSERT_FALSE(view.has_value());
  EXPECT_EQ(view.error(), instrument::LookupError::InstrumentNotFound);
}

TEST_F(Instruments, FailToResolveInstrumentByEmptyDescriptor) {
  fill_cache({[] {
                Instrument instrument;
                instrument.symbol = Symbol{"TSLA"};
                return instrument;
              }(),
              [] {
                Instrument instrument;
                instrument.symbol = Symbol{"AAPL"};
                return instrument;
              }()});

  const auto view = cache.find(descriptor);

  ASSERT_FALSE(view.has_value());
  EXPECT_EQ(view.error(),
            instrument::LookupError::MalformedInstrumentDescriptor);
}

TEST_F(Instruments, FailToResolveInstrumentByEmptyInstrument) {
  fill_cache({[] {
                Instrument instrument;
                instrument.symbol = Symbol{"TSLA"};
                return instrument;
              }(),
              [] {
                Instrument instrument;
                instrument.symbol = Symbol{"AAPL"};
                return instrument;
              }()});

  const Instrument instrument;
  const auto view = cache.find(instrument);

  ASSERT_FALSE(view.has_value());
  EXPECT_EQ(view.error(), instrument::LookupError::InstrumentNotFound);
}

TEST_F(Instruments, FailToResolveInstrumentBecauseOfAmbiguity) {
  descriptor.symbol = Symbol{"AAPL"};
  fill_cache({[] {
                Instrument instrument;
                instrument.symbol = Symbol{"AAPL"};
                instrument.security_type = SecurityType::Option::CommonStock;
                instrument.security_exchange = SecurityExchange{"XLON"};
                instrument.price_currency = PriceCurrency{"GBP"};
                return instrument;
              }(),
              [] {
                Instrument instrument;
                instrument.symbol = Symbol{"AAPL"};
                instrument.security_type = SecurityType::Option::CommonStock;
                instrument.security_exchange = SecurityExchange{"XNAS"};
                instrument.price_currency = PriceCurrency{"USD"};
                return instrument;
              }()});

  const auto view = cache.find(descriptor);

  ASSERT_FALSE(view.has_value());
  EXPECT_EQ(view.error(),
            instrument::LookupError::AmbiguousInstrumentDescriptor);
}

TEST_F(Instruments, FailToResolvesTheFirstInstrument) {
  fill_cache({[] {
                Instrument instrument;
                instrument.database_id = DatabaseId{3};
                instrument.symbol = Symbol{"AAPL"};
                return instrument;
              }(),
              [] {
                Instrument instrument;
                instrument.database_id = DatabaseId{2};
                instrument.symbol = Symbol{"AAPL"};
                return instrument;
              }()});

  Instrument instrument;
  instrument.symbol = Symbol{"AAPL"};

  const auto view = cache.find(instrument);

  ASSERT_TRUE(view.has_value());
  EXPECT_EQ(view->instrument().database_id, DatabaseId{3});
}

TEST_F(Instruments, FailToResolveNonexistentInstrument) {
  descriptor.symbol = Symbol{"AAPL"};
  descriptor.security_exchange = SecurityExchange{"XNYS"};
  fill_cache({[] {
                Instrument instrument;
                instrument.symbol = Symbol{"AAPL"};
                instrument.security_type = SecurityType::Option::CommonStock;
                instrument.security_exchange = SecurityExchange{"XLON"};
                instrument.price_currency = PriceCurrency{"GBP"};
                return instrument;
              }(),
              [] {
                Instrument instrument;
                instrument.symbol = Symbol{"AAPL"};
                instrument.security_type = SecurityType::Option::CommonStock;
                instrument.security_exchange = SecurityExchange{"XNAS"};
                instrument.price_currency = PriceCurrency{"USD"};
                return instrument;
              }()});

  const auto view = cache.find(descriptor);

  ASSERT_FALSE(view.has_value());
  EXPECT_EQ(view.error(), instrument::LookupError::InstrumentNotFound);
}

TEST_F(Instruments, FailToResolveNonexistentInstrumentByInstrument) {
  fill_cache({[] {
                Instrument instrument;
                instrument.symbol = Symbol{"TSLA"};
                return instrument;
              }(),
              [] {
                Instrument instrument;
                instrument.symbol = Symbol{"AAPL"};
                return instrument;
              }()});

  Instrument instrument;
  instrument.symbol = Symbol{"VOW"};

  const auto view = cache.find(instrument);

  ASSERT_FALSE(view.has_value());
  EXPECT_EQ(view.error(), instrument::LookupError::InstrumentNotFound);
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::trading_system::instrument::test
