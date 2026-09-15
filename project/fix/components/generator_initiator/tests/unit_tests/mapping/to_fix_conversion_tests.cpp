#include <gtest/gtest.h>
#include <quickfix/Fields.h>
#include <quickfix/FixValues.h>

#include <chrono>
#include <limits>
#include <stdexcept>
#include <tuple>

#include "common/custom_values.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"
#include "ih/mapping/detail/to_fix_conversion.hpp"
#include "test_utils/utils.hpp"

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

namespace simulator::fix::generator_initiator::test {
namespace {

using namespace std::chrono_literals;
using namespace testing;  // NOLINT

TEST(InitiatorToFixIntegerConversion, ConvertsSignedToFix) {
  ASSERT_EQ(convert_to_fix<FIX::TestIntField>(-42), -42);
}

TEST(InitiatorToFixIntegerConversion, ReportsOverflowOnSignedConversion) {
  constexpr auto overflow_value = std::numeric_limits<std::int64_t>::max();

  ASSERT_THROW(convert_to_fix<FIX::TestIntField>(overflow_value),
               std::invalid_argument);
}

TEST(InitiatorToFixIntegerConversion, ReportsUnderflowOnSignedConversion) {
  constexpr auto overflow_value = std::numeric_limits<std::int64_t>::min();

  ASSERT_THROW(convert_to_fix<FIX::TestIntField>(overflow_value),
               std::invalid_argument);
}

TEST(InitiatorToFixIntegerConversion, ConvertsUnsignedToFix) {
  ASSERT_EQ(convert_to_fix<FIX::TestIntField>(142000u), 142000u);
}

TEST(InitiatorToFixIntegerConversion, ReportsOverflowOnUnsignedConversion) {
  constexpr auto overflow_value = std::numeric_limits<std::uint64_t>::max();

  ASSERT_THROW(convert_to_fix<FIX::TestIntField>(overflow_value),
               std::invalid_argument);
}

TEST(InitiatorToFixFloatingPointConversion, CovertsDoubleToFix) {
  ASSERT_DOUBLE_EQ(convert_to_fix<FIX::TestFloatField>(3.14), 3.14);
}

TEST(InitiatorToFixStringConversion, ConvertsStringToFix) {
  ASSERT_EQ(convert_to_fix<FIX::TestStringField>("test"), "test");
}

TEST(InitiatorToFixLocalDateConversion, ConvertsLocalDaysToFix) {
  const auto fix_field = convert_to_fix<FIX::MaturityDate>(
      std::chrono::local_days{2022y / 12 / 31});

  ASSERT_EQ(fix_field.getValue(), "20221231");
}

TEST(InitiatorToFixLocalDateConversion, AddsZerosToMonthAndDay) {
  const auto fix_field =
      convert_to_fix<FIX::MaturityDate>(std::chrono::local_days{2022y / 1 / 1});

  ASSERT_EQ(fix_field.getValue(), "20220101");
}

TEST(InitiatorToFixUtcDateConversion, ConvertsSystemMicrosecondsToFix) {
  // 2024-05-28 10:31:58 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318000000us};

  const auto fix_field = convert_to_fix<FIX::TestUtcDateField>(timestamp);

  ASSERT_EQ(fix_field.getString(), "20240528");
}

TEST(InitiatorToFixUtcTimeOnlyConversion,
     ConvertsSystemMicrosecondsToFixSeconds) {
  // 2024-05-28 10:31:58.999999 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318999999us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeOnlyField>(
      timestamp, TimestampPrecision::Seconds);

  ASSERT_EQ(fix_field.getString(), "10:31:58");
}

TEST(InitiatorToFixUtcTimeOnlyConversion,
     ConvertsSystemMicrosecondsToFixMilliseconds) {
  // 2024-05-28 10:31:58.123999 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318123999us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeOnlyField>(
      timestamp, TimestampPrecision::Milliseconds);

  ASSERT_EQ(fix_field.getString(), "10:31:58.123");
}

TEST(InitiatorToFixUtcTimeOnlyConversion,
     ConvertsSystemMicrosecondsToFixMicroseconds) {
  // 2024-05-28 10:31:58.123456 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318123456us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeOnlyField>(
      timestamp, TimestampPrecision::Microseconds);

  ASSERT_EQ(fix_field.getString(), "10:31:58.123456");
}

TEST(InitiatorToFixUtcTimeStampConversion,
     ConvertsSystemMicrosecondsToFixSeconds) {
  // 2024-05-28 10:31:58.999999 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318999999us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeStampField>(
      timestamp, TimestampPrecision::Seconds);

  ASSERT_EQ(fix_field.getString(), "20240528-10:31:58");
}

TEST(InitiatorToFixUtcTimeStampConversion,
     ConvertsSystemMicrosecondsToFixMilliseconds) {
  // 2024-05-28 10:31:58.123999 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318123999us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeStampField>(
      timestamp, TimestampPrecision::Milliseconds);

  ASSERT_EQ(fix_field.getString(), "20240528-10:31:58.123");
}

TEST(InitiatorToFixUtcTimeStampConversion,
     ConvertsSystemMicrosecondsToFixMicroseconds) {
  // 2024-05-28 10:31:58.123456 GMT
  constexpr auto timestamp = core::sys_microseconds{1716892318123456us};

  const auto fix_field = convert_to_fix<FIX::TestUtcTimeStampField>(
      timestamp, TimestampPrecision::Microseconds);

  ASSERT_EQ(fix_field.getString(), "20240528-10:31:58.123456");
}

struct InitiatorToFixSecurityTypeConversion
    : public TestWithParam<std::tuple<SecurityType, std::string>> {};

TEST_P(InitiatorToFixSecurityTypeConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::SecurityType>(internal_value),
            expected_fix_value);
}

TEST_F(InitiatorToFixSecurityTypeConversion,
       ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::SecurityType>(
                   invalid_enum_value<SecurityType::Option>()),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, InitiatorToFixSecurityTypeConversion,
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

struct InitiatorToFixSecurityIdSourceConversion
    : public TestWithParam<std::tuple<SecurityIdSource, std::string>> {};

TEST_P(InitiatorToFixSecurityIdSourceConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::SecurityIDSource>(internal_value),
            expected_fix_value);
}

TEST_F(InitiatorToFixSecurityIdSourceConversion,
       ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::SecurityIDSource>(
                   invalid_enum_value<SecurityIdSource::Option>()),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, InitiatorToFixSecurityIdSourceConversion,
  Values(
    std::make_tuple(SecurityIdSource::Option::Cusip, FIX::SecurityIDSource_CUSIP),
    std::make_tuple(SecurityIdSource::Option::Sedol, FIX::SecurityIDSource_SEDOL),
    std::make_tuple(SecurityIdSource::Option::Isin, FIX::SecurityIDSource_ISIN_NUMBER),
    std::make_tuple(SecurityIdSource::Option::Ric, FIX::SecurityIDSource_RIC_CODE),
    std::make_tuple(SecurityIdSource::Option::ExchangeSymbol, FIX::SecurityIDSource_EXCHANGE_SYMBOL),
    std::make_tuple(SecurityIdSource::Option::BloombergSymbol, FIX::SecurityIDSource_BLOOMBERG_SYMBOL)
  ));
// clang-format on

struct InitiatorToFixPartyIdSourceConversion
    : public TestWithParam<std::tuple<PartyIdSource, char>> {};

TEST_P(InitiatorToFixPartyIdSourceConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::PartyIDSource>(internal_value),
            expected_fix_value);
}

TEST_F(InitiatorToFixPartyIdSourceConversion,
       ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::PartyIDSource>(
                   invalid_enum_value<PartyIdSource::Option>()),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, InitiatorToFixPartyIdSourceConversion,
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
    std::make_tuple(PartyIdSource::Option::MasterSPSAID, FIX::PartyIDSource_MASTER_SPASAID),
    std::make_tuple(PartyIdSource::Option::KoreaShortSellingRegistrationNumber, FIX::PartyIDSource_KOREA_SHORT_SELLING_REGISTRATION_NUMBER)
  ));
// clang-format on

struct InitiatorToFixPartyRoleConversion
    : public TestWithParam<std::tuple<PartyRole, int>> {};

TEST_P(InitiatorToFixPartyRoleConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::PartyRole>(internal_value), expected_fix_value);
}

TEST_F(InitiatorToFixPartyRoleConversion,
       ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(
      convert_to_fix<FIX::PartyRole>(invalid_enum_value<PartyRole::Option>()),
      std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, InitiatorToFixPartyRoleConversion,
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
    std::make_tuple(PartyRole::Option::GiveupClearingFirmDepr, FIX::PartyRole_GIVEUP_CLEARING_FIRM_DEPR),
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
    std::make_tuple(PartyRole::Option::Buyer, FIX::PartyRole_BUYER_SELLER),
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
    std::make_tuple(PartyRole::Option::ContraInvestorID, FIX::PartyRole_CONTRA_INVESTOR_ID),
    std::make_tuple(PartyRole::Option::TransferToFirm, FIX::PartyRole_TRANSFER_TO_FIRM),
    std::make_tuple(PartyRole::Option::ContraPositionAccount, FIX::PartyRole_CONTRA_POSITION_ACCOUNT),
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
    std::make_tuple(PartyRole::Option::UnacceptableSettlingCounterparty, FIX::PartyRole_UNACCEPTABLE_SETTLING_COUNTERPARTY),
    std::make_tuple(PartyRole::Option::CLSMemberBank, FIX::PartyRole_CLS_MEMBER_BANK),
    std::make_tuple(PartyRole::Option::InConcertGroup, FIX::PartyRole_IN_CONCERT_GROUP),
    std::make_tuple(PartyRole::Option::InConcertControllingEntity, FIX::PartyRole_IN_CONCERT_CONTROLLING_ENTITY),
    std::make_tuple(PartyRole::Option::LargePositionsReportingAccount, FIX::PartyRole_LARGE_POSITIONS_REPORTING_ACCOUNT),
    std::make_tuple(PartyRole::Option::SettlementFirm, FIX::PartyRole_SETTLEMENT_FIRM),
    std::make_tuple(PartyRole::Option::SettlementAccount, FIX::PartyRole_SETTLEMENT_ACCOUNT),
    std::make_tuple(PartyRole::Option::ReportingMarketCenter, FIX::PartyRole_REPORTING_MARKET_CENTER),
    std::make_tuple(PartyRole::Option::RelatedReportingMarketCenter, FIX::PartyRole_RELATED_REPORTING_MARKET_CENTER),
    std::make_tuple(PartyRole::Option::AwayMarket, FIX::PartyRole_AWAY_MARKET),
    std::make_tuple(PartyRole::Option::GiveupTradingFirm, FIX::PartyRole_GIVEUP_TRADING_FIRM),
    std::make_tuple(PartyRole::Option::TakeupTradingFirm, FIX::PartyRole_TAKEUP_TRADING_FIRM),
    std::make_tuple(PartyRole::Option::GiveupClearingFirm, FIX::NEW_PartyRole_GIVEUP_CLEARING_FIRM),
    std::make_tuple(PartyRole::Option::TakeupClearingFirm, FIX::PartyRole_TAKEUP_CLEARING_FIRM),
    std::make_tuple(PartyRole::Option::OriginatingMarket, FIX::PartyRole_ORIGINATING_MARKET),
    std::make_tuple(PartyRole::Option::MarginAccount, FIX::PartyRole_MARGIN_ACCOUNT),
    std::make_tuple(PartyRole::Option::CollateralAssetAccount, FIX::PartyRole_COLLATERAL_ASSET_ACCOUNT),
    std::make_tuple(PartyRole::Option::DataRepository, FIX::PartyRole_DATA_REPOSITORY),
    std::make_tuple(PartyRole::Option::CalculationAgent, FIX::PartyRole_CALCULATION_AGENT),
    std::make_tuple(PartyRole::Option::ExerciseNoticeSender, FIX::PartyRole_EXERCISE_NOTICE_SENDER),
    std::make_tuple(PartyRole::Option::ExerciseNoticeReceiver, FIX::PartyRole_EXERCISE_NOTICE_RECEIVER),
    std::make_tuple(PartyRole::Option::RateReferenceBank, FIX::PartyRole_RATE_REFERENCE_BANK),
    std::make_tuple(PartyRole::Option::Correspondent, FIX::PartyRole_CORRESPONDENT),
    std::make_tuple(PartyRole::Option::BeneficiaryBank, FIX::PartyRole_BENEFICIARY_BANK),
    std::make_tuple(PartyRole::Option::Borrower, FIX::PartyRole_BORROWER),
    std::make_tuple(PartyRole::Option::PrimaryObligator, FIX::PartyRole_PRIMARY_OBLIGATOR),
    std::make_tuple(PartyRole::Option::Guarantor, FIX::PartyRole_GUARANTOR),
    std::make_tuple(PartyRole::Option::ExcludedReferenceEntity, FIX::PartyRole_EXCLUDED_REFERENCE_ENTITY),
    std::make_tuple(PartyRole::Option::DeterminingParty, FIX::PartyRole_DETERMINING_PARTY),
    std::make_tuple(PartyRole::Option::HedgingParty, FIX::PartyRole_HEDGING_PARTY),
    std::make_tuple(PartyRole::Option::ReportingEntity, FIX::PartyRole_REPORTING_ENTITY),
    std::make_tuple(PartyRole::Option::SalesPerson, FIX::PartyRole_SALES_PERSON),
    std::make_tuple(PartyRole::Option::Operator, FIX::PartyRole_OPERATOR),
    std::make_tuple(PartyRole::Option::CSD, FIX::PartyRole_CSD),
    std::make_tuple(PartyRole::Option::ICSD, FIX::PartyRole_ICSD),
    std::make_tuple(PartyRole::Option::TradingSubAccount, FIX::PartyRole_TRADING_SUB_ACCOUNT),
    std::make_tuple(PartyRole::Option::InvestmentDecisionMaker, FIX::PartyRole_INVESTMENT_DECISION_MAKER),
    std::make_tuple(PartyRole::Option::PublishingIntermediary, FIX::PartyRole_PUBLISHING_INTERMEDIARY),
    std::make_tuple(PartyRole::Option::CSDParticipant, FIX::PartyRole_CSD_PARTICIPANT),
    std::make_tuple(PartyRole::Option::Issuer, FIX::PartyRole_ISSUER),
    std::make_tuple(PartyRole::Option::ContraCustomerAccount, FIX::PartyRole_CONTRA_CUSTOMER_ACCOUNT),
    std::make_tuple(PartyRole::Option::ContraInvestmentDecisionMaker, FIX::PartyRole_CONTRA_INVESTMENT_DECISION_MAKER),
    std::make_tuple(PartyRole::Option::AuthorizingPerson, FIX::PartyRole_AUTHORIZING_PERSON),
    std::make_tuple(PartyRole::Option::PrimaryPlaceOfListing, FIX::PartyRole_PRIMARY_PLACE_OF_LISTING),
    std::make_tuple(PartyRole::Option::SecondaryPlaceOfListing, FIX::PartyRole_SECONDARY_PLACE_OF_LISTING)
  ));
// clang-format on

struct InitiatorToFixMdEntryTypeConversion
    : public TestWithParam<std::tuple<MdEntryType, char>> {};

TEST_P(InitiatorToFixMdEntryTypeConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::MDEntryType>(internal_value),
            expected_fix_value);
}

TEST_F(InitiatorToFixMdEntryTypeConversion,
       ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::MDEntryType>(
                   invalid_enum_value<MdEntryType::Option>()),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, InitiatorToFixMdEntryTypeConversion,
  Values(
    std::make_tuple(MdEntryType::Option::Bid, FIX::MDEntryType_BID),
    std::make_tuple(MdEntryType::Option::Offer, FIX::MDEntryType_OFFER),
    std::make_tuple(MdEntryType::Option::Trade, FIX::MDEntryType_TRADE),
    std::make_tuple(MdEntryType::Option::OpeningPrice, FIX::MDEntryType_OPENING_PRICE),
    std::make_tuple(MdEntryType::Option::ClosingPrice, FIX::MDEntryType_CLOSING_PRICE),
    std::make_tuple(MdEntryType::Option::SettlementPrice, FIX::MDEntryType_SETTLEMENT_PRICE),
    std::make_tuple(MdEntryType::Option::HighPrice, FIX::MDEntryType_TRADING_SESSION_HIGH_PRICE),
    std::make_tuple(MdEntryType::Option::LowPrice, FIX::MDEntryType_TRADING_SESSION_LOW_PRICE),
    std::make_tuple(MdEntryType::Option::Imbalance, FIX::MDEntryType_IMBALANCE),
    std::make_tuple(MdEntryType::Option::TradeVolume, FIX::MDEntryType_TRADE_VOLUME),
    std::make_tuple(MdEntryType::Option::MidPrice, FIX::MDEntryType_MID_PRICE),
    std::make_tuple(MdEntryType::Option::EarlyPrice, FIX::MDEntryType_EARLY_PRICES),
    std::make_tuple(MdEntryType::Option::AuctionClearingPrice, FIX::MDEntryType_AUCTION_CLEARING_PRICE),
    std::make_tuple(MdEntryType::Option::MarketBid, FIX::MDEntryType_MARKET_BID),
    std::make_tuple(MdEntryType::Option::MarketOffer, FIX::MDEntryType_MARKET_OFFER),
    std::make_tuple(MdEntryType::Option::PreviousClosingPrice, FIX::MDEntryType_PREVIOUS_CLOSING_PRICE)
  ));
// clang-format on

struct InitiatorToFixMarketDataUpdateTypeConversion
    : public TestWithParam<std::tuple<MarketDataUpdateType, int>> {};

TEST_P(InitiatorToFixMarketDataUpdateTypeConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::MDUpdateType>(internal_value),
            expected_fix_value);
}

TEST_F(InitiatorToFixMarketDataUpdateTypeConversion,
       ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::MDUpdateType>(
                   invalid_enum_value<MarketDataUpdateType::Option>()),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, InitiatorToFixMarketDataUpdateTypeConversion,
  Values(
    std::make_tuple(MarketDataUpdateType::Option::Snapshot, FIX::MDUpdateType_FULL_REFRESH),
    std::make_tuple(MarketDataUpdateType::Option::Incremental, FIX::MDUpdateType_INCREMENTAL_REFRESH)
  ));
// clang-format on

struct InitiatorToFixMdSubscriptionRequestTypeConversion
    : public TestWithParam<std::tuple<MdSubscriptionRequestType, char>> {};

TEST_P(InitiatorToFixMdSubscriptionRequestTypeConversion, ConvertsToFixValue) {
  const auto [internal_value, expected_fix_value] = GetParam();

  ASSERT_EQ(convert_to_fix<FIX::SubscriptionRequestType>(internal_value),
            expected_fix_value);
}

TEST_F(InitiatorToFixMdSubscriptionRequestTypeConversion,
       ReportsErrorOnUnknownValueConversion) {
  ASSERT_THROW(convert_to_fix<FIX::SubscriptionRequestType>(
                   invalid_enum_value<MdSubscriptionRequestType::Option>()),
               std::invalid_argument);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InternalEnum, InitiatorToFixMdSubscriptionRequestTypeConversion,
  Values(
    std::make_tuple(MdSubscriptionRequestType::Option::Subscribe, FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES),
    std::make_tuple(MdSubscriptionRequestType::Option::Unsubscribe, FIX::SubscriptionRequestType_DISABLE_PREVIOUS_SNAPSHOT_PLUS_UPDATE_REQUEST),
    std::make_tuple(MdSubscriptionRequestType::Option::Snapshot, FIX::SubscriptionRequestType_SNAPSHOT)
  ));
// clang-format on

}  // namespace
}  // namespace simulator::fix::generator_initiator::test