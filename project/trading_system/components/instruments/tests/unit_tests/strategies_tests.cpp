#include <gtest/gtest.h>

#include "common/instrument.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "ih/lookup/match_rate.hpp"
#include "ih/lookup/strategies.hpp"

namespace simulator::trading_system::instrument::lookup::test {
namespace {

struct InstrumentLookupFixture : testing::Test {
  InstrumentDescriptor descriptor;
  Instrument instrument;
  Party party{PartyId{"PARTY"},
              PartyIdSource::Option::Proprietary,
              PartyRole::Option::LiquidityProvider};
};

/*----------------------------------------------------------------------------*/

struct InstrumentsSymbolLookup : InstrumentLookupFixture {
  void SetUp() override {
    descriptor.symbol = Symbol{"AAPL"};
    instrument.symbol = Symbol{"AAPL"};
  }
};

TEST_F(InstrumentsSymbolLookup, CreateFailsWhenSymbolMissing) {
  descriptor.symbol = std::nullopt;

  const auto lookup = SymbolLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsSymbolLookup, CreatesWhenSymbolPresent) {
  const auto lookup = SymbolLookup::create(descriptor);

  ASSERT_TRUE(lookup.has_value());
}

TEST_F(InstrumentsSymbolLookup, MatchesSymbol) {
  instrument.symbol = Symbol{"TSLA"};
  descriptor.symbol = Symbol{"AAPL"};
  const auto lookup = SymbolLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsSymbolLookup, MatchesSecurityType) {
  instrument.security_type = SecurityType::Option::Future;
  descriptor.security_type = SecurityType::Option::CommonStock;
  const auto lookup = SymbolLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsSymbolLookup, MatchesCurrency) {
  instrument.security_type = SecurityType::Option::CommonStock;
  instrument.price_currency = PriceCurrency{"EUR"};
  descriptor.currency = Currency{"USD"};
  const auto lookup = SymbolLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsSymbolLookup, MatchesSecurityExchange) {
  descriptor.security_exchange = SecurityExchange{"XNSE"};
  instrument.security_exchange = SecurityExchange{"XLON"};
  const auto lookup = SymbolLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsSymbolLookup, MatchesParty) {
  instrument.party_id = party.party_id();
  instrument.party_role = party.role();
  descriptor.parties = {party};
  const auto lookup = SymbolLookup::create(descriptor);

  const auto match_rate = (*lookup)(instrument);

  EXPECT_EQ(match_rate, MatchRate(2U));
}

/*----------------------------------------------------------------------------*/

struct InstrumentsSedolIdLookup : InstrumentLookupFixture {
  void SetUp() override {
    instrument.sedol = SedolId{"SEDOL"};
    descriptor.security_id = SecurityId{"SEDOL"};
    descriptor.security_id_source = SecurityIdSource::Option::Sedol;
  }
};

TEST_F(InstrumentsSedolIdLookup, CreateFailsWhenIdentifierSourceMissing) {
  descriptor.security_id_source = std::nullopt;

  const auto lookup = SedolIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsSedolIdLookup, CreateFailsWhenIdentifierMissing) {
  descriptor.security_id = std::nullopt;

  const auto lookup = SedolIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsSedolIdLookup, CreateFailsWhenIdentifierSourceNotSedol) {
  descriptor.security_id_source = SecurityIdSource::Option::Cusip;

  const auto lookup = SedolIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsSedolIdLookup, CreatesWhenIdentifierPresent) {
  ASSERT_TRUE(SedolIdLookup::create(descriptor).has_value());
}

TEST_F(InstrumentsSedolIdLookup, MatchesSedolId) {
  instrument.sedol = SedolId{"OTHER_SEDOL"};
  const auto lookup = SedolIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsSedolIdLookup, MatchesSecurityType) {
  instrument.security_type = SecurityType::Option::CommonStock;
  descriptor.security_type = SecurityType::Option::Future;
  const auto lookup = SedolIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsSedolIdLookup, MatchesCurrency) {
  instrument.security_type = SecurityType::Option::FxSwap;
  instrument.base_currency = BaseCurrency{"USD"};
  descriptor.currency = Currency{"EUR"};
  const auto lookup = SedolIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsSedolIdLookup, MatchesSecurityExchange) {
  instrument.security_exchange = SecurityExchange{"XLON"};
  descriptor.security_exchange = SecurityExchange{"XNSE"};
  const auto lookup = SedolIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsSedolIdLookup, MatchesSymbol) {
  instrument.symbol = Symbol{"TSLA"};
  descriptor.symbol = Symbol{"AAPL"};
  const auto lookup = SedolIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsSedolIdLookup, MatchesParty) {
  instrument.party_id = party.party_id();
  instrument.party_role = party.role();
  descriptor.parties = {party};
  const auto lookup = SedolIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate(2U));
}

/*----------------------------------------------------------------------------*/

struct InstrumentsCusipIdLookup : public InstrumentLookupFixture {
  void SetUp() override {
    instrument.cusip = CusipId{"CUSIP"};
    descriptor.security_id = SecurityId{"CUSIP"};
    descriptor.security_id_source = SecurityIdSource::Option::Cusip;
  }
};

TEST_F(InstrumentsCusipIdLookup, CreateFailsWhenIdentifierMissing) {
  descriptor.security_id = std::nullopt;

  const auto lookup = CusipIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsCusipIdLookup, CreateFailsWhenIdentifierSourceMissing) {
  descriptor.security_id_source = std::nullopt;

  const auto lookup = CusipIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsCusipIdLookup, CreateFailsWhenIdentifierSourceNotCusip) {
  descriptor.security_id_source = SecurityIdSource::Option::Sedol;

  const auto lookup = CusipIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsCusipIdLookup, CreatesWhenIdentifierPresent) {
  const auto lookup = CusipIdLookup::create(descriptor);

  ASSERT_TRUE(lookup.has_value());
}

TEST_F(InstrumentsCusipIdLookup, MatchesCusipId) {
  instrument.cusip = CusipId{"OTHER_CUSIP"};
  const auto lookup = CusipIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsCusipIdLookup, MatchesSecurityType) {
  instrument.security_type = SecurityType::Option::CommonStock;
  descriptor.security_type = SecurityType::Option::Future;
  const auto lookup = CusipIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsCusipIdLookup, MatchesCurrency) {
  instrument.security_type = SecurityType::Option::FxSwap;
  instrument.base_currency = BaseCurrency{"EUR"};
  descriptor.currency = Currency{"USD"};
  const auto lookup = CusipIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsCusipIdLookup, MatchesSecurityExchange) {
  instrument.security_exchange = SecurityExchange{"XLON"};
  descriptor.security_exchange = SecurityExchange{"XNSE"};
  const auto lookup = CusipIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsCusipIdLookup, MatchesSymbol) {
  instrument.symbol = Symbol{"AAPL"};
  descriptor.symbol = Symbol{"TSLA"};
  const auto lookup = CusipIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsCusipIdLookup, MatchesParty) {
  instrument.party_id = party.party_id();
  instrument.party_role = party.role();
  descriptor.parties = {party};
  const auto lookup = CusipIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate(2U));
}

/*----------------------------------------------------------------------------*/

struct InstrumentsIsinIdLookup : InstrumentLookupFixture {
  void SetUp() override {
    instrument.isin = IsinId{"ISIN"};
    instrument.price_currency = PriceCurrency{"USD"};
    instrument.security_type = SecurityType::Option::CommonStock;
    instrument.security_exchange = SecurityExchange{"XNSE"};

    descriptor.security_id = SecurityId{"ISIN"};
    descriptor.security_id_source = SecurityIdSource::Option::Isin;
    descriptor.currency = Currency{"USD"};
    descriptor.security_type = SecurityType::Option::CommonStock;
    descriptor.security_exchange = SecurityExchange{"XNSE"};
  }
};

TEST_F(InstrumentsIsinIdLookup, CreateFailsWhenIdentifierMissing) {
  descriptor.security_id = std::nullopt;

  const auto lookup = IsinIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsIsinIdLookup, CreateFailsWhenIdentifierSourceMissing) {
  descriptor.security_id_source = std::nullopt;

  const auto lookup = IsinIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsIsinIdLookup, CreateFailsWhenIdentifierNotIsin) {
  descriptor.security_id_source = SecurityIdSource::Option::Sedol;

  const auto lookup = IsinIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsIsinIdLookup, CreateFailsWhenSecurityTypeMissing) {
  descriptor.security_type = std::nullopt;

  const auto lookup = IsinIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsIsinIdLookup, CreateFailsWhenCurrencyMissing) {
  descriptor.currency = std::nullopt;

  const auto lookup = IsinIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsIsinIdLookup, CreateFailsWhenSecurityExchangeMissing) {
  descriptor.security_exchange = std::nullopt;

  const auto lookup = IsinIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsIsinIdLookup, CreatesWithValidDescriptor) {
  ASSERT_TRUE(IsinIdLookup::create(descriptor).has_value());
}

TEST_F(InstrumentsIsinIdLookup, MatchesIsinId) {
  instrument.isin = IsinId{"OTHER_ISIN"};
  const auto lookup = IsinIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsIsinIdLookup, MatchesSecurityType) {
  instrument.security_type = SecurityType::Option::Future;
  descriptor.security_type = SecurityType::Option::Option;
  const auto lookup = IsinIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsIsinIdLookup, MatchesCurrency) {
  instrument.security_type = SecurityType::Option::Future;
  instrument.price_currency = PriceCurrency{"EUR"};
  descriptor.currency = Currency{"USD"};
  const auto lookup = IsinIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsIsinIdLookup, MatchesSecurityExchange) {
  instrument.security_exchange = SecurityExchange{"XNYS"};
  descriptor.security_exchange = SecurityExchange{"XNSE"};
  const auto lookup = IsinIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsIsinIdLookup, MatchesSymbol) {
  instrument.symbol = Symbol{"TSLA"};
  descriptor.symbol = Symbol{"AAPL"};
  const auto lookup = IsinIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsIsinIdLookup, MatchesParty) {
  instrument.party_id = party.party_id();
  instrument.party_role = party.role();
  descriptor.parties = {party};
  const auto lookup = IsinIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate(5U));
}

/*----------------------------------------------------------------------------*/

struct InstrumentsRicIdLookup : InstrumentLookupFixture {
  void SetUp() override {
    instrument.ric = RicId{"RIC"};
    descriptor.security_id = SecurityId{"RIC"};
    descriptor.security_id_source = SecurityIdSource::Option::Ric;
  }
};

TEST_F(InstrumentsRicIdLookup, CreateFailsWhenIdentifierMissing) {
  descriptor.security_id = std::nullopt;

  const auto lookup = RicIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsRicIdLookup, CreateFailsWhenIdentifierSourceMissing) {
  descriptor.security_id_source = std::nullopt;

  const auto lookup = RicIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsRicIdLookup, CreateFailsWhenIdentifierSoruceNotRic) {
  descriptor.security_id_source = SecurityIdSource::Option::Sedol;

  const auto lookup = RicIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsRicIdLookup, CreatesWhenIdentifierValid) {
  ASSERT_TRUE(RicIdLookup::create(descriptor).has_value());
}

TEST_F(InstrumentsRicIdLookup, MatchesRicId) {
  instrument.ric = RicId{"OTHER_RIC"};
  const auto lookup = RicIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsRicIdLookup, MatchesSecurityType) {
  instrument.security_type = SecurityType::Option::CommonStock;
  descriptor.security_type = SecurityType::Option::Option;
  const auto lookup = RicIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsRicIdLookup, MatchesCurrency) {
  instrument.security_type = SecurityType::Option::Future;
  instrument.price_currency = PriceCurrency{"EUR"};
  descriptor.currency = Currency{"USD"};
  const auto lookup = RicIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsRicIdLookup, MatchesSecurityExchange) {
  instrument.security_exchange = SecurityExchange{"XLON"};
  descriptor.security_exchange = SecurityExchange{"XNSE"};
  const auto lookup = RicIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsRicIdLookup, MatchesSymbol) {
  instrument.symbol = Symbol{"TSLA"};
  descriptor.symbol = Symbol{"AAPL"};
  const auto lookup = RicIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsRicIdLookup, MatchesParty) {
  instrument.party_id = party.party_id();
  instrument.party_role = party.role();
  descriptor.parties = {party};
  const auto lookup = RicIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate(2U));
}

/*----------------------------------------------------------------------------*/

struct InstrumentsExchangeIdLookup : InstrumentLookupFixture {
  void SetUp() override {
    instrument.exchange_id = ExchangeId{"EXCHANGE"};
    descriptor.security_id = SecurityId{"EXCHANGE"};
    descriptor.security_id_source = SecurityIdSource::Option::ExchangeSymbol;
  }
};

TEST_F(InstrumentsExchangeIdLookup, CreateFailsWhenIdMissing) {
  descriptor.security_id = std::nullopt;

  const auto lookup = ExchangeIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsExchangeIdLookup, CreateFailsWhenIdSourceMissing) {
  descriptor.security_id_source = std::nullopt;

  const auto lookup = ExchangeIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsExchangeIdLookup, CreateFailsWhenIdSoruceNotExchange) {
  descriptor.security_id_source = SecurityIdSource::Option::Sedol;

  const auto lookup = ExchangeIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsExchangeIdLookup, CreatesWhenIdValid) {
  ASSERT_TRUE(ExchangeIdLookup::create(descriptor).has_value());
}

TEST_F(InstrumentsExchangeIdLookup, MatchesExchangeId) {
  instrument.exchange_id = ExchangeId{"OTHER_EXCHANGE_ID"};
  const auto lookup = ExchangeIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsExchangeIdLookup, MatchesSecurityType) {
  instrument.security_type = SecurityType::Option::CommonStock;
  descriptor.security_type = SecurityType::Option::Future;
  const auto lookup = ExchangeIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsExchangeIdLookup, MatchesCurrency) {
  instrument.security_type = SecurityType::Option::Option;
  instrument.price_currency = PriceCurrency{"EUR"};
  descriptor.currency = Currency{"USD"};
  const auto lookup = ExchangeIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsExchangeIdLookup, MatchesSecurityExchange) {
  instrument.security_exchange = SecurityExchange{"XLON"};
  descriptor.security_exchange = SecurityExchange{"XNSE"};
  const auto lookup = ExchangeIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsExchangeIdLookup, MatchesSymbol) {
  instrument.symbol = Symbol{"TSLA"};
  descriptor.symbol = Symbol{"AAPL"};
  const auto lookup = ExchangeIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsExchangeIdLookup, MatchesParty) {
  instrument.party_id = party.party_id();
  instrument.party_role = party.role();
  descriptor.parties = {party};
  const auto lookup = ExchangeIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate(2U));
}

/*----------------------------------------------------------------------------*/

struct InstrumentsBloombergIdLookup : InstrumentLookupFixture {
  void SetUp() override {
    instrument.bloomberg_id = BloombergId{"BBG"};
    descriptor.security_id = SecurityId{"BBG"};
    descriptor.security_id_source = SecurityIdSource::Option::BloombergSymbol;
  }
};

TEST_F(InstrumentsBloombergIdLookup, CreateFailsWhenIdMissing) {
  descriptor.security_id = std::nullopt;

  const auto lookup = BloombergIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsBloombergIdLookup, CreateFailsWhenIdSourceMissing) {
  descriptor.security_id_source = std::nullopt;

  const auto lookup = BloombergIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsBloombergIdLookup, CreateFailsWhenIdSourceNotBloomberg) {
  descriptor.security_id_source = SecurityIdSource::Option::Sedol;

  const auto lookup = BloombergIdLookup::create(descriptor);

  ASSERT_FALSE(lookup.has_value());
  EXPECT_EQ(lookup.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(InstrumentsBloombergIdLookup, CreatesWhenIdentifierValid) {
  ASSERT_TRUE(BloombergIdLookup::create(descriptor).has_value());
}

TEST_F(InstrumentsBloombergIdLookup, MatchesBloombergId) {
  instrument.bloomberg_id = BloombergId{"OTHER_BBG_ID"};
  const auto lookup = BloombergIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsBloombergIdLookup, MatchesSecurityType) {
  instrument.security_type = SecurityType::Option::CommonStock;
  descriptor.security_type = SecurityType::Option::Option;
  const auto lookup = BloombergIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsBloombergIdLookup, MatchesCurrency) {
  instrument.security_type = SecurityType::Option::Future;
  instrument.price_currency = PriceCurrency{"USD"};
  descriptor.currency = Currency{"AUD"};
  const auto lookup = BloombergIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsBloombergIdLookup, MatchesSecurityExchange) {
  instrument.security_exchange = SecurityExchange{"XLON"};
  descriptor.security_exchange = SecurityExchange{"XNSE"};
  const auto lookup = BloombergIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsBloombergIdLookup, MatchesSymbol) {
  descriptor.symbol = Symbol{"TSLA"};
  instrument.symbol = Symbol{"AAPL"};
  const auto lookup = BloombergIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate::NoMatch);
}

TEST_F(InstrumentsBloombergIdLookup, MatchesParty) {
  instrument.party_id = party.party_id();
  instrument.party_role = party.role();
  descriptor.parties = {party};
  const auto lookup = BloombergIdLookup::create(descriptor);

  const auto match = (*lookup)(instrument);

  EXPECT_EQ(match, MatchRate(2U));
}

/*----------------------------------------------------------------------------*/

}  // namespace
}  // namespace simulator::trading_system::instrument::lookup::test