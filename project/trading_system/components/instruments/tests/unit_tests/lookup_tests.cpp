#include <gtest/gtest.h>

#include "common/instrument.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "ih/instruments_container.hpp"
#include "ih/lookup/lookup.hpp"

namespace simulator::trading_system::instrument::lookup::tests {
namespace {

using namespace ::testing;  // NOLINT

struct InstrumentsLookup : Test {
  InstrumentDescriptor descriptor;
  Container container;

  auto SetUp() -> void override {
    container.emplace([] {
      Instrument instrument;
      instrument.identifier = InstrumentId{1};
      instrument.symbol = Symbol{"unique-symbol"};
      return instrument;
    }());
    container.emplace([] {
      Instrument instrument;
      instrument.identifier = InstrumentId{2};
      instrument.symbol = Symbol{"ambiguous-symbol"};
      return instrument;
    }());
    container.emplace([] {
      Instrument instrument;
      instrument.identifier = InstrumentId{3};
      instrument.symbol = Symbol{"ambiguous-symbol"};
      return instrument;
    }());
  }
};

TEST_F(InstrumentsLookup, CreatesWithSymbolStrategy) {
  descriptor.symbol = Symbol{"AAPL"};

  auto lookup = Lookup::create(descriptor);

  ASSERT_TRUE(lookup.has_value());
  EXPECT_TRUE(lookup->uses<SymbolLookup>());
}

TEST_F(InstrumentsLookup, CreatesWithCusipStrategy) {
  descriptor.security_id = SecurityId{"123456789"};
  descriptor.security_id_source = SecurityIdSource::Option::Cusip;

  auto lookup = Lookup::create(descriptor);

  ASSERT_TRUE(lookup.has_value());
  EXPECT_TRUE(lookup->uses<CusipIdLookup>());
}

TEST_F(InstrumentsLookup, CreatesWithSedolStrategy) {
  descriptor.security_id = SecurityId{"123456789"};
  descriptor.security_id_source = SecurityIdSource::Option::Sedol;

  auto lookup = Lookup::create(descriptor);

  ASSERT_TRUE(lookup.has_value());
  EXPECT_TRUE(lookup->uses<SedolIdLookup>());
}

TEST_F(InstrumentsLookup, CreatesWithIsinStrategy) {
  descriptor.security_id = SecurityId{"123456789"};
  descriptor.security_id_source = SecurityIdSource::Option::Isin;
  descriptor.security_type = SecurityType::Option::CommonStock;
  descriptor.currency = Currency{"USD"};
  descriptor.security_exchange = SecurityExchange{"XNYS"};

  auto lookup = Lookup::create(descriptor);

  ASSERT_TRUE(lookup.has_value());
  EXPECT_TRUE(lookup->uses<IsinIdLookup>());
}

TEST_F(InstrumentsLookup, CreatesWithRicStrategy) {
  descriptor.security_id = SecurityId{"123456789"};
  descriptor.security_id_source = SecurityIdSource::Option::Ric;

  auto lookup = Lookup::create(descriptor);

  ASSERT_TRUE(lookup.has_value());
  EXPECT_TRUE(lookup->uses<RicIdLookup>());
}

TEST_F(InstrumentsLookup, CreatesWithExchangeStrategy) {
  descriptor.security_id = SecurityId{"123456789"};
  descriptor.security_id_source = SecurityIdSource::Option::ExchangeSymbol;

  auto lookup = Lookup::create(descriptor);

  ASSERT_TRUE(lookup.has_value());
  EXPECT_TRUE(lookup->uses<ExchangeIdLookup>());
}

TEST_F(InstrumentsLookup, CreatesWithBloombergStrategy) {
  descriptor.security_id = SecurityId{"123456789"};
  descriptor.security_id_source = SecurityIdSource::Option::BloombergSymbol;

  auto lookup = Lookup::create(descriptor);

  ASSERT_TRUE(lookup.has_value());
  EXPECT_TRUE(lookup->uses<BloombergIdLookup>());
}

TEST_F(InstrumentsLookup, ReportsErrorForUnknwonSecurityIdSource) {
  descriptor.security_id = SecurityId{"123456789"};
  descriptor.security_id_source = static_cast<SecurityIdSource::Option>(0xFF);

  auto lookup = Lookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsLookup, ReportsErrorForEmptyDescriptor) {
  auto lookup = Lookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsLookup, FindsUniqueInstrument) {
  descriptor.symbol = Symbol{"unique-symbol"};
  const auto lookup = Lookup::create(descriptor);
  ASSERT_TRUE(lookup.has_value());

  const auto view = (*lookup)(container);

  ASSERT_TRUE(view.has_value());
  EXPECT_EQ(view->instrument().identifier, InstrumentId{1});
}

TEST_F(InstrumentsLookup, ReportsInstrumentNotFound) {
  descriptor.symbol = Symbol{"nonexistent-symbol"};
  const auto lookup = Lookup::create(descriptor);
  ASSERT_TRUE(lookup.has_value());

  const auto view = (*lookup)(container);

  ASSERT_FALSE(view.has_value());
  EXPECT_EQ(view.error(), LookupError::InstrumentNotFound);
}

TEST_F(InstrumentsLookup, ReportsDescriptorAmbiguous) {
  descriptor.symbol = Symbol{"ambiguous-symbol"};
  const auto lookup = Lookup::create(descriptor);
  ASSERT_TRUE(lookup.has_value());

  const auto view = (*lookup)(container);

  ASSERT_FALSE(view.has_value());
  EXPECT_EQ(view.error(), LookupError::AmbiguousInstrumentDescriptor);
}

}  // namespace
}  // namespace simulator::trading_system::instrument::lookup::tests