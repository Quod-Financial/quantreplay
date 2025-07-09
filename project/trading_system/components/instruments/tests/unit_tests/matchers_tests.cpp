#include <gtest/gtest.h>

#include "ih/lookup/match_rate.hpp"
#include "ih/lookup/matchers.hpp"

namespace simulator::trading_system::instrument::lookup::test {
namespace {

struct InstrumentMatcherTest : testing::Test {
  InstrumentDescriptor descriptor;
  Instrument instrument;
};

/*----------------------------------------------------------------------------*/

struct InstrumentSymbolMatcher : InstrumentMatcherTest {
  auto SetUp() -> void override {
    descriptor.symbol = Symbol{"AAPL"};
    instrument.symbol = Symbol{"AAPL"};
  }
};

TEST_F(InstrumentSymbolMatcher, MatchWhenValuesEqual) {
  EXPECT_EQ(match_symbol(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentSymbolMatcher, NoMatchWhenInstrumentValueMissing) {
  instrument.symbol = std::nullopt;

  EXPECT_EQ(match_symbol(descriptor, instrument), MatchRate::NoMatch);
}

TEST_F(InstrumentSymbolMatcher, UnmatchableWhenDescriptorValueMissing) {
  descriptor.symbol = std::nullopt;

  EXPECT_EQ(match_symbol(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentSymbolMatcher, NoMatchWhenValuesDiffer) {
  instrument.symbol = Symbol{"GOOG"};

  EXPECT_EQ(match_symbol(descriptor, instrument), MatchRate::NoMatch);
}

/*----------------------------------------------------------------------------*/

struct InstrumentSedolMatcher : InstrumentMatcherTest {
  auto SetUp() -> void override {
    instrument.sedol = SedolId{"SEDOL"};
    descriptor.security_id = SecurityId{"SEDOL"};
  }
};

TEST_F(InstrumentSedolMatcher, MatchWhenValuesEqual) {
  EXPECT_EQ(match_sedol_id(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentSedolMatcher, NoMatchWhenInstrumentValueMissing) {
  instrument.sedol = std::nullopt;

  EXPECT_EQ(match_sedol_id(descriptor, instrument), MatchRate::NoMatch);
}

TEST_F(InstrumentSedolMatcher, UnmatchableWhenDescriptorSecurityIdMissing) {
  descriptor.security_id = std::nullopt;

  EXPECT_EQ(match_sedol_id(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentSedolMatcher, NoMatchWhenValuesDiffer) {
  instrument.sedol = SedolId{"DIFFER"};

  EXPECT_EQ(match_sedol_id(descriptor, instrument), MatchRate::NoMatch);
}

/*----------------------------------------------------------------------------*/

struct InstrumentCusipMatcher : InstrumentMatcherTest {
  auto SetUp() -> void override {
    instrument.cusip = CusipId{"CUSIP"};
    descriptor.security_id = SecurityId{"CUSIP"};
  }
};

TEST_F(InstrumentCusipMatcher, MatchWhenValuesEqual) {
  EXPECT_EQ(match_cusip_id(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentCusipMatcher, NoMatchWhenInstrumentValueMissing) {
  instrument.cusip = std::nullopt;

  EXPECT_EQ(match_cusip_id(descriptor, instrument), MatchRate::NoMatch);
}

TEST_F(InstrumentCusipMatcher, UnmatchableWhenDescriptorValueMissing) {
  descriptor.security_id = std::nullopt;

  EXPECT_EQ(match_cusip_id(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentCusipMatcher, NoMatchWhenValuesDiffer) {
  instrument.cusip = CusipId{"DIFFER"};

  EXPECT_EQ(match_cusip_id(descriptor, instrument), MatchRate::NoMatch);
}

/*----------------------------------------------------------------------------*/

struct InstrumentIsinMatcher : InstrumentMatcherTest {
  auto SetUp() -> void override {
    instrument.isin = IsinId{"ISIN"};
    descriptor.security_id = SecurityId{"ISIN"};
  }
};

TEST_F(InstrumentIsinMatcher, MatchWhenValuesEqual) {
  EXPECT_EQ(match_isin_id(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentIsinMatcher, NoMatchWhenInstrumentValueMissing) {
  instrument.isin = std::nullopt;

  EXPECT_EQ(match_isin_id(descriptor, instrument), MatchRate::NoMatch);
}

TEST_F(InstrumentIsinMatcher, UnmatchableWhenDescriptorValueMissing) {
  descriptor.security_id = std::nullopt;

  EXPECT_EQ(match_isin_id(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentIsinMatcher, NoMatchWhenValuesDiffer) {
  instrument.isin = IsinId{"DIFFER"};

  EXPECT_EQ(match_isin_id(descriptor, instrument), MatchRate::NoMatch);
}

/*----------------------------------------------------------------------------*/

struct InstrumentRicMatcher : InstrumentMatcherTest {
  auto SetUp() -> void override {
    instrument.ric = RicId{"RIC"};
    descriptor.security_id = SecurityId{"RIC"};
  }
};

TEST_F(InstrumentRicMatcher, MatchWhenValuesEqual) {
  EXPECT_EQ(match_ric_id(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentRicMatcher, NoMatchWhenInstrumentValueMissing) {
  instrument.ric = std::nullopt;

  EXPECT_EQ(match_ric_id(descriptor, instrument), MatchRate::NoMatch);
}

TEST_F(InstrumentRicMatcher, UnmatchableWhenDescriptorValueMissing) {
  descriptor.security_id = std::nullopt;

  EXPECT_EQ(match_ric_id(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentRicMatcher, NoMatchWhenValuesDiffer) {
  instrument.ric = RicId{"DIFFER"};

  EXPECT_EQ(match_ric_id(descriptor, instrument), MatchRate::NoMatch);
}

/*----------------------------------------------------------------------------*/

struct InstrumentExchangeIdMatcher : InstrumentMatcherTest {
  auto SetUp() -> void override {
    instrument.exchange_id = ExchangeId{"EXCH"};
    descriptor.security_id = SecurityId{"EXCH"};
  }
};

TEST_F(InstrumentExchangeIdMatcher, MatchWhenValuesEqual) {
  EXPECT_EQ(match_exchange_id(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentExchangeIdMatcher, NoMatchWhenInstrumentValueMissing) {
  instrument.exchange_id = std::nullopt;

  EXPECT_EQ(match_exchange_id(descriptor, instrument), MatchRate::NoMatch);
}

TEST_F(InstrumentExchangeIdMatcher, UnmatchableWhenDescriptorValueMissing) {
  descriptor.security_id = std::nullopt;

  EXPECT_EQ(match_exchange_id(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentExchangeIdMatcher, NoMatchWhenValuesDiffer) {
  instrument.exchange_id = ExchangeId{"DIFFER"};

  EXPECT_EQ(match_exchange_id(descriptor, instrument), MatchRate::NoMatch);
}

/*----------------------------------------------------------------------------*/

struct InstrumentBloombergIdMatcher : InstrumentMatcherTest {
  auto SetUp() -> void override {
    instrument.bloomberg_id = BloombergId{"BBG"};
    descriptor.security_id = SecurityId{"BBG"};
  }
};

TEST_F(InstrumentBloombergIdMatcher, MatchWhenValuesEqual) {
  EXPECT_EQ(match_bloomberg_id(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentBloombergIdMatcher, NoMatchWhenInstrumentValueMissing) {
  instrument.bloomberg_id = std::nullopt;

  EXPECT_EQ(match_bloomberg_id(descriptor, instrument), MatchRate::NoMatch);
}

TEST_F(InstrumentBloombergIdMatcher, UnmatchableWhenDescriptorValueMissing) {
  descriptor.security_id = std::nullopt;

  EXPECT_EQ(match_bloomberg_id(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentBloombergIdMatcher, NoMatchWhenValuesDiffer) {
  instrument.bloomberg_id = BloombergId{"DIFFER"};

  EXPECT_EQ(match_bloomberg_id(descriptor, instrument), MatchRate::NoMatch);
}

/*----------------------------------------------------------------------------*/

struct InstrumentSecurityTypeMatcher : InstrumentMatcherTest {
  auto SetUp() -> void override {
    descriptor.security_type = SecurityType::Option::CommonStock;
    instrument.security_type = SecurityType::Option::CommonStock;
  }
};

TEST_F(InstrumentSecurityTypeMatcher, MatchWhenValuesEqual) {
  EXPECT_EQ(match_security_type(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentSecurityTypeMatcher, NoMatchWhenInstrumentValueMissing) {
  instrument.security_type = std::nullopt;

  EXPECT_EQ(match_security_type(descriptor, instrument), MatchRate::NoMatch);
}

TEST_F(InstrumentSecurityTypeMatcher, UnmatchableWhenDescriptorValueMissing) {
  descriptor.security_type = std::nullopt;

  EXPECT_EQ(match_security_type(descriptor, instrument),
            MatchRate::Unmatchable);
}

TEST_F(InstrumentSecurityTypeMatcher, NoMatchWhenValuesDiffer) {
  instrument.security_type = SecurityType::Option::Future;

  EXPECT_EQ(match_security_type(descriptor, instrument), MatchRate::NoMatch);
}

/*----------------------------------------------------------------------------*/

struct InstrumentSecurityExchangeMatcher : InstrumentMatcherTest {
  auto SetUp() -> void override {
    descriptor.security_exchange = SecurityExchange{"XLSE"};
    instrument.security_exchange = SecurityExchange{"XLSE"};
  }
};

TEST_F(InstrumentSecurityExchangeMatcher, MatchWhenValuesEqual) {
  EXPECT_EQ(match_security_exchange(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentSecurityExchangeMatcher, NoMatchWhenInstrumentValueMissing) {
  instrument.security_exchange = std::nullopt;

  EXPECT_EQ(match_security_exchange(descriptor, instrument),
            MatchRate::NoMatch);
}

TEST_F(InstrumentSecurityExchangeMatcher,
       UnmatchableWhenDescriptorValueMissing) {
  descriptor.security_exchange = std::nullopt;

  EXPECT_EQ(match_security_exchange(descriptor, instrument),
            MatchRate::Unmatchable);
}

TEST_F(InstrumentSecurityExchangeMatcher, NoMatchWhenValuesDiffer) {
  instrument.security_exchange = SecurityExchange{"XNAS"};

  EXPECT_EQ(match_security_exchange(descriptor, instrument),
            MatchRate::NoMatch);
}

/*----------------------------------------------------------------------------*/

struct InstrumentCurrencyMatcher : InstrumentMatcherTest {
  auto SetUp() -> void override {
    descriptor.currency = Currency{"USD"};
    instrument.base_currency = BaseCurrency{"USD"};
    instrument.price_currency = PriceCurrency{"USD"};
  }
};

TEST_F(InstrumentCurrencyMatcher, MatchWhenPriceCurrencyEqualForEq) {
  instrument.security_type = SecurityType::Option::Future;

  EXPECT_EQ(match_currency(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentCurrencyMatcher, MatchWhenBaseCurrencyEqualForFx) {
  instrument.security_type = SecurityType::Option::FxSpot;

  EXPECT_EQ(match_currency(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentCurrencyMatcher, UnmatchableWhenDescriptorValueMissing) {
  descriptor.currency = std::nullopt;

  EXPECT_EQ(match_currency(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentCurrencyMatcher, NoMatchWhenCurrencyCategoryUnkown) {
  instrument.security_type = std::nullopt;

  EXPECT_EQ(match_currency(descriptor, instrument), MatchRate::NoMatch);
}

TEST_F(InstrumentCurrencyMatcher, NoMatchWhenPriceCurrencyDifferForEq) {
  instrument.security_type = SecurityType::Option::Future;
  instrument.price_currency = PriceCurrency{"EUR"};

  EXPECT_EQ(match_currency(descriptor, instrument), MatchRate::NoMatch);
}

TEST_F(InstrumentCurrencyMatcher, NoMatchWhenBaseCurrencyDifferForFx) {
  instrument.security_type = SecurityType::Option::FxSpot;
  instrument.base_currency = BaseCurrency{"EUR"};

  EXPECT_EQ(match_currency(descriptor, instrument), MatchRate::NoMatch);
}

/*----------------------------------------------------------------------------*/

struct InstrumentPartyIdMatcher : InstrumentMatcherTest {
  auto SetUp() -> void override {
    instrument.party_id = PartyId{"PARTY"};
    instrument.party_role = PartyRole::Option::LiquidityProvider;

    descriptor.parties = {{PartyId{"PARTY"},
                           PartyIdSource::Option::Proprietary,
                           PartyRole::Option::LiquidityProvider}};
  };
};

TEST_F(InstrumentPartyIdMatcher, MatchWhenPartySpecifiedInInstrument) {
  EXPECT_EQ(match_party(descriptor, instrument), MatchRate::Match);
}

TEST_F(InstrumentPartyIdMatcher, UnmatchableWhenInstrumentPartyIdMissing) {
  instrument.party_id = std::nullopt;

  EXPECT_EQ(match_party(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentPartyIdMatcher, UnmatchableWhenInstrumentPartyRoleMissing) {
  instrument.party_role = std::nullopt;

  EXPECT_EQ(match_party(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentPartyIdMatcher, UnmatchableWhenPartyNotInDescriptor) {
  descriptor.parties = {};

  EXPECT_EQ(match_party(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentPartyIdMatcher, UnmatchableWhenPartyIdDifferent) {
  instrument.party_id = PartyId{"DIFFER"};

  EXPECT_EQ(match_party(descriptor, instrument), MatchRate::Unmatchable);
}

TEST_F(InstrumentPartyIdMatcher, UnmatchableWhenPartyRoleDifferent) {
  instrument.party_role = PartyRole::Option::ClearingFirm;

  EXPECT_EQ(match_party(descriptor, instrument), MatchRate::Unmatchable);
}

/*----------------------------------------------------------------------------*/

TEST(InstrumentsMatcher, CombinesMatchersResults) {
  constexpr auto matcher =
      make_matcher([](auto&&, auto&&) { return MatchRate::Match; },
                   [](auto&&, auto&&) { return MatchRate::Match; },
                   [](auto&&, auto&&) { return MatchRate::NoMatch; });

  EXPECT_EQ(matcher(InstrumentDescriptor{}, Instrument{}), MatchRate::NoMatch);
}

/*----------------------------------------------------------------------------*/

}  // namespace
}  // namespace simulator::trading_system::instrument::lookup::test