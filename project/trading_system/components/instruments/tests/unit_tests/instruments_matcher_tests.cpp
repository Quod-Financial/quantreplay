#include <gtest/gtest.h>

#include "common/instrument.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "ih/instruments_matcher.hpp"

namespace simulator::trading_system::instrument::tests {
namespace {

struct TradingSystemInstrumentsMatcher : testing::Test {
  InstrumentDescriptor descriptor;
  Instrument instrument;
};

TEST_F(
    TradingSystemInstrumentsMatcher,
    ReturnsMalformedInstrumentDescriptorWhenSecurityIdSourceAndSymbolAreMissing) {
  descriptor.security_id_source = std::nullopt;
  descriptor.symbol = std::nullopt;

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(result.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(TradingSystemInstrumentsMatcher,
       MatchesInstrumentDescriptorUsingSymbolStrategy) {
  instrument.symbol = Symbol{"Symbol"};
  descriptor.symbol = Symbol{"Symbol"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_TRUE(result.has_value());
}

TEST_F(TradingSystemInstrumentsMatcher,
       ReturnsInstrumentNotFoundWhenSymbolStrategyMatchFails) {
  instrument.symbol = Symbol{"Symbol"};
  descriptor.symbol = Symbol{"AnotherSymbol"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(result.error(), LookupError::InstrumentNotFound);
}

TEST_F(TradingSystemInstrumentsMatcher,
       MatchesInstrumentDescriptorUsingSedolStrategy) {
  instrument.sedol = SedolId{"Sedol123"};
  descriptor.security_id_source = SecurityIdSource::Option::Sedol;
  descriptor.security_id = SecurityId{"Sedol123"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_TRUE(result.has_value());
}

TEST_F(TradingSystemInstrumentsMatcher,
       ReturnsInstrumentNotFoundWhenSedolStrategyMatchFails) {
  instrument.sedol = SedolId{"Sedol123"};
  descriptor.security_id_source = SecurityIdSource::Option::Sedol;
  descriptor.security_id = SecurityId{"AnotherSedol"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(result.error(), LookupError::InstrumentNotFound);
}

TEST_F(TradingSystemInstrumentsMatcher,
       MatchesInstrumentDescriptorUsingCusipStrategy) {
  instrument.cusip = CusipId{"Cusip123"};
  descriptor.security_id_source = SecurityIdSource::Option::Cusip;
  descriptor.security_id = SecurityId{"Cusip123"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_TRUE(result.has_value());
}

TEST_F(TradingSystemInstrumentsMatcher,
       ReturnsInstrumentNotFoundWhenCusipStrategyMatchFails) {
  instrument.cusip = CusipId{"Cusip123"};
  descriptor.security_id_source = SecurityIdSource::Option::Cusip;
  descriptor.security_id = SecurityId{"AnotherCusip"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(result.error(), LookupError::InstrumentNotFound);
}

TEST_F(TradingSystemInstrumentsMatcher,
       MatchesInstrumentDescriptorUsingIsinStrategy) {
  instrument.isin = IsinId{"Isin123"};
  instrument.security_type = SecurityType::Option::CommonStock;
  instrument.security_exchange = SecurityExchange{"XNYS"};
  instrument.price_currency = PriceCurrency{"USD"};

  descriptor.security_id_source = SecurityIdSource::Option::Isin;
  descriptor.security_id = SecurityId{"Isin123"};
  descriptor.security_type = SecurityType::Option::CommonStock;
  descriptor.security_exchange = SecurityExchange{"XNYS"};
  descriptor.currency = Currency{"USD"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_TRUE(result.has_value());
}

TEST_F(TradingSystemInstrumentsMatcher,
       ReturnsInstrumentNotFoundWhenIsinStrategyMatchFails) {
  instrument.isin = IsinId{"Isin123"};
  instrument.security_type = SecurityType::Option::CommonStock;
  instrument.security_exchange = SecurityExchange{"XNYS"};
  instrument.price_currency = PriceCurrency{"USD"};

  descriptor.security_id_source = SecurityIdSource::Option::Isin;
  descriptor.security_id = SecurityId{"AnotherIsin"};
  descriptor.security_type = SecurityType::Option::CommonStock;
  descriptor.security_exchange = SecurityExchange{"XNYS"};
  descriptor.currency = Currency{"USD"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(result.error(), LookupError::InstrumentNotFound);
}

TEST_F(
    TradingSystemInstrumentsMatcher,
    ReturnsMalformedInstrumentDescriptorWhenCurrencyIsMissingForIsinStrategy) {
  instrument.isin = IsinId{"Isin123"};
  instrument.security_type = SecurityType::Option::CommonStock;
  instrument.security_exchange = SecurityExchange{"XNYS"};
  instrument.price_currency = PriceCurrency{"USD"};

  descriptor.security_id_source = SecurityIdSource::Option::Isin;
  descriptor.security_id = SecurityId{"AnotherIsin"};
  descriptor.security_type = SecurityType::Option::CommonStock;
  descriptor.security_exchange = SecurityExchange{"XNYS"};
  descriptor.currency = std::nullopt;

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(result.error(), LookupError::MalformedInstrumentDescriptor);
}

TEST_F(TradingSystemInstrumentsMatcher,
       MatchesInstrumentDescriptorUsingRicStrategy) {
  instrument.ric = RicId{"Ric123"};
  descriptor.security_id_source = SecurityIdSource::Option::Ric;
  descriptor.security_id = SecurityId{"Ric123"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_TRUE(result.has_value());
}

TEST_F(TradingSystemInstrumentsMatcher,
       ReturnsInstrumentNotFoundWhenRicStrategyMatchFails) {
  instrument.ric = RicId{"Ric123"};
  descriptor.security_id_source = SecurityIdSource::Option::Ric;
  descriptor.security_id = SecurityId{"AnotherRic"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(result.error(), LookupError::InstrumentNotFound);
}

TEST_F(TradingSystemInstrumentsMatcher,
       MatchesInstrumentDescriptorUsingExchangeIdStrategy) {
  instrument.exchange_id = ExchangeId{"Exchange123"};
  descriptor.security_id_source = SecurityIdSource::Option::ExchangeSymbol;
  descriptor.security_id = SecurityId{"Exchange123"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_TRUE(result.has_value());
}

TEST_F(TradingSystemInstrumentsMatcher,
       ReturnsInstrumentNotFoundWhenExchangeIdStrategyMatchFails) {
  instrument.exchange_id = ExchangeId{"Exchange123"};
  descriptor.security_id_source = SecurityIdSource::Option::ExchangeSymbol;
  descriptor.security_id = SecurityId{"AnotherExchange"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(result.error(), LookupError::InstrumentNotFound);
}

TEST_F(TradingSystemInstrumentsMatcher,
       MatchesInstrumentDescriptorUsingBloombergIdStrategy) {
  instrument.bloomberg_id = BloombergId{"Bloomberg123"};
  descriptor.security_id_source = SecurityIdSource::Option::BloombergSymbol;
  descriptor.security_id = SecurityId{"Bloomberg123"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_TRUE(result.has_value());
}

TEST_F(TradingSystemInstrumentsMatcher,
       ReturnsInstrumentNotFoundWhenBloombergIdStrategyMatchFails) {
  instrument.bloomberg_id = BloombergId{"Bloomberg123"};
  descriptor.security_id_source = SecurityIdSource::Option::BloombergSymbol;
  descriptor.security_id = SecurityId{"AnotherBloomberg"};

  const Matcher::Implementation matcher{instrument};
  const auto result = matcher(descriptor);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(result.error(), LookupError::InstrumentNotFound);
}

}  // namespace
}  // namespace simulator::trading_system::instrument::tests
