#include <gtest/gtest.h>

#include "common/instrument.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "instruments/matcher.hpp"

namespace simulator::trading_system::instrument::test {
namespace {

TEST(InstrumentMatcher, MatchesInstrumentDescriptorUsingSymbolStrategy) {
  Instrument instrument;
  instrument.symbol = Symbol{"Symbol"};
  InstrumentDescriptor descriptor;
  descriptor.symbol = Symbol{"Symbol"};

  const auto matcher = Matcher::create(instrument);
  const auto result = matcher(descriptor);
  ASSERT_TRUE(result.has_value());
}

TEST(InstrumentMatcher, ReturnsInstrumentNotFoundWhenSymbolStrategyMatchFails) {
  Instrument instrument;
  instrument.symbol = Symbol{"Symbol"};
  InstrumentDescriptor descriptor;
  descriptor.symbol = Symbol{"AnotherSymbol"};

  const auto matcher = Matcher::create(instrument);
  const auto result = matcher(descriptor);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(result.error(), LookupError::InstrumentNotFound);
}

}  // namespace
}  // namespace simulator::trading_system::instrument::test
