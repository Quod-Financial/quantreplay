#include <gtest/gtest.h>
#include <quickfix/Fields.h>
#include <quickfix/FixValues.h>

#include <chrono>
#include <limits>
#include <stdexcept>
#include <tuple>

#include "common/custom_values.hpp"
#include "common/mapping/detail/to_fix_conversion.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"

namespace FIX {
namespace {

USER_DEFINE_INT(TestIntField, 1);
USER_DEFINE_FLOAT(TestFloatField, 1);
USER_DEFINE_STRING(TestStringField, 1);
USER_DEFINE_LOCALMKTDATE(TestLocalMktDateField, 1);
USER_DEFINE_UTCDATE(TestUtcDateField, 1);
USER_DEFINE_UTCTIMEONLY(TestUtcTimeOnlyField, 1);
USER_DEFINE_UTCTIMESTAMP(TestUtcTimeStampField, 1);

}  // namespace
}  // namespace FIX

namespace simulator::fix::test {
namespace {

using namespace std::chrono_literals;
using namespace testing;  // NOLINT

/*----------------------------------------------------------------------------*/

TEST(ToFixIntegerConversion, ConvertsSignedToFix) {
  ASSERT_EQ(convert_to_fix<FIX::TestIntField>(-42), -42);
}

TEST(ToFixIntegerConversion, ReportsOverflowOnSignedConversion) {
  constexpr auto overflow_value = std::numeric_limits<std::int64_t>::max();

  ASSERT_THROW(convert_to_fix<FIX::TestIntField>(overflow_value),
               std::invalid_argument);
}

TEST(ToFixIntegerConversion, ReportsUnderflowOnSignedConversion) {
  constexpr auto overflow_value = std::numeric_limits<std::int64_t>::min();

  ASSERT_THROW(convert_to_fix<FIX::TestIntField>(overflow_value),
               std::invalid_argument);
}

TEST(ToFixIntegerConversion, ConvertsUnsignedToFix) {
  ASSERT_EQ(convert_to_fix<FIX::TestIntField>(142000u), 142000u);
}

TEST(ToFixIntegerConversion, ReportsOverflowOnUnsignedConversion) {
  constexpr auto overflow_value = std::numeric_limits<std::uint64_t>::max();

  ASSERT_THROW(convert_to_fix<FIX::TestIntField>(overflow_value),
               std::invalid_argument);
}

/*----------------------------------------------------------------------------*/

TEST(ToFixFloatingPointConversion, CovertsDoubleToFix) {
  ASSERT_DOUBLE_EQ(convert_to_fix<FIX::TestFloatField>(3.14), 3.14);
}

/*----------------------------------------------------------------------------*/

TEST(ToFixStringConversion, ConvertsStringToFix) {
  ASSERT_EQ(convert_to_fix<FIX::TestStringField>("test"), "test");
}

/*----------------------------------------------------------------------------*/

TEST(ToFixLocalDateConversion, ConvertsLocalDaysToFix) {
  const auto fix_field = convert_to_fix<FIX::MaturityDate>(
      std::chrono::local_days{2022y / 12 / 31});

  ASSERT_EQ(fix_field.getValue(), "20221231");
}

TEST(ToFixLocalDateConversion, AddsZerosToMonthAndDay) {
  const auto fix_field =
      convert_to_fix<FIX::MaturityDate>(std::chrono::local_days{2022y / 1 / 1});

  ASSERT_EQ(fix_field.getValue(), "20220101");
}

/*----------------------------------------------------------------------------*/

TEST(ToFixUtcDateConversion, ConvertsSystemMicrosecondsToFix) {
  // 2024-05-28 10:31:58 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318000000us};

  const auto fix_field = convert_to_fix<FIX::TestUtcDateField>(timestamp);

  ASSERT_EQ(fix_field.getString(), "20240528");
}

/*----------------------------------------------------------------------------*/

TEST(ToFixUtcTimeOnlyConversion, ConvertsSystemMicrosecondsToFixSeconds) {
  // 2024-05-28 10:31:58.999999 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318999999us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeOnlyField>(timestamp, TimestampPrecision::Seconds);

  ASSERT_EQ(fix_field.getString(), "10:31:58");
}

/*----------------------------------------------------------------------------*/

TEST(ToFixUtcTimeOnlyConversion, ConvertsSystemMicrosecondsToFixMilliseconds) {
  // 2024-05-28 10:31:58.123999 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318123999us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeOnlyField>(timestamp, TimestampPrecision::Milliseconds);

  ASSERT_EQ(fix_field.getString(), "10:31:58.123");
}

/*----------------------------------------------------------------------------*/

TEST(ToFixUtcTimeOnlyConversion, ConvertsSystemMicrosecondsToFixMicroseconds) {
  // 2024-05-28 10:31:58.123456 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318123456us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeOnlyField>(timestamp, TimestampPrecision::Microseconds);

  ASSERT_EQ(fix_field.getString(), "10:31:58.123456");
}

/*----------------------------------------------------------------------------*/

TEST(ToFixUtcTimeStampConversion, ConvertsSystemMicrosecondsToFixSeconds) {
  // 2024-05-28 10:31:58.999999 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318999999us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeStampField>(timestamp, TimestampPrecision::Seconds);

  ASSERT_EQ(fix_field.getString(), "20240528-10:31:58");
}

/*----------------------------------------------------------------------------*/

TEST(ToFixUtcTimeStampConversion, ConvertsSystemMicrosecondsToFixMilliseconds) {
  // 2024-05-28 10:31:58.123999 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318123999us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeStampField>(timestamp, TimestampPrecision::Milliseconds);

  ASSERT_EQ(fix_field.getString(), "20240528-10:31:58.123");
}

/*----------------------------------------------------------------------------*/

TEST(ToFixUtcTimeStampConversion, ConvertsSystemMicrosecondsToFixMicroseconds) {
  // 2024-05-28 10:31:58.123456 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318123456us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeStampField>(timestamp, TimestampPrecision::Microseconds);

  ASSERT_EQ(fix_field.getString(), "20240528-10:31:58.123456");
}

/*----------------------------------------------------------------------------*/

struct ToFixOrderStatusConversion
    : public TestWithParam<std::tuple<OrderStatus, char>> {};

TEST_P(ToFixOrderStatusConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::OrdStatus>(internal_value), expected_fix_value);
}

TEST_F(ToFixOrderStatusConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(
      convert_to_fix<FIX::OrdStatus>(static_cast<OrderStatus::Option>(0xFF)),
      std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixOrderStatusConversion,
  Values(
    std::make_tuple(OrderStatus::Option::New, FIX::OrdStatus_NEW),
    std::make_tuple(OrderStatus::Option::PartiallyFilled, FIX::OrdStatus_PARTIALLY_FILLED),
    std::make_tuple(OrderStatus::Option::Filled, FIX::OrdStatus_FILLED),
    std::make_tuple(OrderStatus::Option::Modified, FIX::OrdStatus_REPLACED),
    std::make_tuple(OrderStatus::Option::Cancelled, FIX::OrdStatus_CANCELED),
    std::make_tuple(OrderStatus::Option::Rejected, FIX::OrdStatus_REJECTED)
  )
);
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixOrderTypeConversion
    : public TestWithParam<std::tuple<OrderType, char>> {};

TEST_P(ToFixOrderTypeConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::OrdType>(internal_value), expected_fix_value);
}

TEST_F(ToFixOrderTypeConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(
      convert_to_fix<FIX::OrdType>(static_cast<OrderType::Option>(0xFF)),
      std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixOrderTypeConversion,
  Values(
    std::make_tuple(OrderType::Option::Market, FIX::OrdType_MARKET),
    std::make_tuple(OrderType::Option::Limit, FIX::OrdType_LIMIT)
  )
);
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToSideConversion : public TestWithParam<std::tuple<Side, char>> {};

TEST_P(ToSideConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::Side>(internal_value), expected_fix_value);
}

TEST_F(ToSideConversion, ReportsErrorOnUnknownValueConversionConversion) {
  ASSERT_THROW(convert_to_fix<FIX::Side>(static_cast<Side::Option>(0xFF)),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToSideConversion,
  Values(
    std::make_tuple(Side::Option::Buy, FIX::Side_BUY),
    std::make_tuple(Side::Option::Sell, FIX::Side_SELL),
    std::make_tuple(Side::Option::SellShort, FIX::Side_SELL_SHORT),
    std::make_tuple(Side::Option::SellShortExempt, FIX::Side_SELL_SHORT_EXEMPT)
  )
);
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixTimeInForceConversion
    : public TestWithParam<std::tuple<TimeInForce, char>> {};

TEST_P(ToFixTimeInForceConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::TimeInForce>(internal_value),
            expected_fix_value);
}

TEST_F(ToFixTimeInForceConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(
      convert_to_fix<FIX::TimeInForce>(static_cast<TimeInForce::Option>(0xFF)),
      std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixTimeInForceConversion,
  Values(
    std::make_tuple(TimeInForce::Option::Day, FIX::TimeInForce_DAY),
    std::make_tuple(TimeInForce::Option::ImmediateOrCancel, FIX::TimeInForce_IMMEDIATE_OR_CANCEL),
    std::make_tuple(TimeInForce::Option::FillOrKill, FIX::TimeInForce_FILL_OR_KILL),
    std::make_tuple(TimeInForce::Option::GoodTillDate, FIX::TimeInForce_GOOD_TILL_DATE),
    std::make_tuple(TimeInForce::Option::GoodTillCancel, FIX::TimeInForce_GOOD_TILL_CANCEL)
  )
);
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixSecurityTypeConversion
    : public TestWithParam<std::tuple<SecurityType, std::string>> {};

TEST_P(ToFixSecurityTypeConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::SecurityType>(internal_value),
            expected_fix_value);
}

TEST_F(ToFixSecurityTypeConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::SecurityType>(
                   static_cast<SecurityType::Option>(0xFF)),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixSecurityTypeConversion,
  Values(
    std::make_tuple(SecurityType::Option::CommonStock, FIX::SecurityType_COMMON_STOCK),
    std::make_tuple(SecurityType::Option::Future, FIX::SecurityType_FUTURE),
    std::make_tuple(SecurityType::Option::Option, FIX::SecurityType_OPTION),
    std::make_tuple(SecurityType::Option::MultiLeg, FIX::SecurityType_MULTILEG_INSTRUMENT),
    std::make_tuple(SecurityType::Option::SyntheticMultiLeg, FIX::SecurityType_SYNTHETIC_MULTILEG_INSTRUMENT),
    std::make_tuple(SecurityType::Option::Warrant, FIX::SecurityType_WARRANT),
    std::make_tuple(SecurityType::Option::MutualFund, FIX::SecurityType_MUTUAL_FUND),
    std::make_tuple(SecurityType::Option::CorporateBond, FIX::SecurityType_CORPORATE_BOND),
    std::make_tuple(SecurityType::Option::ConvertibleBond, FIX::SecurityType_CONVERTIBLE_BOND),
    std::make_tuple(SecurityType::Option::RepurchaseAgreement, FIX::SecurityType_REPURCHASE_AGREEMENT),
    std::make_tuple(SecurityType::Option::Index, FIX::SecurityType_INDEX),
    std::make_tuple(SecurityType::Option::ContractForDifference, FIX::SecurityType_CONTRACT_FOR_DIFFERENCE),
    std::make_tuple(SecurityType::Option::Certificate, FIX::SecurityType_CERTIFICATE_OF_DEPOSIT),
    std::make_tuple(SecurityType::Option::FxSpot, FIX::SecurityType_FX_SPOT),
    std::make_tuple(SecurityType::Option::Forward, FIX::SecurityType_FORWARD),
    std::make_tuple(SecurityType::Option::FxForward, FIX::SecurityType_FX_FORWARD),
    std::make_tuple(SecurityType::Option::FxNonDeliverableForward, FIX::SecurityType_NON_DELIVERABLE_FORWARD),
    std::make_tuple(SecurityType::Option::FxSwap, FIX::SecurityType_FX_SWAP),
    std::make_tuple(SecurityType::Option::FxNonDeliverableSwap, FIX::SecurityType_NON_DELIVERABLE_SWAP)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixSecurityIdSourceConversion
    : public TestWithParam<std::tuple<SecurityIdSource, std::string>> {};

TEST_P(ToFixSecurityIdSourceConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::SecurityIDSource>(internal_value),
            expected_fix_value);
}

TEST_F(ToFixSecurityIdSourceConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::SecurityIDSource>(
                   static_cast<SecurityIdSource::Option>(0xFF)),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixSecurityIdSourceConversion,
  Values(
    std::make_tuple(SecurityIdSource::Option::Cusip, FIX::SecurityIDSource_CUSIP),
    std::make_tuple(SecurityIdSource::Option::Sedol, FIX::SecurityIDSource_SEDOL),
    std::make_tuple(SecurityIdSource::Option::Isin, FIX::SecurityIDSource_ISIN_NUMBER),
    std::make_tuple(SecurityIdSource::Option::Ric, FIX::SecurityIDSource_RIC_CODE),
    std::make_tuple(SecurityIdSource::Option::ExchangeSymbol, FIX::SecurityIDSource_EXCHANGE_SYMBOL),
    std::make_tuple(SecurityIdSource::Option::BloombergSymbol, FIX::SecurityIDSource_BLOOMBERG_SYMBOL)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixPartyIdSourceConversion
    : public TestWithParam<std::tuple<PartyIdSource, char>> {};

TEST_P(ToFixPartyIdSourceConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::PartyIDSource>(internal_value),
            expected_fix_value);
}

TEST_F(ToFixPartyIdSourceConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::PartyIDSource>(
                   static_cast<PartyIdSource::Option>(0xFF)),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixPartyIdSourceConversion,
  Values(
    std::make_tuple(PartyIdSource::Option::UKNationalInsuranceOrPensionNumber, FIX::PartyIDSource_UK_NATIONAL_INSURANCE_OR_PENSION_NUMBER),
    std::make_tuple(PartyIdSource::Option::USSocialSecurityNumber, FIX::PartyIDSource_US_SOCIAL_SECURITY_NUMBER),
    std::make_tuple(PartyIdSource::Option::USEmployerOrTaxIDNumber, FIX::PartyIDSource_US_EMPLOYER_OR_TAX_ID_NUMBER),
    std::make_tuple(PartyIdSource::Option::AustralianBusinessNumber, FIX::PartyIDSource_AUSTRALIAN_BUSINESS_NUMBER),
    std::make_tuple(PartyIdSource::Option::AustralianTaxFileNumber, FIX::PartyIDSource_AUSTRALIAN_TAX_FILE_NUMBER),
    std::make_tuple(PartyIdSource::Option::TaxID, FIX::PartyIDSource_TAX_ID),
    std::make_tuple(PartyIdSource::Option::KoreanInvestorID, FIX::PartyIDSource_KOREAN_INVESTOR_ID),
    std::make_tuple(PartyIdSource::Option::TaiwaneseForeignInvestorID, FIX::PartyIDSource_TAIWANESE_QUALIFIED_FOREIGN_INVESTOR_ID_QFII_FID),
    std::make_tuple(PartyIdSource::Option::TaiwaneseTradingAcct, FIX::PartyIDSource_TAIWANESE_TRADING_ACCOUNT),
    std::make_tuple(PartyIdSource::Option::MalaysianCentralDepository, FIX::PartyIDSource_MALAYSIAN_CENTRAL_DEPOSITORY),
    std::make_tuple(PartyIdSource::Option::ChineseInvestorID, FIX::PartyIDSource_CHINESE_INVESTOR_ID),
    std::make_tuple(PartyIdSource::Option::ISITCAcronym, FIX::PartyIDSource_DIRECTED_BROKER_THREE_CHARACTER_ACRONYM_AS_DEFINED_IN_ISITC_ETC_BEST_PRACTICE_GUIDELINES_DOCUMENT),
    std::make_tuple(PartyIdSource::Option::BIC, FIX::PartyIDSource_BIC),
    std::make_tuple(PartyIdSource::Option::GeneralIdentifier, FIX::PartyIDSource_GENERALLY_ACCEPTED_MARKET_PARTICIPANT_IDENTIFIER),
    std::make_tuple(PartyIdSource::Option::Proprietary, FIX::PartyIDSource_PROPRIETARY),
    std::make_tuple(PartyIdSource::Option::ISOCountryCode, FIX::PartyIDSource_ISO_COUNTRY_CODE),
    std::make_tuple(PartyIdSource::Option::SettlementEntityLocation, FIX::PartyIDSource_SETTLEMENT_ENTITY_LOCATION),
    std::make_tuple(PartyIdSource::Option::MIC, FIX::PartyIDSource_MIC),
    std::make_tuple(PartyIdSource::Option::CSDParticipant, FIX::PartyIDSource_CSD_PARTICIPANT_MEMBER_CODE),
    std::make_tuple(PartyIdSource::Option::AustralianCompanyNumber, FIX::PartyIDSource_AUSTRALIAN_COMPANY_NUMBER),
    std::make_tuple(PartyIdSource::Option::AustralianRegisteredBodyNumber, FIX::PartyIDSource_AUSTRALIAN_REGISTERED_BODY_NUMBER),
    std::make_tuple(PartyIdSource::Option::CFTCReportingFirmIdentifier, FIX::PartyIDSource_CFTC_REPORTING_FIRM_IDENTIFIER),
    std::make_tuple(PartyIdSource::Option::LegalEntityIdentifier, FIX::PartyIDSource_LEI),
    std::make_tuple(PartyIdSource::Option::InterimIdentifier, FIX::PartyIDSource_INTERIM_IDENTIFIER),
    std::make_tuple(PartyIdSource::Option::ShortCodeIdentifier, FIX::PartyIDSource_SHORT_CODE_IDENTIFIER),
    std::make_tuple(PartyIdSource::Option::NationalIDNaturalPerson, FIX::PartyIDSource_NATIONAL_ID_NATURAL_PERSON),
    std::make_tuple(PartyIdSource::Option::IndiaPermanentAccountNumber, FIX::PartyIDSource_INDIA_PERMANENT_ACCOUNT_NUMBER),
    std::make_tuple(PartyIdSource::Option::FDID, FIX::PartyIDSource_FDID),
    std::make_tuple(PartyIdSource::Option::SPSAID, FIX::PartyIDSource_SPASAID),
    std::make_tuple(PartyIdSource::Option::MasterSPSAID, FIX::PartyIDSource_MASTER_SPASAID)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixPartyRoleConversion
    : public TestWithParam<std::tuple<PartyRole, int>> {};

TEST_P(ToFixPartyRoleConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::PartyRole>(internal_value), expected_fix_value);
}

TEST_F(ToFixPartyRoleConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(
      convert_to_fix<FIX::PartyRole>(static_cast<PartyRole::Option>(0xFF)),
      std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixPartyRoleConversion,
  Values(
    std::make_tuple(PartyRole::Option::ExecutingFirm, FIX::PartyRole_EXECUTING_FIRM),
    std::make_tuple(PartyRole::Option::BrokerOfCredit, FIX::PartyRole_BROKER_OF_CREDIT),
    std::make_tuple(PartyRole::Option::ClientID, FIX::PartyRole_CLIENT_ID),
    std::make_tuple(PartyRole::Option::ClearingFirm, FIX::PartyRole_CLEARING_FIRM),
    std::make_tuple(PartyRole::Option::InvestorID, FIX::PartyRole_INVESTOR_ID),
    std::make_tuple(PartyRole::Option::IntroducingFirm, FIX::PartyRole_INTRODUCING_FIRM),
    std::make_tuple(PartyRole::Option::EnteringFirm, FIX::PartyRole_ENTERING_FIRM),
    std::make_tuple(PartyRole::Option::Locate, FIX::PartyRole_LOCATE),
    std::make_tuple(PartyRole::Option::FundManagerClientID, FIX::PartyRole_FUND_MANAGER_CLIENT_ID),
    std::make_tuple(PartyRole::Option::SettlementLocation, FIX::PartyRole_SETTLEMENT_LOCATION),
    std::make_tuple(PartyRole::Option::OrderOriginationTrader, FIX::PartyRole_ORDER_ORIGINATION_TRADER),
    std::make_tuple(PartyRole::Option::ExecutingTrader, FIX::PartyRole_EXECUTING_TRADER),
    std::make_tuple(PartyRole::Option::OrderOriginationFirm, FIX::PartyRole_ORDER_ORIGINATION_FIRM),
    std::make_tuple(PartyRole::Option::GiveupClearingFirm, FIX::PartyRole_GIVEUP_CLEARING_FIRM),
    std::make_tuple(PartyRole::Option::CorrespondantClearingFirm, FIX::PartyRole_CORRESPONDANT_CLEARING_FIRM),
    std::make_tuple(PartyRole::Option::ExecutingSystem, FIX::PartyRole_EXECUTING_SYSTEM),
    std::make_tuple(PartyRole::Option::ContraFirm, FIX::PartyRole_CONTRA_FIRM),
    std::make_tuple(PartyRole::Option::ContraClearingFirm, FIX::PartyRole_CONTRA_CLEARING_FIRM),
    std::make_tuple(PartyRole::Option::SponsoringFirm, FIX::PartyRole_SPONSORING_FIRM),
    std::make_tuple(PartyRole::Option::UnderlyingContraFirm, FIX::PartyRole_UNDERLYING_CONTRA_FIRM),
    std::make_tuple(PartyRole::Option::ClearingOrganization, FIX::PartyRole_CLEARING_ORGANIZATION),
    std::make_tuple(PartyRole::Option::Exchange, FIX::PartyRole_EXCHANGE),
    std::make_tuple(PartyRole::Option::CustomerAccount, FIX::PartyRole_CUSTOMER_ACCOUNT),
    std::make_tuple(PartyRole::Option::CorrespondentClearingOrganization, FIX::PartyRole_CORRESPONDENT_CLEARING_ORGANIZATION),
    std::make_tuple(PartyRole::Option::CorrespondentBroker, FIX::PartyRole_CORRESPONDENT_BROKER),
    std::make_tuple(PartyRole::Option::Custodian, FIX::PartyRole_CUSTODIAN),
    std::make_tuple(PartyRole::Option::Intermediary, FIX::PartyRole_INTERMEDIARY),
    std::make_tuple(PartyRole::Option::Agent, FIX::PartyRole_AGENT),
    std::make_tuple(PartyRole::Option::SubCustodian, FIX::PartyRole_SUB_CUSTODIAN),
    std::make_tuple(PartyRole::Option::Beneficiary, FIX::PartyRole_BENEFICIARY),
    std::make_tuple(PartyRole::Option::InterestedParty, FIX::PartyRole_INTERESTED_PARTY),
    std::make_tuple(PartyRole::Option::RegulatoryBody, FIX::PartyRole_REGULATORY_BODY),
    std::make_tuple(PartyRole::Option::LiquidityProvider, FIX::PartyRole_LIQUIDITY_PROVIDER),
    std::make_tuple(PartyRole::Option::EnteringTrader, FIX::PartyRole_ENTERING_TRADER),
    std::make_tuple(PartyRole::Option::ContraTrader, FIX::PartyRole_CONTRA_TRADER),
    std::make_tuple(PartyRole::Option::PositionAccount, FIX::PartyRole_POSITION_ACCOUNT),
    std::make_tuple(PartyRole::Option::ContraExchange, FIX::PartyRole_CONTRA_EXCHANGE),
    std::make_tuple(PartyRole::Option::InternalCarryAccount, FIX::PartyRole_INTERNAL_CARRY_ACCOUNT),
    std::make_tuple(PartyRole::Option::OrderEntryOperatorID, FIX::PartyRole_ORDER_ENTRY_OPERATOR_ID),
    std::make_tuple(PartyRole::Option::SecondaryAccountNumber, FIX::PartyRole_SECONDARY_ACCOUNT_NUMBER),
    std::make_tuple(PartyRole::Option::ForeignFirm, FIX::PartyRole_FOREIGN_FIRM),
    std::make_tuple(PartyRole::Option::ThirdPartyAllocationFirm, FIX::PartyRole_THIRD_PARTY_ALLOCATION_FIRM),
    std::make_tuple(PartyRole::Option::ClaimingAccount, FIX::PartyRole_CLAIMING_ACCOUNT),
    std::make_tuple(PartyRole::Option::AssetManager, FIX::PartyRole_ASSET_MANAGER),
    std::make_tuple(PartyRole::Option::PledgorAccount, FIX::PartyRole_PLEDGOR_ACCOUNT),
    std::make_tuple(PartyRole::Option::PledgeeAccount, FIX::PartyRole_PLEDGEE_ACCOUNT),
    std::make_tuple(PartyRole::Option::LargeTraderReportableAccount, FIX::PartyRole_LARGE_TRADER_REPORTABLE_ACCOUNT),
    std::make_tuple(PartyRole::Option::TraderMnemonic, FIX::PartyRole_TRADER_MNEMONIC),
    std::make_tuple(PartyRole::Option::SenderLocation, FIX::PartyRole_SENDER_LOCATION),
    std::make_tuple(PartyRole::Option::SessionID, FIX::PartyRole_SESSION_ID),
    std::make_tuple(PartyRole::Option::AcceptableCounterparty, FIX::PartyRole_ACCEPTABLE_COUNTERPARTY),
    std::make_tuple(PartyRole::Option::UnacceptableCounterparty, FIX::PartyRole_UNACCEPTABLE_COUNTERPARTY),
    std::make_tuple(PartyRole::Option::EnteringUnit, FIX::PartyRole_ENTERING_UNIT),
    std::make_tuple(PartyRole::Option::ExecutingUnit, FIX::PartyRole_EXECUTING_UNIT),
    std::make_tuple(PartyRole::Option::IntroducingBroker, FIX::PartyRole_INTRODUCING_BROKER),
    std::make_tuple(PartyRole::Option::QuoteOriginator, FIX::PartyRole_QUOTE_ORIGINATOR),
    std::make_tuple(PartyRole::Option::ReportOriginator, FIX::PartyRole_REPORT_ORIGINATOR),
    std::make_tuple(PartyRole::Option::SystematicInternaliser, FIX::PartyRole_SYSTEMATIC_INTERNALISER),
    std::make_tuple(PartyRole::Option::MultilateralTradingFacility, FIX::PartyRole_MULTILATERAL_TRADING_FACILITY),
    std::make_tuple(PartyRole::Option::RegulatedMarket, FIX::PartyRole_REGULATED_MARKET),
    std::make_tuple(PartyRole::Option::MarketMaker, FIX::PartyRole_MARKET_MAKER),
    std::make_tuple(PartyRole::Option::InvestmentFirm, FIX::PartyRole_INVESTMENT_FIRM),
    std::make_tuple(PartyRole::Option::HostCompetentAuthority, FIX::PartyRole_HOST_COMPETENT_AUTHORITY),
    std::make_tuple(PartyRole::Option::HomeCompetentAuthority, FIX::PartyRole_HOME_COMPETENT_AUTHORITY),
    std::make_tuple(PartyRole::Option::CompetentAuthorityLiquidity, FIX::PartyRole_COMPETENT_AUTHORITY_OF_THE_MOST_RELEVANT_MARKET_IN_TERMS_OF_LIQUIDITY),
    std::make_tuple(PartyRole::Option::CompetentAuthorityTransactionVenue, FIX::PartyRole_COMPETENT_AUTHORITY_OF_THE_TRANSACTION),
    std::make_tuple(PartyRole::Option::ReportingIntermediary, FIX::PartyRole_REPORTING_INTERMEDIARY),
    std::make_tuple(PartyRole::Option::ExecutionVenue, FIX::PartyRole_EXECUTION_VENUE),
    std::make_tuple(PartyRole::Option::MarketDataEntryOriginator, FIX::PartyRole_MARKET_DATA_ENTRY_ORIGINATOR),
    std::make_tuple(PartyRole::Option::LocationID, FIX::PartyRole_LOCATION_ID),
    std::make_tuple(PartyRole::Option::DeskID, FIX::PartyRole_DESK_ID),
    std::make_tuple(PartyRole::Option::MarketDataMarket, FIX::PartyRole_MARKET_DATA_MARKET),
    std::make_tuple(PartyRole::Option::AllocationEntity, FIX::PartyRole_ALLOCATION_ENTITY),
    std::make_tuple(PartyRole::Option::PrimeBroker, FIX::PartyRole_PRIME_BROKER_PROVIDING_GENERAL_TRADE_SERVICES),
    std::make_tuple(PartyRole::Option::StepOutFirm, FIX::PartyRole_STEP_OUT_FIRM),
    std::make_tuple(PartyRole::Option::BrokerClearingID, FIX::PartyRole_BROKERCLEARINGID),
    std::make_tuple(PartyRole::Option::CentralRegistrationDepository, FIX::PartyRole_CENTRAL_REGISTRATION_DEPOSITORY),
    std::make_tuple(PartyRole::Option::ClearingAccount, FIX::PartyRole_CLEARING_ACCOUNT),
    std::make_tuple(PartyRole::Option::AcceptableSettlingCounterparty, FIX::PartyRole_ACCEPTABLE_SETTLING_COUNTERPARTY),
    std::make_tuple(PartyRole::Option::UnacceptableSettlingCounterparty, FIX::PartyRole_UNACCEPTABLE_SETTLING_COUNTERPARTY)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixExecutionTypeConversion
    : public TestWithParam<std::tuple<ExecutionType, char>> {};

TEST_P(ToFixExecutionTypeConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::ExecType>(internal_value), expected_fix_value);
}

TEST_F(ToFixExecutionTypeConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(
      convert_to_fix<FIX::ExecType>(static_cast<ExecutionType::Option>(0xFF)),
      std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixExecutionTypeConversion,
  Values(
    std::make_tuple(ExecutionType::Option::OrderPlaced, FIX::ExecType_NEW),
    std::make_tuple(ExecutionType::Option::OrderModified, FIX::ExecType_REPLACED),
    std::make_tuple(ExecutionType::Option::OrderCancelled, FIX::ExecType_CANCELED),
    std::make_tuple(ExecutionType::Option::OrderTraded, FIX::ExecType_TRADE),
    std::make_tuple(ExecutionType::Option::Rejected, FIX::ExecType_REJECTED)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixMdRejectReasonConversion
    : public TestWithParam<std::tuple<MdRejectReason, char>> {};

TEST_P(ToFixMdRejectReasonConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::MDReqRejReason>(internal_value),
            expected_fix_value);
}

TEST_F(ToFixMdRejectReasonConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::MDReqRejReason>(
                   static_cast<MdRejectReason::Option>(0xFF)),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixMdRejectReasonConversion,
  Values(
    std::make_tuple(MdRejectReason::Option::UnknownSymbol, FIX::MDReqRejReason_UNKNOWN_SYMBOL),
    std::make_tuple(MdRejectReason::Option::DuplicateMdReqId, FIX::MDReqRejReason_DUPLICATE_MDREQID)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixMdEntryTypeConversion
    : public TestWithParam<std::tuple<MdEntryType, char>> {};

TEST_P(ToFixMdEntryTypeConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::MDEntryType>(internal_value),
            expected_fix_value);
}

TEST_F(ToFixMdEntryTypeConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(
      convert_to_fix<FIX::MDEntryType>(static_cast<MdEntryType::Option>(0xFF)),
      std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixMdEntryTypeConversion,
  Values(
    std::make_tuple(MdEntryType::Option::Bid, FIX::MDEntryType_BID),
    std::make_tuple(MdEntryType::Option::Offer, FIX::MDEntryType_OFFER),
    std::make_tuple(MdEntryType::Option::Trade, FIX::MDEntryType_TRADE),
    std::make_tuple(MdEntryType::Option::LowPrice, FIX::MDEntryType_TRADING_SESSION_LOW_PRICE),
    std::make_tuple(MdEntryType::Option::MidPrice, FIX::MDEntryType_MID_PRICE),
    std::make_tuple(MdEntryType::Option::HighPrice, FIX::MDEntryType_TRADING_SESSION_HIGH_PRICE)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixMarketEntryActionConversion
    : public TestWithParam<std::tuple<MarketEntryAction, char>> {};

TEST_P(ToFixMarketEntryActionConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::MDUpdateAction>(internal_value),
            expected_fix_value);
}

TEST_F(ToFixMarketEntryActionConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::MDUpdateAction>(
                   static_cast<MarketEntryAction::Option>(0xFF)),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixMarketEntryActionConversion,
  Values(
    std::make_tuple(MarketEntryAction::Option::New, FIX::MDUpdateAction_NEW),
    std::make_tuple(MarketEntryAction::Option::Change, FIX::MDUpdateAction_CHANGE),
    std::make_tuple(MarketEntryAction::Option::Delete, FIX::MDUpdateAction_DELETE)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixTradingPhaseConversion
    : public TestWithParam<std::tuple<TradingPhase, std::string>> {};

TEST_P(ToFixTradingPhaseConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::TradingSessionSubID>(internal_value),
            expected_fix_value);
}

TEST_F(ToFixTradingPhaseConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::TradingSessionSubID>(
                   static_cast<TradingPhase::Option>(0xFF)),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixTradingPhaseConversion,
  Values(
    std::make_tuple(TradingPhase::Option::Open, FIX::TradingSessionSubID_OPEN),
    std::make_tuple(TradingPhase::Option::Closed, FIX::TradingSessionSubID_CLOSED),
    std::make_tuple(TradingPhase::Option::PostTrading, FIX::TradingSessionSubID_POST_TRADING),
    std::make_tuple(TradingPhase::Option::OpeningAuction, FIX::TradingSessionSubID_OPENING_OR_OPENING_AUCTION),
    std::make_tuple(TradingPhase::Option::IntradayAuction, FIX::TradingSessionSubID_INTRADAY_AUCTION),
    std::make_tuple(TradingPhase::Option::ClosingAuction, FIX::TradingSessionSubID_CLOSING_OR_CLOSING_AUCTION)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixTradingStatusConversion
    : public TestWithParam<std::tuple<TradingStatus, int>> {};

TEST_P(ToFixTradingStatusConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::SecurityTradingStatus>(internal_value),
            expected_fix_value);
}

TEST_F(ToFixTradingStatusConversion, ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::SecurityTradingStatus>(
                   static_cast<TradingStatus::Option>(0xFF)),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixTradingStatusConversion,
  Values(
    std::make_tuple(TradingStatus::Option::Halt, FIX::SecurityTradingStatus_TRADING_HALT),
    std::make_tuple(TradingStatus::Option::Resume, FIX::SecurityTradingStatus_RESUME)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixRejectedMessageTypeConversion
    : public TestWithParam<std::tuple<RejectedMessageType, std::string>> {};

TEST_P(ToFixRejectedMessageTypeConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::RefMsgType>(internal_value),
            expected_fix_value);
}

TEST_F(ToFixRejectedMessageTypeConversion,
       ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::RefMsgType>(
                   static_cast<RejectedMessageType::Option>(0xFF)),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixRejectedMessageTypeConversion,
  Values(
    std::make_tuple(RejectedMessageType::Option::SecurityStatusRequest, FIX::MsgType_SecurityStatusRequest)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct ToFixBusinessRejectReasonConversion
    : public TestWithParam<std::tuple<BusinessRejectReason, int>> {};

TEST_P(ToFixBusinessRejectReasonConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::BusinessRejectReason>(internal_value),
            expected_fix_value);
}

TEST_F(ToFixBusinessRejectReasonConversion,
       ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::BusinessRejectReason>(
                   static_cast<BusinessRejectReason::Option>(0xFF)),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, ToFixBusinessRejectReasonConversion,
  Values(
    std::make_tuple(BusinessRejectReason::Option::Other, FIX::BusinessRejectReason_OTHER),
    std::make_tuple(BusinessRejectReason::Option::UnknownId, FIX::BusinessRejectReason_UNKNOWN_ID),
    std::make_tuple(BusinessRejectReason::Option::UnknownSecurity, FIX::BusinessRejectReason_UNKNOWN_SECURITY)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

}  // namespace
}  // namespace simulator::fix::test