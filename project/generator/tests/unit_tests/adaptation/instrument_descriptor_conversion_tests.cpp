#include <gmock/gmock.h>

#include <optional>

#include "core/domain/attributes.hpp"
#include "core/domain/party.hpp"
#include "ih/adaptation/protocol_conversion.hpp"

namespace simulator::generator::test {
namespace {

using namespace testing;  // NOLINT

struct GeneratorInstrumentDescriptorConversion : public Test {
  data_layer::Listing::Patch patch;
  static constexpr int listing_id = 42;

  GeneratorInstrumentDescriptorConversion() { patch.with_venue_id("Venue"); }
};

TEST_F(GeneratorInstrumentDescriptorConversion, ConvertsSymbol) {
  patch.with_symbol("AAPL");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.symbol, Optional(Eq(Symbol{"AAPL"})));
}

TEST_F(GeneratorInstrumentDescriptorConversion, IgnoresAbsentSymbol) {
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.symbol, Eq(std::nullopt));
}

TEST_F(GeneratorInstrumentDescriptorConversion, IgnoresEmptySymbol) {
  patch.with_symbol({});
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.symbol, Eq(std::nullopt));
}

TEST_F(GeneratorInstrumentDescriptorConversion, CovertsSupportedSecurityType) {
  patch.with_security_type("CS");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.security_type,
              Optional(Eq(SecurityType::Option::CommonStock)));
}

TEST_F(GeneratorInstrumentDescriptorConversion, IgnoresUnknownSecurityType) {
  patch.with_security_type("UNKNOWN");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.security_type, Eq(std::nullopt));
}

TEST_F(GeneratorInstrumentDescriptorConversion, IgnoresAbsentSecurityType) {
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.security_type, Eq(std::nullopt));
}

TEST_F(GeneratorInstrumentDescriptorConversion,
       ConvertsPriceCurrencyInEqListing) {
  patch.with_security_type("CS").with_price_currency("USD");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.currency, Optional(Eq(Currency{"USD"})));
}

TEST_F(GeneratorInstrumentDescriptorConversion,
       ConvertsFxBaseCurrencyInFxListing) {
  patch.with_security_type("FXSPOT").with_fx_base_currency("EUR");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.currency, Optional(Eq(Currency{"EUR"})));
}

TEST_F(GeneratorInstrumentDescriptorConversion,
       IgnoresAbsentPriceCurrencyInEqListing) {
  patch.with_security_type("CS");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.currency, Eq(std::nullopt));
}

TEST_F(GeneratorInstrumentDescriptorConversion,
       IgnoresAbsentFxBaseCurrencyInFxListing) {
  patch.with_security_type("FXFWD");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.currency, Eq(std::nullopt));
}

TEST_F(GeneratorInstrumentDescriptorConversion,
       IgnoresCurrencyWithoutSecurityType) {
  patch.with_price_currency("USD").with_fx_base_currency("EUR");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.currency, Eq(std::nullopt));
}

TEST_F(GeneratorInstrumentDescriptorConversion, ConvertsSecurityExchange) {
  patch.with_security_exchange("XLON");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.security_exchange,
              Optional(Eq(SecurityExchange{"XLON"})));
}

TEST_F(GeneratorInstrumentDescriptorConversion, IgnoresEmptySecurityExchange) {
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.security_exchange, Eq(std::nullopt));
}

TEST_F(GeneratorInstrumentDescriptorConversion, ConvertsListingParty) {
  patch.with_party_id("PARTY").with_party_role("ExecutingFirm");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(
      descriptor.parties,
      ElementsAre(
          AllOf(Property(&Party::party_id, Eq(PartyId{"PARTY"})),
                Property(&Party::role, Eq(PartyRole::Option::ExecutingFirm)))));
}

TEST_F(GeneratorInstrumentDescriptorConversion,
       SetsPartyIdSourceProprietaryInListingParty) {
  patch.with_party_id("PARTY").with_party_role("ExecutingFirm");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.parties,
              ElementsAre(Property(&Party::source,
                                   Eq(PartyIdSource::Option::Proprietary))));
}

TEST_F(GeneratorInstrumentDescriptorConversion, IgnoresPartyWithUnknownRole) {
  patch.with_party_id("PARTY").with_party_role("UNKNOWN_ROLE");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.parties, IsEmpty());
}

TEST_F(GeneratorInstrumentDescriptorConversion, IgnoresPartyWithoutRole) {
  patch.with_party_id("PARTY");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.parties, IsEmpty());
}

TEST_F(GeneratorInstrumentDescriptorConversion, IgnoresPartyWithoutPartyId) {
  patch.with_party_role("ExecutingFirm");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  ASSERT_THAT(descriptor.parties, IsEmpty());
}

TEST_F(GeneratorInstrumentDescriptorConversion, ConvertsCusipIdentifier) {
  patch.with_cusip_id("CUSIP");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  EXPECT_THAT(descriptor.security_id, Optional(Eq(SecurityId{"CUSIP"})));
  EXPECT_THAT(descriptor.security_id_source,
              Optional(Eq(SecurityIdSource::Option::Cusip)));
}

TEST_F(GeneratorInstrumentDescriptorConversion, ConvertsSedolIdentifier) {
  patch.with_sedol_id("SEDOL");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  EXPECT_THAT(descriptor.security_id, Optional(Eq(SecurityId{"SEDOL"})));
  EXPECT_THAT(descriptor.security_id_source,
              Optional(Eq(SecurityIdSource::Option::Sedol)));
}

TEST_F(GeneratorInstrumentDescriptorConversion, ConvertsIsinIdentifier) {
  patch.with_isin_id("ISIN");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  EXPECT_THAT(descriptor.security_id, Optional(Eq(SecurityId{"ISIN"})));
  EXPECT_THAT(descriptor.security_id_source,
              Optional(Eq(SecurityIdSource::Option::Isin)));
}

TEST_F(GeneratorInstrumentDescriptorConversion, ConvertsRicIdentifier) {
  patch.with_ric_id("RIC");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  EXPECT_THAT(descriptor.security_id, Optional(Eq(SecurityId{"RIC"})));
  EXPECT_THAT(descriptor.security_id_source,
              Optional(Eq(SecurityIdSource::Option::Ric)));
}

TEST_F(GeneratorInstrumentDescriptorConversion, ConvertsExchangeIdentifier) {
  patch.with_exchange_symbol_id("EXC");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  EXPECT_THAT(descriptor.security_id, Optional(Eq(SecurityId{"EXC"})));
  EXPECT_THAT(descriptor.security_id_source,
              Optional(Eq(SecurityIdSource::Option::ExchangeSymbol)));
}

TEST_F(GeneratorInstrumentDescriptorConversion, ConvertsBloombergIdentifier) {
  patch.with_bloomberg_symbol_id("BBG");
  const auto listing = data_layer::Listing::create(patch, listing_id);

  const auto descriptor = convert_to_instrument_descriptor(listing);

  EXPECT_THAT(descriptor.security_id, Optional(Eq(SecurityId{"BBG"})));
  EXPECT_THAT(descriptor.security_id_source,
              Optional(Eq(SecurityIdSource::Option::BloombergSymbol)));
}

TEST_F(GeneratorInstrumentDescriptorConversion, IgnoresEmptySecurityId) {
  const auto listing = data_layer::Listing::create(patch, listing_id);
  const auto descriptor = convert_to_instrument_descriptor(listing);

  EXPECT_THAT(descriptor.security_id, Eq(std::nullopt));
  EXPECT_THAT(descriptor.security_id_source, Eq(std::nullopt));
}

}  // namespace
}  // namespace simulator::generator::test