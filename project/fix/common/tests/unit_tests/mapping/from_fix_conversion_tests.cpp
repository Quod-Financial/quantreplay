#include <gtest/gtest.h>
#include <quickfix/Exceptions.h>
#include <quickfix/Values.h>

#include <chrono>
#include <string>
#include <tuple>

#include "common/custom_values.hpp"
#include "common/mapping/detail/from_fix_conversion.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"

namespace FIX {
namespace {
// NOLINTBEGIN(*explicit-constructor*)

USER_DEFINE_INT(TEST_INT_FIELD, 1);
USER_DEFINE_FLOAT(TEST_FLOAT_FIELD, 1);
USER_DEFINE_STRING(TEST_STRING_FIELD, 1);
USER_DEFINE_UTCTIMESTAMP(TEST_UTCTIMESTAMP_FIELD, 1);
USER_DEFINE_LOCALMKTDATE(TEST_LOCALMKTDATE_FIELD, 1);

// NOLINTEND(*explicit-constructor*)
}  // namespace
}  // namespace FIX

namespace simulator::fix::test {
namespace {

using namespace std::chrono_literals;
using namespace testing;  // NOLINT

/*----------------------------------------------------------------------------*/

TEST(FromFixIntegerConversion, ConvertsToSignedInteger) {
  ASSERT_EQ(convert_from_fix<std::int32_t>(FIX::TEST_INT_FIELD(42)), 42);
}

TEST(FromFixIntegerConversion, ReportsUndeflowWhenConvertsToSignedInteger) {
  ASSERT_THROW(convert_from_fix<std::int8_t>(FIX::TEST_INT_FIELD(-200)),
               FIX::IncorrectTagValue);
}

TEST(FromFixIntegerConversion, ReportsOverflowWhenConvertsToSignedInteger) {
  ASSERT_THROW(convert_from_fix<std::int8_t>(FIX::TEST_INT_FIELD(200)),
               FIX::IncorrectTagValue);
}

/*----------------------------------------------------------------------------*/

TEST(FromFixIntegerConversion, ConvertsToUnsignedInteger) {
  ASSERT_EQ(convert_from_fix<std::uint32_t>(FIX::TEST_INT_FIELD(42)), 42);
}

TEST(FromFixIntegerConversion, ReportsUnderflowWhenConvertsToUnsignedInteger) {
  ASSERT_THROW(convert_from_fix<std::uint16_t>(FIX::TEST_INT_FIELD(-1)),
               FIX::IncorrectTagValue);
}

TEST(FromFixIntegerConversion, ReportsOverflowWhenConvertsToUnsignedInteger) {
  ASSERT_THROW(convert_from_fix<std::uint8_t>(FIX::TEST_INT_FIELD(300)),
               FIX::IncorrectTagValue);
}

/*----------------------------------------------------------------------------*/

TEST(FromFixFloatConversion, ConvertsToDouble) {
  const FIX::TEST_FLOAT_FIELD fix_field = 42.42;

  ASSERT_DOUBLE_EQ(convert_from_fix<double>(fix_field), 42.42);
}

/*----------------------------------------------------------------------------*/

TEST(FromFixStringConversion, ConvertsToString) {
  const FIX::TEST_STRING_FIELD fix_field{"test"};

  ASSERT_EQ(convert_from_fix<std::string>(fix_field), "test");
}

/*----------------------------------------------------------------------------*/

TEST(FromFixUtcTimestampConversion, ConvertsToSystemTime) {
  constexpr auto FixTimestampPrecision = 6;  // microseconds precision

  // test timestamp is: 2024-09-01 12:01:02.138567
  // unix timestamp is: 1725192062138567 (us)
  const FIX::TEST_UTCTIMESTAMP_FIELD fix_field{
      FIX::UtcTimeStamp{12, 1, 2, 138567, 1, 9, 2024, FixTimestampPrecision},
      FixTimestampPrecision};

  ASSERT_EQ(
      convert_from_fix<core::sys_microseconds>(fix_field).time_since_epoch(),
      1725192062138567us);
}

/*----------------------------------------------------------------------------*/

TEST(FromFixLocalMktDateConversion, ConvertsToLocalDays) {
  // 2024-09-01
  const FIX::TEST_LOCALMKTDATE_FIELD fix_field{"20240901"};

  ASSERT_EQ(convert_from_fix<std::chrono::local_days>(fix_field),
            std::chrono::local_days{2024y / 9 / 1});
}

TEST(FromFixLocalMktDateConversion, ReportsErrorWhenDateLengthInvalid) {
  const FIX::TEST_LOCALMKTDATE_FIELD fix_field{"202409"};

  ASSERT_THROW(convert_from_fix<std::chrono::local_days>(fix_field),
               FIX::IncorrectTagValue);
}

TEST(FromFixLocalMktDateConversion, ReportsErrorWhenDateHasInvalidFormat) {
  const FIX::TEST_LOCALMKTDATE_FIELD fix_field{"2024-09-01"};

  ASSERT_THROW(convert_from_fix<std::chrono::local_days>(fix_field),
               FIX::IncorrectTagValue);
}

/*----------------------------------------------------------------------------*/

struct FromFixOrderTypeConversion
    : public TestWithParam<std::tuple<char, OrderType>> {};

TEST_P(FromFixOrderTypeConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<OrderType::Option>(FIX::OrdType(fix_value)));
}

TEST_F(FromFixOrderTypeConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(convert_from_fix<OrderType::Option>(FIX::OrdType('x')),
               FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, FromFixOrderTypeConversion,
  Values(
    std::make_tuple(FIX::OrdType_MARKET, OrderType::Option::Market),
    std::make_tuple(FIX::OrdType_LIMIT, OrderType::Option::Limit)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct FromFixSideConversion : public TestWithParam<std::tuple<char, Side>> {};

TEST_P(FromFixSideConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<Side::Option>(FIX::Side(fix_value)));
}

TEST_F(FromFixSideConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(convert_from_fix<Side::Option>(FIX::Side('x')),
               FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, FromFixSideConversion,
  Values(
    std::make_tuple(FIX::Side_BUY, Side::Option::Buy),
    std::make_tuple(FIX::Side_SELL, Side::Option::Sell),
    std::make_tuple(FIX::Side_SELL_SHORT, Side::Option::SellShort),
    std::make_tuple(FIX::Side_SELL_SHORT_EXEMPT, Side::Option::SellShortExempt)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct FromFixTimeInForceConversion
    : public TestWithParam<std::tuple<char, TimeInForce>> {};

TEST_P(FromFixTimeInForceConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<TimeInForce::Option>(FIX::TimeInForce(fix_value)));
}

TEST_F(FromFixTimeInForceConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(convert_from_fix<TimeInForce::Option>(FIX::TimeInForce('x')),
               FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, FromFixTimeInForceConversion,
  Values(
    std::make_tuple(FIX::TimeInForce_DAY, TimeInForce::Option::Day),
    std::make_tuple(FIX::TimeInForce_IMMEDIATE_OR_CANCEL, TimeInForce::Option::ImmediateOrCancel),
    std::make_tuple(FIX::TimeInForce_FILL_OR_KILL, TimeInForce::Option::FillOrKill),
    std::make_tuple(FIX::TimeInForce_GOOD_TILL_DATE, TimeInForce::Option::GoodTillDate),
    std::make_tuple(FIX::TimeInForce_GOOD_TILL_CANCEL, TimeInForce::Option::GoodTillCancel)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct FromFixSecurityTypeConversion
    : public TestWithParam<std::tuple<std::string, SecurityType>> {};

TEST_P(FromFixSecurityTypeConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(
      expected_internal_value,
      convert_from_fix<SecurityType::Option>(FIX::SecurityType(fix_value)));
}

TEST_F(FromFixSecurityTypeConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(convert_from_fix<SecurityType::Option>(FIX::SecurityType("x")),
               FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, FromFixSecurityTypeConversion,
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

/*----------------------------------------------------------------------------*/

struct FromFixSecurityIdSourceConversion
    : public TestWithParam<std::tuple<std::string, SecurityIdSource>> {};

TEST_P(FromFixSecurityIdSourceConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<SecurityIdSource::Option>(
                FIX::SecurityIDSource(fix_value)));
}

TEST_F(FromFixSecurityIdSourceConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(
      convert_from_fix<SecurityIdSource::Option>(FIX::SecurityIDSource("x")),
      FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, FromFixSecurityIdSourceConversion,
  Values(
    std::make_tuple(FIX::SecurityIDSource_CUSIP, SecurityIdSource::Option::Cusip),
    std::make_tuple(FIX::SecurityIDSource_SEDOL, SecurityIdSource::Option::Sedol),
    std::make_tuple(FIX::SecurityIDSource_ISIN_NUMBER, SecurityIdSource::Option::Isin),
    std::make_tuple(FIX::SecurityIDSource_RIC_CODE, SecurityIdSource::Option::Ric),
    std::make_tuple(FIX::SecurityIDSource_EXCHANGE_SYMBOL, SecurityIdSource::Option::ExchangeSymbol),
    std::make_tuple(FIX::SecurityIDSource_BLOOMBERG_SYMBOL, SecurityIdSource::Option::BloombergSymbol)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct FromFixPartyIdSourceConversion
    : public TestWithParam<std::tuple<char, PartyIdSource>> {};

TEST_P(FromFixPartyIdSourceConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(
      expected_internal_value,
      convert_from_fix<PartyIdSource::Option>(FIX::PartyIDSource(fix_value)));
}

TEST_F(FromFixPartyIdSourceConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(convert_from_fix<PartyIdSource::Option>(FIX::PartyIDSource('x')),
               FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, FromFixPartyIdSourceConversion,
  Values(
    std::make_tuple(FIX::PartyIDSource_UK_NATIONAL_INSURANCE_OR_PENSION_NUMBER, PartyIdSource::Option::UKNationalInsuranceOrPensionNumber),
    std::make_tuple(FIX::PartyIDSource_US_SOCIAL_SECURITY_NUMBER, PartyIdSource::Option::USSocialSecurityNumber),
    std::make_tuple(FIX::PartyIDSource_US_EMPLOYER_OR_TAX_ID_NUMBER, PartyIdSource::Option::USEmployerOrTaxIDNumber),
    std::make_tuple(FIX::PartyIDSource_AUSTRALIAN_BUSINESS_NUMBER, PartyIdSource::Option::AustralianBusinessNumber),
    std::make_tuple(FIX::PartyIDSource_AUSTRALIAN_TAX_FILE_NUMBER, PartyIdSource::Option::AustralianTaxFileNumber),
    std::make_tuple(FIX::PartyIDSource_TAX_ID, PartyIdSource::Option::TaxID),
    std::make_tuple(FIX::PartyIDSource_KOREAN_INVESTOR_ID, PartyIdSource::Option::KoreanInvestorID),
    std::make_tuple(FIX::PartyIDSource_TAIWANESE_QUALIFIED_FOREIGN_INVESTOR_ID_QFII_FID, PartyIdSource::Option::TaiwaneseForeignInvestorID),
    std::make_tuple(FIX::PartyIDSource_TAIWANESE_TRADING_ACCOUNT, PartyIdSource::Option::TaiwaneseTradingAcct),
    std::make_tuple(FIX::PartyIDSource_MALAYSIAN_CENTRAL_DEPOSITORY, PartyIdSource::Option::MalaysianCentralDepository),
    std::make_tuple(FIX::PartyIDSource_CHINESE_INVESTOR_ID, PartyIdSource::Option::ChineseInvestorID),
    std::make_tuple(FIX::PartyIDSource_DIRECTED_BROKER_THREE_CHARACTER_ACRONYM_AS_DEFINED_IN_ISITC_ETC_BEST_PRACTICE_GUIDELINES_DOCUMENT, PartyIdSource::Option::ISITCAcronym),
    std::make_tuple(FIX::PartyIDSource_BIC, PartyIdSource::Option::BIC),
    std::make_tuple(FIX::PartyIDSource_GENERALLY_ACCEPTED_MARKET_PARTICIPANT_IDENTIFIER, PartyIdSource::Option::GeneralIdentifier),
    std::make_tuple(FIX::PartyIDSource_PROPRIETARY, PartyIdSource::Option::Proprietary),
    std::make_tuple(FIX::PartyIDSource_ISO_COUNTRY_CODE, PartyIdSource::Option::ISOCountryCode),
    std::make_tuple(FIX::PartyIDSource_SETTLEMENT_ENTITY_LOCATION, PartyIdSource::Option::SettlementEntityLocation),
    std::make_tuple(FIX::PartyIDSource_MIC, PartyIdSource::Option::MIC),
    std::make_tuple(FIX::PartyIDSource_CSD_PARTICIPANT_MEMBER_CODE, PartyIdSource::Option::CSDParticipant),
    std::make_tuple(FIX::PartyIDSource_AUSTRALIAN_COMPANY_NUMBER, PartyIdSource::Option::AustralianCompanyNumber),
    std::make_tuple(FIX::PartyIDSource_AUSTRALIAN_REGISTERED_BODY_NUMBER, PartyIdSource::Option::AustralianRegisteredBodyNumber),
    std::make_tuple(FIX::PartyIDSource_CFTC_REPORTING_FIRM_IDENTIFIER, PartyIdSource::Option::CFTCReportingFirmIdentifier),
    std::make_tuple(FIX::PartyIDSource_LEI, PartyIdSource::Option::LegalEntityIdentifier),
    std::make_tuple(FIX::PartyIDSource_INTERIM_IDENTIFIER, PartyIdSource::Option::InterimIdentifier),
    std::make_tuple(FIX::PartyIDSource_SHORT_CODE_IDENTIFIER, PartyIdSource::Option::ShortCodeIdentifier),
    std::make_tuple(FIX::PartyIDSource_NATIONAL_ID_NATURAL_PERSON, PartyIdSource::Option::NationalIDNaturalPerson),
    std::make_tuple(FIX::PartyIDSource_INDIA_PERMANENT_ACCOUNT_NUMBER, PartyIdSource::Option::IndiaPermanentAccountNumber),
    std::make_tuple(FIX::PartyIDSource_FDID, PartyIdSource::Option::FDID),
    std::make_tuple(FIX::PartyIDSource_SPASAID, PartyIdSource::Option::SPSAID),
    std::make_tuple(FIX::PartyIDSource_MASTER_SPASAID, PartyIdSource::Option::MasterSPSAID)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct FromFixPartyRoleConversion
    : public TestWithParam<std::tuple<int, PartyRole>> {};

TEST_P(FromFixPartyRoleConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<PartyRole::Option>(FIX::PartyRole(fix_value)));
}

TEST_F(FromFixPartyRoleConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(convert_from_fix<PartyRole::Option>(FIX::PartyRole(-1)),
               FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, FromFixPartyRoleConversion,
  Values(
    std::make_tuple(FIX::PartyRole_EXECUTING_FIRM, PartyRole::Option::ExecutingFirm),
    std::make_tuple(FIX::PartyRole_BROKER_OF_CREDIT, PartyRole::Option::BrokerOfCredit),
    std::make_tuple(FIX::PartyRole_CLIENT_ID, PartyRole::Option::ClientID),
    std::make_tuple(FIX::PartyRole_CLEARING_FIRM, PartyRole::Option::ClearingFirm),
    std::make_tuple(FIX::PartyRole_INVESTOR_ID, PartyRole::Option::InvestorID),
    std::make_tuple(FIX::PartyRole_INTRODUCING_FIRM, PartyRole::Option::IntroducingFirm),
    std::make_tuple(FIX::PartyRole_ENTERING_FIRM, PartyRole::Option::EnteringFirm),
    std::make_tuple(FIX::PartyRole_LOCATE, PartyRole::Option::Locate),
    std::make_tuple(FIX::PartyRole_FUND_MANAGER_CLIENT_ID, PartyRole::Option::FundManagerClientID),
    std::make_tuple(FIX::PartyRole_SETTLEMENT_LOCATION, PartyRole::Option::SettlementLocation),
    std::make_tuple(FIX::PartyRole_ORDER_ORIGINATION_TRADER, PartyRole::Option::OrderOriginationTrader),
    std::make_tuple(FIX::PartyRole_EXECUTING_TRADER, PartyRole::Option::ExecutingTrader),
    std::make_tuple(FIX::PartyRole_ORDER_ORIGINATION_FIRM, PartyRole::Option::OrderOriginationFirm),
    std::make_tuple(FIX::PartyRole_GIVEUP_CLEARING_FIRM, PartyRole::Option::GiveupClearingFirm),
    std::make_tuple(FIX::PartyRole_CORRESPONDANT_CLEARING_FIRM, PartyRole::Option::CorrespondantClearingFirm),
    std::make_tuple(FIX::PartyRole_EXECUTING_SYSTEM, PartyRole::Option::ExecutingSystem),
    std::make_tuple(FIX::PartyRole_CONTRA_FIRM, PartyRole::Option::ContraFirm),
    std::make_tuple(FIX::PartyRole_CONTRA_CLEARING_FIRM, PartyRole::Option::ContraClearingFirm),
    std::make_tuple(FIX::PartyRole_SPONSORING_FIRM, PartyRole::Option::SponsoringFirm),
    std::make_tuple(FIX::PartyRole_UNDERLYING_CONTRA_FIRM, PartyRole::Option::UnderlyingContraFirm),
    std::make_tuple(FIX::PartyRole_CLEARING_ORGANIZATION, PartyRole::Option::ClearingOrganization),
    std::make_tuple(FIX::PartyRole_EXCHANGE, PartyRole::Option::Exchange),
    std::make_tuple(FIX::PartyRole_CUSTOMER_ACCOUNT, PartyRole::Option::CustomerAccount),
    std::make_tuple(FIX::PartyRole_CORRESPONDENT_CLEARING_ORGANIZATION, PartyRole::Option::CorrespondentClearingOrganization),
    std::make_tuple(FIX::PartyRole_CORRESPONDENT_BROKER, PartyRole::Option::CorrespondentBroker),
    std::make_tuple(FIX::PartyRole_CUSTODIAN, PartyRole::Option::Custodian),
    std::make_tuple(FIX::PartyRole_INTERMEDIARY, PartyRole::Option::Intermediary),
    std::make_tuple(FIX::PartyRole_AGENT, PartyRole::Option::Agent),
    std::make_tuple(FIX::PartyRole_SUB_CUSTODIAN, PartyRole::Option::SubCustodian),
    std::make_tuple(FIX::PartyRole_BENEFICIARY, PartyRole::Option::Beneficiary),
    std::make_tuple(FIX::PartyRole_INTERESTED_PARTY, PartyRole::Option::InterestedParty),
    std::make_tuple(FIX::PartyRole_REGULATORY_BODY, PartyRole::Option::RegulatoryBody),
    std::make_tuple(FIX::PartyRole_LIQUIDITY_PROVIDER, PartyRole::Option::LiquidityProvider),
    std::make_tuple(FIX::PartyRole_ENTERING_TRADER, PartyRole::Option::EnteringTrader),
    std::make_tuple(FIX::PartyRole_CONTRA_TRADER, PartyRole::Option::ContraTrader),
    std::make_tuple(FIX::PartyRole_POSITION_ACCOUNT, PartyRole::Option::PositionAccount),
    std::make_tuple(FIX::PartyRole_CONTRA_EXCHANGE, PartyRole::Option::ContraExchange),
    std::make_tuple(FIX::PartyRole_INTERNAL_CARRY_ACCOUNT, PartyRole::Option::InternalCarryAccount),
    std::make_tuple(FIX::PartyRole_ORDER_ENTRY_OPERATOR_ID, PartyRole::Option::OrderEntryOperatorID),
    std::make_tuple(FIX::PartyRole_SECONDARY_ACCOUNT_NUMBER, PartyRole::Option::SecondaryAccountNumber),
    std::make_tuple(FIX::PartyRole_FOREIGN_FIRM, PartyRole::Option::ForeignFirm),
    std::make_tuple(FIX::PartyRole_THIRD_PARTY_ALLOCATION_FIRM, PartyRole::Option::ThirdPartyAllocationFirm),
    std::make_tuple(FIX::PartyRole_CLAIMING_ACCOUNT, PartyRole::Option::ClaimingAccount),
    std::make_tuple(FIX::PartyRole_ASSET_MANAGER, PartyRole::Option::AssetManager),
    std::make_tuple(FIX::PartyRole_PLEDGOR_ACCOUNT, PartyRole::Option::PledgorAccount),
    std::make_tuple(FIX::PartyRole_PLEDGEE_ACCOUNT, PartyRole::Option::PledgeeAccount),
    std::make_tuple(FIX::PartyRole_LARGE_TRADER_REPORTABLE_ACCOUNT, PartyRole::Option::LargeTraderReportableAccount),
    std::make_tuple(FIX::PartyRole_TRADER_MNEMONIC, PartyRole::Option::TraderMnemonic),
    std::make_tuple(FIX::PartyRole_SENDER_LOCATION, PartyRole::Option::SenderLocation),
    std::make_tuple(FIX::PartyRole_SESSION_ID, PartyRole::Option::SessionID),
    std::make_tuple(FIX::PartyRole_ACCEPTABLE_COUNTERPARTY, PartyRole::Option::AcceptableCounterparty),
    std::make_tuple(FIX::PartyRole_UNACCEPTABLE_COUNTERPARTY, PartyRole::Option::UnacceptableCounterparty),
    std::make_tuple(FIX::PartyRole_ENTERING_UNIT, PartyRole::Option::EnteringUnit),
    std::make_tuple(FIX::PartyRole_EXECUTING_UNIT, PartyRole::Option::ExecutingUnit),
    std::make_tuple(FIX::PartyRole_INTRODUCING_BROKER, PartyRole::Option::IntroducingBroker),
    std::make_tuple(FIX::PartyRole_QUOTE_ORIGINATOR, PartyRole::Option::QuoteOriginator),
    std::make_tuple(FIX::PartyRole_REPORT_ORIGINATOR, PartyRole::Option::ReportOriginator),
    std::make_tuple(FIX::PartyRole_SYSTEMATIC_INTERNALISER, PartyRole::Option::SystematicInternaliser),
    std::make_tuple(FIX::PartyRole_MULTILATERAL_TRADING_FACILITY, PartyRole::Option::MultilateralTradingFacility),
    std::make_tuple(FIX::PartyRole_REGULATED_MARKET, PartyRole::Option::RegulatedMarket),
    std::make_tuple(FIX::PartyRole_MARKET_MAKER, PartyRole::Option::MarketMaker),
    std::make_tuple(FIX::PartyRole_INVESTMENT_FIRM, PartyRole::Option::InvestmentFirm),
    std::make_tuple(FIX::PartyRole_HOST_COMPETENT_AUTHORITY, PartyRole::Option::HostCompetentAuthority),
    std::make_tuple(FIX::PartyRole_HOME_COMPETENT_AUTHORITY, PartyRole::Option::HomeCompetentAuthority),
    std::make_tuple(FIX::PartyRole_COMPETENT_AUTHORITY_OF_THE_MOST_RELEVANT_MARKET_IN_TERMS_OF_LIQUIDITY, PartyRole::Option::CompetentAuthorityLiquidity),
    std::make_tuple(FIX::PartyRole_COMPETENT_AUTHORITY_OF_THE_TRANSACTION, PartyRole::Option::CompetentAuthorityTransactionVenue),
    std::make_tuple(FIX::PartyRole_REPORTING_INTERMEDIARY, PartyRole::Option::ReportingIntermediary),
    std::make_tuple(FIX::PartyRole_EXECUTION_VENUE, PartyRole::Option::ExecutionVenue),
    std::make_tuple(FIX::PartyRole_MARKET_DATA_ENTRY_ORIGINATOR, PartyRole::Option::MarketDataEntryOriginator),
    std::make_tuple(FIX::PartyRole_LOCATION_ID, PartyRole::Option::LocationID),
    std::make_tuple(FIX::PartyRole_DESK_ID, PartyRole::Option::DeskID),
    std::make_tuple(FIX::PartyRole_MARKET_DATA_MARKET, PartyRole::Option::MarketDataMarket),
    std::make_tuple(FIX::PartyRole_ALLOCATION_ENTITY, PartyRole::Option::AllocationEntity),
    std::make_tuple(FIX::PartyRole_PRIME_BROKER_PROVIDING_GENERAL_TRADE_SERVICES, PartyRole::Option::PrimeBroker),
    std::make_tuple(FIX::PartyRole_STEP_OUT_FIRM, PartyRole::Option::StepOutFirm),
    std::make_tuple(FIX::PartyRole_BROKERCLEARINGID, PartyRole::Option::BrokerClearingID),
    std::make_tuple(FIX::PartyRole_CENTRAL_REGISTRATION_DEPOSITORY, PartyRole::Option::CentralRegistrationDepository),
    std::make_tuple(FIX::PartyRole_CLEARING_ACCOUNT, PartyRole::Option::ClearingAccount),
    std::make_tuple(FIX::PartyRole_ACCEPTABLE_SETTLING_COUNTERPARTY, PartyRole::Option::AcceptableSettlingCounterparty),
    std::make_tuple(FIX::PartyRole_UNACCEPTABLE_SETTLING_COUNTERPARTY, PartyRole::Option::UnacceptableSettlingCounterparty)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct FromFixMdEntryTypeConversion
    : public TestWithParam<std::tuple<char, MdEntryType>> {};

TEST_P(FromFixMdEntryTypeConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<MdEntryType::Option>(FIX::MDEntryType(fix_value)));
}

TEST_F(FromFixMdEntryTypeConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(convert_from_fix<MdEntryType::Option>(FIX::MDEntryType('x')),
               FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, FromFixMdEntryTypeConversion,
  Values(
    std::make_tuple(FIX::MDEntryType_BID, MdEntryType::Option::Bid),
    std::make_tuple(FIX::MDEntryType_OFFER, MdEntryType::Option::Offer),
    std::make_tuple(FIX::MDEntryType_TRADE, MdEntryType::Option::Trade),
    std::make_tuple(FIX::MDEntryType_TRADING_SESSION_LOW_PRICE, MdEntryType::Option::LowPrice),
    std::make_tuple(FIX::MDEntryType_MID_PRICE, MdEntryType::Option::MidPrice),
    std::make_tuple(FIX::MDEntryType_TRADING_SESSION_HIGH_PRICE, MdEntryType::Option::HighPrice)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct FromFixMarketDataUpdateTypeConversion
    : public TestWithParam<std::tuple<int, MarketDataUpdateType>> {};

TEST_P(FromFixMarketDataUpdateTypeConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<MarketDataUpdateType::Option>(
                FIX::MDUpdateType(fix_value)));
}

TEST_F(FromFixMarketDataUpdateTypeConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(
      convert_from_fix<MarketDataUpdateType::Option>(FIX::MDUpdateType(-1)),
      FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, FromFixMarketDataUpdateTypeConversion,
  Values(
    std::make_tuple(FIX::MDUpdateType_FULL_REFRESH, MarketDataUpdateType::Option::Snapshot),
    std::make_tuple(FIX::MDUpdateType_INCREMENTAL_REFRESH, MarketDataUpdateType::Option::Incremental)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

struct FromFixMdSubscriptionRequestTypeConversion
    : public TestWithParam<std::tuple<char, MdSubscriptionRequestType>> {};

TEST_P(FromFixMdSubscriptionRequestTypeConversion, ConvertsSupportedFixValue) {
  const auto& [fix_value, expected_internal_value] = GetParam();

  ASSERT_EQ(expected_internal_value,
            convert_from_fix<MdSubscriptionRequestType::Option>(
                FIX::SubscriptionRequestType(fix_value)));
}

TEST_F(FromFixMdSubscriptionRequestTypeConversion, ReportsUnknownFixValue) {
  ASSERT_THROW(convert_from_fix<MdSubscriptionRequestType::Option>(
                   FIX::SubscriptionRequestType('x')),
               FIX::IncorrectTagValue);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(FixValues, FromFixMdSubscriptionRequestTypeConversion,
  Values(
    std::make_tuple(FIX::SubscriptionRequestType_SNAPSHOT, MdSubscriptionRequestType::Option::Snapshot),
    std::make_tuple(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES, MdSubscriptionRequestType::Option::Subscribe),
    std::make_tuple(FIX::SubscriptionRequestType_DISABLE_PREVIOUS_SNAPSHOT_PLUS_UPDATE_REQUEST, MdSubscriptionRequestType::Option::Unsubscribe)
  ));
// clang-format on

/*----------------------------------------------------------------------------*/

}  // namespace
}  // namespace simulator::fix::test