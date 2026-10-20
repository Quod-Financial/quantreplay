#include <gtest/gtest.h>
#include <quickfix/Exceptions.h>
#include <quickfix/Values.h>

#include <string>
#include <tuple>

#include "common/custom_fields.hpp"
#include "common/custom_values.hpp"
#include "core/domain/attributes.hpp"
#include "ih/mapping/detail/from_fix_conversion.hpp"

namespace FIX {
namespace {
// NOLINTBEGIN(*explicit-constructor*)

USER_DEFINE_INT(TEST_INT_FIELD, 1);
USER_DEFINE_FLOAT(TEST_FLOAT_FIELD, 1);
USER_DEFINE_STRING(TEST_STRING_FIELD, 1);

// NOLINTEND(*explicit-constructor*)
}  // namespace
}  // namespace FIX

namespace simulator::fix::generator_initiator::test {
namespace {

using namespace testing;  // NOLINT

TEST(InitiatorFromFixIntegerConversion, ConvertsToSignedInteger) {
  ASSERT_EQ(convert_from_fix<std::int32_t>(FIX::TEST_INT_FIELD{42}), 42);
}

TEST(InitiatorFromFixIntegerConversion,
     ReportsUndeflowWhenConvertsToSignedInteger) {
  ASSERT_THROW(convert_from_fix<std::int8_t>(FIX::TEST_INT_FIELD{-200}),
               FIX::IncorrectTagValue);
}

TEST(InitiatorFromFixIntegerConversion,
     ReportsOverflowWhenConvertsToSignedInteger) {
  ASSERT_THROW(convert_from_fix<std::int8_t>(FIX::TEST_INT_FIELD{200}),
               FIX::IncorrectTagValue);
}

TEST(InitiatorFromFixIntegerConversion, ConvertsToUnsignedInteger) {
  ASSERT_EQ(convert_from_fix<std::uint32_t>(FIX::TEST_INT_FIELD{42}), 42);
}

TEST(InitiatorFromFixIntegerConversion,
     ReportsUnderflowWhenConvertsToUnsignedInteger) {
  ASSERT_THROW(convert_from_fix<std::uint16_t>(FIX::TEST_INT_FIELD{-1}),
               FIX::IncorrectTagValue);
}

TEST(InitiatorFromFixIntegerConversion,
     ReportsOverflowWhenConvertsToUnsignedInteger) {
  ASSERT_THROW(convert_from_fix<std::uint8_t>(FIX::TEST_INT_FIELD{300}),
               FIX::IncorrectTagValue);
}

TEST(InitiatorFromFixFloatConversion, ConvertsToDouble) {
  const FIX::TEST_FLOAT_FIELD fix_field = 42.42;

  ASSERT_DOUBLE_EQ(convert_from_fix<double>(fix_field), 42.42);
}

TEST(InitiatorFromFixStringConversion, ConvertsToString) {
  const FIX::TEST_STRING_FIELD fix_field{"test"};

  ASSERT_EQ(convert_from_fix<std::string>(fix_field), "test");
}

struct InitiatorFromFixAggressorSideConversion
    : public TestWithParam<std::tuple<char, Side>> {};

TEST_P(InitiatorFromFixAggressorSideConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<Side::Option>(FIX::AggressorSide{fix_value}));
}

TEST_F(InitiatorFromFixAggressorSideConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(convert_from_fix<Side::Option>(FIX::AggressorSide{'x'}),
               FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, InitiatorFromFixAggressorSideConversion,
  Values(
    std::make_tuple(FIX::Side_BUY, Side::Option::Buy),
    std::make_tuple(FIX::Side_SELL, Side::Option::Sell)
  ));
// clang-format on

struct InitiatorFromFixMarketEntryActionConversion
    : public TestWithParam<std::tuple<char, MarketEntryAction>> {};

TEST_P(InitiatorFromFixMarketEntryActionConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<MarketEntryAction::Option>(
                FIX::MDUpdateAction{fix_value}));
}

TEST_F(InitiatorFromFixMarketEntryActionConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(
      convert_from_fix<MarketEntryAction::Option>(FIX::MDUpdateAction{'x'}),
      FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, InitiatorFromFixMarketEntryActionConversion,
  Values(
    std::make_tuple(FIX::MDUpdateAction_NEW, MarketEntryAction::Option::New),
    std::make_tuple(FIX::MDUpdateAction_CHANGE, MarketEntryAction::Option::Change),
    std::make_tuple(FIX::MDUpdateAction_DELETE, MarketEntryAction::Option::Delete)
  ));
// clang-format on

struct InitiatorFromFixSecurityTypeConversion
    : public TestWithParam<std::tuple<std::string, SecurityType>> {};

TEST_P(InitiatorFromFixSecurityTypeConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(
      expected_internal_value,
      convert_from_fix<SecurityType::Option>(FIX::SecurityType{fix_value}));
}

TEST_F(InitiatorFromFixSecurityTypeConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(convert_from_fix<SecurityType::Option>(FIX::SecurityType{"x"}),
               FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, InitiatorFromFixSecurityTypeConversion,
  Values(
    std::make_tuple(FIX::SecurityType_COMMON_STOCK, SecurityType::Option::CommonStock),
    std::make_tuple(FIX::SecurityType_FUTURE, SecurityType::Option::Future),
    std::make_tuple(FIX::SecurityType_OPTION, SecurityType::Option::Option),
    std::make_tuple(FIX::SecurityType_MULTILEG_INSTRUMENT, SecurityType::Option::MultiLeg),
    std::make_tuple(FIX::SecurityType_SYNTHETIC_MULTILEG_INSTRUMENT, SecurityType::Option::SyntheticMultiLeg),
    std::make_tuple(FIX::SecurityType_WARRANT, SecurityType::Option::Warrant),
    std::make_tuple(FIX::SecurityType_MUTUAL_FUND, SecurityType::Option::MutualFund),
    std::make_tuple(FIX::SecurityType_CORPORATE_BOND, SecurityType::Option::CorporateBond),
    std::make_tuple(FIX::SecurityType_CONVERTIBLE_BOND, SecurityType::Option::ConvertibleBond),
    std::make_tuple(FIX::SecurityType_REPURCHASE_AGREEMENT, SecurityType::Option::RepurchaseAgreement),
    std::make_tuple(FIX::SecurityType_INDEX, SecurityType::Option::Index),
    std::make_tuple(FIX::SecurityType_CONTRACT_FOR_DIFFERENCE, SecurityType::Option::ContractForDifference),
    std::make_tuple(FIX::SecurityType_CERTIFICATE_OF_DEPOSIT, SecurityType::Option::Certificate),
    std::make_tuple(FIX::SecurityType_FX_SPOT, SecurityType::Option::FxSpot),
    std::make_tuple(FIX::SecurityType_FORWARD, SecurityType::Option::Forward),
    std::make_tuple(FIX::SecurityType_FX_FORWARD, SecurityType::Option::FxForward),
    std::make_tuple(FIX::SecurityType_NON_DELIVERABLE_FORWARD, SecurityType::Option::FxNonDeliverableForward),
    std::make_tuple(FIX::SecurityType_FX_SWAP, SecurityType::Option::FxSwap),
    std::make_tuple(FIX::SecurityType_NON_DELIVERABLE_SWAP, SecurityType::Option::FxNonDeliverableSwap)
  ));
// clang-format on

struct InitiatorFromFixSecurityIdSourceConversion
    : public TestWithParam<std::tuple<std::string, SecurityIdSource>> {};

TEST_P(InitiatorFromFixSecurityIdSourceConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<SecurityIdSource::Option>(
                FIX::SecurityIDSource{fix_value}));
}

TEST_F(InitiatorFromFixSecurityIdSourceConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(
      convert_from_fix<SecurityIdSource::Option>(FIX::SecurityIDSource{"x"}),
      FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, InitiatorFromFixSecurityIdSourceConversion,
  Values(
    std::make_tuple(FIX::SecurityIDSource_CUSIP, SecurityIdSource::Option::Cusip),
    std::make_tuple(FIX::SecurityIDSource_SEDOL, SecurityIdSource::Option::Sedol),
    std::make_tuple(FIX::SecurityIDSource_ISIN_NUMBER, SecurityIdSource::Option::Isin),
    std::make_tuple(FIX::SecurityIDSource_RIC_CODE, SecurityIdSource::Option::Ric),
    std::make_tuple(FIX::SecurityIDSource_EXCHANGE_SYMBOL, SecurityIdSource::Option::ExchangeSymbol),
    std::make_tuple(FIX::SecurityIDSource_BLOOMBERG_SYMBOL, SecurityIdSource::Option::BloombergSymbol)
  ));
// clang-format on

struct InitiatorFromFixMdEntryTypeConversion
    : public TestWithParam<std::tuple<char, MdEntryType>> {};

TEST_P(InitiatorFromFixMdEntryTypeConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<MdEntryType::Option>(FIX::MDEntryType{fix_value}));
}

TEST_F(InitiatorFromFixMdEntryTypeConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(convert_from_fix<MdEntryType::Option>(FIX::MDEntryType{'x'}),
               FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, InitiatorFromFixMdEntryTypeConversion,
  Values(
    std::make_tuple(FIX::MDEntryType_BID, MdEntryType::Option::Bid),
    std::make_tuple(FIX::MDEntryType_OFFER, MdEntryType::Option::Offer),
    std::make_tuple(FIX::MDEntryType_TRADE, MdEntryType::Option::Trade),
    std::make_tuple(FIX::MDEntryType_OPENING_PRICE, MdEntryType::Option::OpeningPrice),
    std::make_tuple(FIX::MDEntryType_CLOSING_PRICE, MdEntryType::Option::ClosingPrice),
    std::make_tuple(FIX::MDEntryType_SETTLEMENT_PRICE, MdEntryType::Option::SettlementPrice),
    std::make_tuple(FIX::MDEntryType_TRADING_SESSION_HIGH_PRICE, MdEntryType::Option::HighPrice),
    std::make_tuple(FIX::MDEntryType_TRADING_SESSION_LOW_PRICE, MdEntryType::Option::LowPrice),
    std::make_tuple(FIX::MDEntryType_IMBALANCE, MdEntryType::Option::Imbalance),
    std::make_tuple(FIX::MDEntryType_TRADE_VOLUME, MdEntryType::Option::TradeVolume),
    std::make_tuple(FIX::MDEntryType_MID_PRICE, MdEntryType::Option::MidPrice),
    std::make_tuple(FIX::MDEntryType_EARLY_PRICES, MdEntryType::Option::EarlyPrice),
    std::make_tuple(FIX::MDEntryType_AUCTION_CLEARING_PRICE, MdEntryType::Option::AuctionClearingPrice),
    std::make_tuple(FIX::MDEntryType_MARKET_BID, MdEntryType::Option::MarketBid),
    std::make_tuple(FIX::MDEntryType_MARKET_OFFER, MdEntryType::Option::MarketOffer),
    std::make_tuple(FIX::MDEntryType_PREVIOUS_CLOSING_PRICE, MdEntryType::Option::PreviousClosingPrice)
  ));
// clang-format on

}  // namespace
}  // namespace simulator::fix::generator_initiator::test