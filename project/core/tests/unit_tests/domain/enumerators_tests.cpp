#include <gtest/gtest.h>

#include <optional>
#include <sstream>
#include <utility>

#include "core/domain/attributes.hpp"
#include "fixtures/given_expected_fixture.hpp"

namespace simulator::core::test {
namespace {

using namespace testing;  // NOLINT

struct CoreSecurityTypeConversion
    : public GivenExpectedFixture<std::string,
                                  std::optional<SecurityType::Option>> {};

TEST_P(CoreSecurityTypeConversion, FromStringConversion) {
  using core::enumerators::convert_security_type_from_string;

  ASSERT_EQ(enumerators::convert_security_type_from_string(given()),
            expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Conversion,
    CoreSecurityTypeConversion,
    Values(std::make_pair("bad-value", std::nullopt),
           std::make_pair("CS", SecurityType::Option::CommonStock),
           std::make_pair("FUT", SecurityType::Option::Future),
           std::make_pair("OPT", SecurityType::Option::Option),
           std::make_pair("MLEG", SecurityType::Option::MultiLeg),
           std::make_pair("SML", SecurityType::Option::SyntheticMultiLeg),
           std::make_pair("WAR", SecurityType::Option::Warrant),
           std::make_pair("MF", SecurityType::Option::MutualFund),
           std::make_pair("CORP", SecurityType::Option::CorporateBond),
           std::make_pair("CB", SecurityType::Option::ConvertibleBond),
           std::make_pair("REPO", SecurityType::Option::RepurchaseAgreement),
           std::make_pair("INDEX", SecurityType::Option::Index),
           std::make_pair("CFD", SecurityType::Option::ContractForDifference),
           std::make_pair("CD", SecurityType::Option::Certificate),
           std::make_pair("FXSPOT", SecurityType::Option::FxSpot),
           std::make_pair("FORWARD", SecurityType::Option::Forward),
           std::make_pair("FXFWD", SecurityType::Option::FxForward),
           std::make_pair("FXNDF",SecurityType::Option::FxNonDeliverableForward),
           std::make_pair("FXSWAP", SecurityType::Option::FxSwap),
           std::make_pair("FXNDS", SecurityType::Option::FxNonDeliverableSwap)));
// clang-format on

struct CorePartyRoleConversion
    : public GivenExpectedFixture<std::string,
                                  std::optional<PartyRole::Option>> {};

TEST_P(CorePartyRoleConversion, FromStringConversion) {
  using core::enumerators::convert_party_role_from_string;

  ASSERT_EQ(enumerators::convert_party_role_from_string(given()), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Conversion,
    CorePartyRoleConversion,
    Values(std::make_pair("bad-value", std::nullopt),
           std::make_pair("ExecutingFirm", PartyRole::Option::ExecutingFirm),
           std::make_pair("BrokerOfCredit", PartyRole::Option::BrokerOfCredit),
           std::make_pair("ClientID", PartyRole::Option::ClientID),
           std::make_pair("ClearingFirm", PartyRole::Option::ClearingFirm),
           std::make_pair("ClearingFirm", PartyRole::Option::ClearingFirm),
           std::make_pair("InvestorID", PartyRole::Option::InvestorID),
           std::make_pair("IntroducingFirm", PartyRole::Option::IntroducingFirm),
           std::make_pair("EnteringFirm", PartyRole::Option::EnteringFirm),
           std::make_pair("Locate", PartyRole::Option::Locate),
           std::make_pair("FundManagerClientID", PartyRole::Option::FundManagerClientID),
           std::make_pair("SettlementLocation", PartyRole::Option::SettlementLocation),
           std::make_pair("OrderOriginationTrader", PartyRole::Option::OrderOriginationTrader),
           std::make_pair("ExecutingTrader", PartyRole::Option::ExecutingTrader),
           std::make_pair("OrderOriginationFirm", PartyRole::Option::OrderOriginationFirm),
           std::make_pair("CorrespondantClearingFirm", PartyRole::Option::CorrespondantClearingFirm),
           std::make_pair("ExecutingSystem", PartyRole::Option::ExecutingSystem),
           std::make_pair("ContraFirm", PartyRole::Option::ContraFirm),
           std::make_pair("ContraClearingFirm", PartyRole::Option::ContraClearingFirm),
           std::make_pair("SponsoringFirm", PartyRole::Option::SponsoringFirm),
           std::make_pair("UnderlyingContraFirm", PartyRole::Option::UnderlyingContraFirm),
           std::make_pair("ClearingOrganization", PartyRole::Option::ClearingOrganization),
           std::make_pair("Exchange", PartyRole::Option::Exchange),
           std::make_pair("CustomerAccount", PartyRole::Option::CustomerAccount),
           std::make_pair("CorrespondentClearingOrganization", PartyRole::Option::CorrespondentClearingOrganization),
           std::make_pair("CorrespondentBroker", PartyRole::Option::CorrespondentBroker),
           std::make_pair("Buyer", PartyRole::Option::Buyer),
           std::make_pair("Custodian", PartyRole::Option::Custodian),
           std::make_pair("Intermediary", PartyRole::Option::Intermediary),
           std::make_pair("Agent", PartyRole::Option::Agent),
           std::make_pair("SubCustodian", PartyRole::Option::SubCustodian),
           std::make_pair("Beneficiary", PartyRole::Option::Beneficiary),
           std::make_pair("InterestedParty", PartyRole::Option::InterestedParty),
           std::make_pair("RegulatoryBody", PartyRole::Option::RegulatoryBody),
           std::make_pair("LiquidityProvider", PartyRole::Option::LiquidityProvider),
           std::make_pair("EnteringTrader", PartyRole::Option::EnteringTrader),
           std::make_pair("ContraTrader", PartyRole::Option::ContraTrader),
           std::make_pair("PositionAccount", PartyRole::Option::PositionAccount),
           std::make_pair("ContraInvestorID", PartyRole::Option::ContraInvestorID),
           std::make_pair("TransferToFirm", PartyRole::Option::TransferToFirm),
           std::make_pair("ContraPositionAccount", PartyRole::Option::ContraPositionAccount),
           std::make_pair("ContraExchange", PartyRole::Option::ContraExchange),
           std::make_pair("InternalCarryAccount", PartyRole::Option::InternalCarryAccount),
           std::make_pair("OrderEntryOperatorID", PartyRole::Option::OrderEntryOperatorID),
           std::make_pair("SecondaryAccountNumber", PartyRole::Option::SecondaryAccountNumber),
           std::make_pair("ForeignFirm", PartyRole::Option::ForeignFirm),
           std::make_pair("ThirdPartyAllocationFirm", PartyRole::Option::ThirdPartyAllocationFirm),
           std::make_pair("ClaimingAccount", PartyRole::Option::ClaimingAccount),
           std::make_pair("AssetManager", PartyRole::Option::AssetManager),
           std::make_pair("PledgorAccount", PartyRole::Option::PledgorAccount),
           std::make_pair("PledgeeAccount", PartyRole::Option::PledgeeAccount),
           std::make_pair("LargeTraderReportableAccount", PartyRole::Option::LargeTraderReportableAccount),
           std::make_pair("TraderMnemonic", PartyRole::Option::TraderMnemonic),
           std::make_pair("SenderLocation", PartyRole::Option::SenderLocation),
           std::make_pair("SessionID", PartyRole::Option::SessionID),
           std::make_pair("AcceptableCounterparty", PartyRole::Option::AcceptableCounterparty),
           std::make_pair("UnacceptableCounterparty", PartyRole::Option::UnacceptableCounterparty),
           std::make_pair("EnteringUnit", PartyRole::Option::EnteringUnit),
           std::make_pair("ExecutingUnit", PartyRole::Option::ExecutingUnit),
           std::make_pair("IntroducingBroker", PartyRole::Option::IntroducingBroker),
           std::make_pair("QuoteOriginator", PartyRole::Option::QuoteOriginator),
           std::make_pair("ReportOriginator", PartyRole::Option::ReportOriginator),
           std::make_pair("SystematicInternaliser", PartyRole::Option::SystematicInternaliser),
           std::make_pair("MultilateralTradingFacility", PartyRole::Option::MultilateralTradingFacility),
           std::make_pair("RegulatedMarket", PartyRole::Option::RegulatedMarket),
           std::make_pair("MarketMaker", PartyRole::Option::MarketMaker),
           std::make_pair("InvestmentFirm", PartyRole::Option::InvestmentFirm),
           std::make_pair("HostCompetentAuthority", PartyRole::Option::HostCompetentAuthority),
           std::make_pair("HomeCompetentAuthority", PartyRole::Option::HomeCompetentAuthority),
           std::make_pair("CompetentAuthorityLiquidity", PartyRole::Option::CompetentAuthorityLiquidity),
           std::make_pair("CompetentAuthorityTransactionVenue", PartyRole::Option::CompetentAuthorityTransactionVenue),
           std::make_pair("ReportingIntermediary", PartyRole::Option::ReportingIntermediary),
           std::make_pair("ExecutionVenue", PartyRole::Option::ExecutionVenue),
           std::make_pair("MarketDataEntryOriginator", PartyRole::Option::MarketDataEntryOriginator),
           std::make_pair("LocationID", PartyRole::Option::LocationID),
           std::make_pair("DeskID", PartyRole::Option::DeskID),
           std::make_pair("MarketDataMarket", PartyRole::Option::MarketDataMarket),
           std::make_pair("AllocationEntity", PartyRole::Option::AllocationEntity),
           std::make_pair("PrimeBroker", PartyRole::Option::PrimeBroker),
           std::make_pair("StepOutFirm", PartyRole::Option::StepOutFirm),
           std::make_pair("BrokerClearingID", PartyRole::Option::BrokerClearingID),
           std::make_pair("CentralRegistrationDepository", PartyRole::Option::CentralRegistrationDepository),
           std::make_pair("ClearingAccount", PartyRole::Option::ClearingAccount),
           std::make_pair("AcceptableSettlingCounterparty", PartyRole::Option::AcceptableSettlingCounterparty),
           std::make_pair("UnacceptableSettlingCounterparty", PartyRole::Option::UnacceptableSettlingCounterparty),
           std::make_pair("CLSMemberBank", PartyRole::Option::CLSMemberBank),
           std::make_pair("InConcertGroup", PartyRole::Option::InConcertGroup),
           std::make_pair("InConcertControllingEntity", PartyRole::Option::InConcertControllingEntity),
           std::make_pair("LargePositionsReportingAccount", PartyRole::Option::LargePositionsReportingAccount),
           std::make_pair("SettlementFirm", PartyRole::Option::SettlementFirm),
           std::make_pair("SettlementAccount", PartyRole::Option::SettlementAccount),
           std::make_pair("ReportingMarketCenter", PartyRole::Option::ReportingMarketCenter),
           std::make_pair("RelatedReportingMarketCenter", PartyRole::Option::RelatedReportingMarketCenter),
           std::make_pair("AwayMarket", PartyRole::Option::AwayMarket),
           std::make_pair("GiveupTradingFirm", PartyRole::Option::GiveupTradingFirm),
           std::make_pair("TakeupTradingFirm", PartyRole::Option::TakeupTradingFirm),
           std::make_pair("GiveupClearingFirm", PartyRole::Option::GiveupClearingFirm),
           std::make_pair("TakeupClearingFirm", PartyRole::Option::TakeupClearingFirm),
           std::make_pair("OriginatingMarket", PartyRole::Option::OriginatingMarket),
           std::make_pair("MarginAccount", PartyRole::Option::MarginAccount),
           std::make_pair("CollateralAssetAccount", PartyRole::Option::CollateralAssetAccount),
           std::make_pair("DataRepository", PartyRole::Option::DataRepository),
           std::make_pair("CalculationAgent", PartyRole::Option::CalculationAgent),
           std::make_pair("ExerciseNoticeSender", PartyRole::Option::ExerciseNoticeSender),
           std::make_pair("ExerciseNoticeReceiver", PartyRole::Option::ExerciseNoticeReceiver),
           std::make_pair("RateReferenceBank", PartyRole::Option::RateReferenceBank),
           std::make_pair("Correspondent", PartyRole::Option::Correspondent),
           std::make_pair("Borrower", PartyRole::Option::Borrower),
           std::make_pair("PrimaryObligator", PartyRole::Option::PrimaryObligator),
           std::make_pair("Guarantor", PartyRole::Option::Guarantor),
           std::make_pair("ExcludedReferenceEntity", PartyRole::Option::ExcludedReferenceEntity),
           std::make_pair("DeterminingParty", PartyRole::Option::DeterminingParty),
           std::make_pair("HedgingParty", PartyRole::Option::HedgingParty),
           std::make_pair("ReportingEntity", PartyRole::Option::ReportingEntity),
           std::make_pair("SalesPerson", PartyRole::Option::SalesPerson),
           std::make_pair("Operator", PartyRole::Option::Operator),
           std::make_pair("CSD", PartyRole::Option::CSD),
           std::make_pair("ICSD", PartyRole::Option::ICSD),
           std::make_pair("TradingSubAccount", PartyRole::Option::TradingSubAccount),
           std::make_pair("InvestmentDecisionMaker", PartyRole::Option::InvestmentDecisionMaker),
           std::make_pair("PublishingIntermediary", PartyRole::Option::PublishingIntermediary),
           std::make_pair("CSDParticipant", PartyRole::Option::CSDParticipant),
           std::make_pair("Issuer", PartyRole::Option::Issuer),
           std::make_pair("ContraCustomerAccount", PartyRole::Option::ContraCustomerAccount),
           std::make_pair("ContraInvestmentDecisionMaker", PartyRole::Option::ContraInvestmentDecisionMaker)));
// clang-format on

struct CoreExecutionTypeFormatting
    : public GivenExpectedFixture<ExecutionType::Option, std::string> {};

TEST_P(CoreExecutionTypeFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreExecutionTypeFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreExecutionTypeFormatting,
    Values(std::make_pair(static_cast<ExecutionType::Option>(0xFF), "undefined"),
           std::make_pair(ExecutionType::Option::OrderPlaced, "OrderPlaced"),
           std::make_pair(ExecutionType::Option::OrderModified, "OrderModified"),
           std::make_pair(ExecutionType::Option::OrderCancelled, "OrderCancelled"),
           std::make_pair(ExecutionType::Option::Rejected, "Rejected"),
           std::make_pair(ExecutionType::Option::OrderTraded, "OrderTraded")));
// clang-format on

struct CoreOrderStatusFormatting
    : public GivenExpectedFixture<OrderStatus::Option, std::string> {};

TEST_P(CoreOrderStatusFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreOrderStatusFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreOrderStatusFormatting,
    Values(std::make_pair(static_cast<OrderStatus::Option>(0xFF), "undefined"),
           std::make_pair(OrderStatus::Option::New, "New"),
           std::make_pair(OrderStatus::Option::PartiallyFilled, "PartiallyFilled"),
           std::make_pair(OrderStatus::Option::Filled, "Filled"),
           std::make_pair(OrderStatus::Option::Modified, "Modified"),
           std::make_pair(OrderStatus::Option::Cancelled, "Cancelled"),
           std::make_pair(OrderStatus::Option::Rejected, "Rejected")));
// clang-format on

struct CoreOrderTypeFormatting
    : public GivenExpectedFixture<OrderType::Option, std::string> {};

TEST_P(CoreOrderTypeFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreOrderTypeFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreOrderTypeFormatting,
    Values(std::make_pair(static_cast<OrderType::Option>(0xFF), "undefined"),
           std::make_pair(OrderType::Option::Limit, "Limit"),
           std::make_pair(OrderType::Option::Market, "Market")));
// clang-format on

struct CorePartyIdSourceFormatting
    : public GivenExpectedFixture<PartyIdSource::Option, std::string> {};

TEST_P(CorePartyIdSourceFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CorePartyIdSourceFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CorePartyIdSourceFormatting,
    Values(std::make_pair(static_cast<PartyIdSource::Option>(0xFF), "undefined"),
           std::make_pair(PartyIdSource::Option::UKNationalInsuranceOrPensionNumber, "UKNationalInsuranceOrPensionNumber"),
           std::make_pair(PartyIdSource::Option::USSocialSecurityNumber, "USSocialSecurityNumber"),
           std::make_pair(PartyIdSource::Option::USEmployerOrTaxIDNumber, "USEmployerOrTaxIDNumber"),
           std::make_pair(PartyIdSource::Option::AustralianBusinessNumber, "AustralianBusinessNumber"),
           std::make_pair(PartyIdSource::Option::AustralianTaxFileNumber, "AustralianTaxFileNumber"),
           std::make_pair(PartyIdSource::Option::TaxID, "TaxID"),
           std::make_pair(PartyIdSource::Option::KoreanInvestorID, "KoreanInvestorID"),
           std::make_pair(PartyIdSource::Option::TaiwaneseForeignInvestorID, "TaiwaneseForeignInvestorID"),
           std::make_pair(PartyIdSource::Option::TaiwaneseTradingAcct, "TaiwaneseTradingAcct"),
           std::make_pair(PartyIdSource::Option::MalaysianCentralDepository, "MalaysianCentralDepository"),
           std::make_pair(PartyIdSource::Option::ChineseInvestorID, "ChineseInvestorID"),
           std::make_pair(PartyIdSource::Option::ISITCAcronym, "ISITCAcronym"),
           std::make_pair(PartyIdSource::Option::BIC, "BIC"),
           std::make_pair(PartyIdSource::Option::GeneralIdentifier, "GeneralIdentifier"),
           std::make_pair(PartyIdSource::Option::Proprietary, "Proprietary"),
           std::make_pair(PartyIdSource::Option::ISOCountryCode, "ISOCountryCode"),
           std::make_pair(PartyIdSource::Option::SettlementEntityLocation, "SettlementEntityLocation"),
           std::make_pair(PartyIdSource::Option::MIC, "MIC"),
           std::make_pair(PartyIdSource::Option::CSDParticipant, "CSDParticipant"),
           std::make_pair(PartyIdSource::Option::AustralianCompanyNumber, "AustralianCompanyNumber"),
           std::make_pair(PartyIdSource::Option::AustralianRegisteredBodyNumber, "AustralianRegisteredBodyNumber"),
           std::make_pair(PartyIdSource::Option::CFTCReportingFirmIdentifier, "CFTCReportingFirmIdentifier"),
           std::make_pair(PartyIdSource::Option::LegalEntityIdentifier, "LegalEntityIdentifier"),
           std::make_pair(PartyIdSource::Option::InterimIdentifier, "InterimIdentifier"),
           std::make_pair(PartyIdSource::Option::ShortCodeIdentifier, "ShortCodeIdentifier"),
           std::make_pair(PartyIdSource::Option::NationalIDNaturalPerson, "NationalIDNaturalPerson"),
           std::make_pair(PartyIdSource::Option::IndiaPermanentAccountNumber, "IndiaPermanentAccountNumber"),
           std::make_pair(PartyIdSource::Option::FDID, "FDID"),
           std::make_pair(PartyIdSource::Option::SPSAID, "SPSAID"),
           std::make_pair(PartyIdSource::Option::MasterSPSAID, "MasterSPSAID")));
// clang-format on

struct CorePartyRoleFormatting
    : public GivenExpectedFixture<PartyRole::Option, std::string> {};

TEST_P(CorePartyRoleFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CorePartyRoleFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CorePartyRoleFormatting,
    Values(std::make_pair(static_cast<PartyRole::Option>(0xFF), "undefined"),
           std::make_pair(PartyRole::Option::ExecutingFirm, "ExecutingFirm"),
           std::make_pair(PartyRole::Option::BrokerOfCredit, "BrokerOfCredit"),
           std::make_pair(PartyRole::Option::ClientID, "ClientID"),
           std::make_pair(PartyRole::Option::ClearingFirm, "ClearingFirm"),
           std::make_pair(PartyRole::Option::ClearingFirm, "ClearingFirm"),
           std::make_pair(PartyRole::Option::InvestorID, "InvestorID"),
           std::make_pair(PartyRole::Option::IntroducingFirm, "IntroducingFirm"),
           std::make_pair(PartyRole::Option::EnteringFirm, "EnteringFirm"),
           std::make_pair(PartyRole::Option::Locate, "Locate"),
           std::make_pair(PartyRole::Option::FundManagerClientID, "FundManagerClientID"),
           std::make_pair(PartyRole::Option::SettlementLocation, "SettlementLocation"),
           std::make_pair(PartyRole::Option::OrderOriginationTrader, "OrderOriginationTrader"),
           std::make_pair(PartyRole::Option::ExecutingTrader, "ExecutingTrader"),
           std::make_pair(PartyRole::Option::OrderOriginationFirm, "OrderOriginationFirm"),
           std::make_pair(PartyRole::Option::CorrespondantClearingFirm, "CorrespondantClearingFirm"),
           std::make_pair(PartyRole::Option::ExecutingSystem, "ExecutingSystem"),
           std::make_pair(PartyRole::Option::ContraFirm, "ContraFirm"),
           std::make_pair(PartyRole::Option::ContraClearingFirm, "ContraClearingFirm"),
           std::make_pair(PartyRole::Option::SponsoringFirm, "SponsoringFirm"),
           std::make_pair(PartyRole::Option::UnderlyingContraFirm, "UnderlyingContraFirm"),
           std::make_pair(PartyRole::Option::ClearingOrganization, "ClearingOrganization"),
           std::make_pair(PartyRole::Option::Exchange, "Exchange"),
           std::make_pair(PartyRole::Option::CustomerAccount, "CustomerAccount"),
           std::make_pair(PartyRole::Option::CorrespondentClearingOrganization, "CorrespondentClearingOrganization"),
           std::make_pair(PartyRole::Option::CorrespondentBroker, "CorrespondentBroker"),
           std::make_pair(PartyRole::Option::Buyer, "Buyer"),
           std::make_pair(PartyRole::Option::Custodian, "Custodian"),
           std::make_pair(PartyRole::Option::Intermediary, "Intermediary"),
           std::make_pair(PartyRole::Option::Agent, "Agent"),
           std::make_pair(PartyRole::Option::SubCustodian, "SubCustodian"),
           std::make_pair(PartyRole::Option::Beneficiary, "Beneficiary"),
           std::make_pair(PartyRole::Option::InterestedParty, "InterestedParty"),
           std::make_pair(PartyRole::Option::RegulatoryBody, "RegulatoryBody"),
           std::make_pair(PartyRole::Option::LiquidityProvider, "LiquidityProvider"),
           std::make_pair(PartyRole::Option::EnteringTrader, "EnteringTrader"),
           std::make_pair(PartyRole::Option::ContraTrader, "ContraTrader"),
           std::make_pair(PartyRole::Option::PositionAccount, "PositionAccount"),
           std::make_pair(PartyRole::Option::ContraInvestorID, "ContraInvestorID"),
           std::make_pair(PartyRole::Option::TransferToFirm, "TransferToFirm"),
           std::make_pair(PartyRole::Option::ContraPositionAccount, "ContraPositionAccount"),
           std::make_pair(PartyRole::Option::ContraExchange, "ContraExchange"),
           std::make_pair(PartyRole::Option::InternalCarryAccount, "InternalCarryAccount"),
           std::make_pair(PartyRole::Option::OrderEntryOperatorID, "OrderEntryOperatorID"),
           std::make_pair(PartyRole::Option::SecondaryAccountNumber, "SecondaryAccountNumber"),
           std::make_pair(PartyRole::Option::ForeignFirm, "ForeignFirm"),
           std::make_pair(PartyRole::Option::ThirdPartyAllocationFirm, "ThirdPartyAllocationFirm"),
           std::make_pair(PartyRole::Option::ClaimingAccount, "ClaimingAccount"),
           std::make_pair(PartyRole::Option::AssetManager, "AssetManager"),
           std::make_pair(PartyRole::Option::PledgorAccount, "PledgorAccount"),
           std::make_pair(PartyRole::Option::PledgeeAccount, "PledgeeAccount"),
           std::make_pair(PartyRole::Option::LargeTraderReportableAccount, "LargeTraderReportableAccount"),
           std::make_pair(PartyRole::Option::TraderMnemonic, "TraderMnemonic"),
           std::make_pair(PartyRole::Option::SenderLocation, "SenderLocation"),
           std::make_pair(PartyRole::Option::SessionID, "SessionID"),
           std::make_pair(PartyRole::Option::AcceptableCounterparty, "AcceptableCounterparty"),
           std::make_pair(PartyRole::Option::UnacceptableCounterparty, "UnacceptableCounterparty"),
           std::make_pair(PartyRole::Option::EnteringUnit, "EnteringUnit"),
           std::make_pair(PartyRole::Option::ExecutingUnit, "ExecutingUnit"),
           std::make_pair(PartyRole::Option::IntroducingBroker, "IntroducingBroker"),
           std::make_pair(PartyRole::Option::QuoteOriginator, "QuoteOriginator"),
           std::make_pair(PartyRole::Option::ReportOriginator, "ReportOriginator"),
           std::make_pair(PartyRole::Option::SystematicInternaliser, "SystematicInternaliser"),
           std::make_pair(PartyRole::Option::MultilateralTradingFacility, "MultilateralTradingFacility"),
           std::make_pair(PartyRole::Option::RegulatedMarket, "RegulatedMarket"),
           std::make_pair(PartyRole::Option::MarketMaker, "MarketMaker"),
           std::make_pair(PartyRole::Option::InvestmentFirm, "InvestmentFirm"),
           std::make_pair(PartyRole::Option::HostCompetentAuthority, "HostCompetentAuthority"),
           std::make_pair(PartyRole::Option::HomeCompetentAuthority, "HomeCompetentAuthority"),
           std::make_pair(PartyRole::Option::CompetentAuthorityLiquidity, "CompetentAuthorityLiquidity"),
           std::make_pair(PartyRole::Option::CompetentAuthorityTransactionVenue, "CompetentAuthorityTransactionVenue"),
           std::make_pair(PartyRole::Option::ReportingIntermediary, "ReportingIntermediary"),
           std::make_pair(PartyRole::Option::ExecutionVenue, "ExecutionVenue"),
           std::make_pair(PartyRole::Option::MarketDataEntryOriginator, "MarketDataEntryOriginator"),
           std::make_pair(PartyRole::Option::LocationID, "LocationID"),
           std::make_pair(PartyRole::Option::DeskID, "DeskID"),
           std::make_pair(PartyRole::Option::MarketDataMarket, "MarketDataMarket"),
           std::make_pair(PartyRole::Option::AllocationEntity, "AllocationEntity"),
           std::make_pair(PartyRole::Option::PrimeBroker, "PrimeBroker"),
           std::make_pair(PartyRole::Option::StepOutFirm, "StepOutFirm"),
           std::make_pair(PartyRole::Option::BrokerClearingID, "BrokerClearingID"),
           std::make_pair(PartyRole::Option::CentralRegistrationDepository, "CentralRegistrationDepository"),
           std::make_pair(PartyRole::Option::ClearingAccount, "ClearingAccount"),
           std::make_pair(PartyRole::Option::AcceptableSettlingCounterparty, "AcceptableSettlingCounterparty"),
           std::make_pair(PartyRole::Option::UnacceptableSettlingCounterparty, "UnacceptableSettlingCounterparty"),
           std::make_pair(PartyRole::Option::CLSMemberBank, "CLSMemberBank"),
           std::make_pair(PartyRole::Option::InConcertGroup, "InConcertGroup"),
           std::make_pair(PartyRole::Option::InConcertControllingEntity, "InConcertControllingEntity"),
           std::make_pair(PartyRole::Option::LargePositionsReportingAccount, "LargePositionsReportingAccount"),
           std::make_pair(PartyRole::Option::SettlementFirm, "SettlementFirm"),
           std::make_pair(PartyRole::Option::SettlementAccount, "SettlementAccount"),
           std::make_pair(PartyRole::Option::ReportingMarketCenter, "ReportingMarketCenter"),
           std::make_pair(PartyRole::Option::RelatedReportingMarketCenter, "RelatedReportingMarketCenter"),
           std::make_pair(PartyRole::Option::AwayMarket, "AwayMarket"),
           std::make_pair(PartyRole::Option::GiveupTradingFirm, "GiveupTradingFirm"),
           std::make_pair(PartyRole::Option::TakeupTradingFirm, "TakeupTradingFirm"),
           std::make_pair(PartyRole::Option::GiveupClearingFirm, "GiveupClearingFirm"),
           std::make_pair(PartyRole::Option::TakeupClearingFirm, "TakeupClearingFirm"),
           std::make_pair(PartyRole::Option::OriginatingMarket, "OriginatingMarket"),
           std::make_pair(PartyRole::Option::MarginAccount, "MarginAccount"),
           std::make_pair(PartyRole::Option::CollateralAssetAccount, "CollateralAssetAccount"),
           std::make_pair(PartyRole::Option::DataRepository, "DataRepository"),
           std::make_pair(PartyRole::Option::CalculationAgent, "CalculationAgent"),
           std::make_pair(PartyRole::Option::ExerciseNoticeSender, "ExerciseNoticeSender"),
           std::make_pair(PartyRole::Option::ExerciseNoticeReceiver, "ExerciseNoticeReceiver"),
           std::make_pair(PartyRole::Option::RateReferenceBank, "RateReferenceBank"),
           std::make_pair(PartyRole::Option::Correspondent, "Correspondent"),
           std::make_pair(PartyRole::Option::Borrower, "Borrower"),
           std::make_pair(PartyRole::Option::PrimaryObligator, "PrimaryObligator"),
           std::make_pair(PartyRole::Option::Guarantor, "Guarantor"),
           std::make_pair(PartyRole::Option::ExcludedReferenceEntity, "ExcludedReferenceEntity"),
           std::make_pair(PartyRole::Option::DeterminingParty, "DeterminingParty"),
           std::make_pair(PartyRole::Option::HedgingParty, "HedgingParty"),
           std::make_pair(PartyRole::Option::ReportingEntity, "ReportingEntity"),
           std::make_pair(PartyRole::Option::SalesPerson, "SalesPerson"),
           std::make_pair(PartyRole::Option::Operator, "Operator"),
           std::make_pair(PartyRole::Option::CSD, "CSD"),
           std::make_pair(PartyRole::Option::ICSD, "ICSD"),
           std::make_pair(PartyRole::Option::TradingSubAccount, "TradingSubAccount"),
           std::make_pair(PartyRole::Option::InvestmentDecisionMaker, "InvestmentDecisionMaker"),
           std::make_pair(PartyRole::Option::PublishingIntermediary, "PublishingIntermediary"),
           std::make_pair(PartyRole::Option::CSDParticipant, "CSDParticipant"),
           std::make_pair(PartyRole::Option::Issuer, "Issuer"),
           std::make_pair(PartyRole::Option::ContraCustomerAccount, "ContraCustomerAccount"),
           std::make_pair(PartyRole::Option::ContraInvestmentDecisionMaker, "ContraInvestmentDecisionMaker")));
// clang-format on

struct CoreSecurityIdSourceFormatting
    : public GivenExpectedFixture<SecurityIdSource::Option, std::string> {};

TEST_P(CoreSecurityIdSourceFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreSecurityIdSourceFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreSecurityIdSourceFormatting,
    Values(std::make_pair(static_cast<SecurityIdSource::Option>(0xFF), "undefined"),
           std::make_pair(SecurityIdSource::Option::Cusip, "CUSIP"),
           std::make_pair(SecurityIdSource::Option::Sedol, "SEDOL"),
           std::make_pair(SecurityIdSource::Option::Isin, "ISIN"),
           std::make_pair(SecurityIdSource::Option::Ric, "RIC"),
           std::make_pair(SecurityIdSource::Option::ExchangeSymbol, "ExchangeSymbol"),
           std::make_pair(SecurityIdSource::Option::BloombergSymbol, "BloombergSymbol")));
// clang-format on

struct CoreSecurityTypeFormatting
    : public GivenExpectedFixture<SecurityType::Option, std::string> {};

TEST_P(CoreSecurityTypeFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreSecurityTypeFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreSecurityTypeFormatting,
    Values(std::make_pair(static_cast<SecurityType::Option>(0xFF), "undefined"),
           std::make_pair(SecurityType::Option::CommonStock, "CommonStock"),
           std::make_pair(SecurityType::Option::Future, "Future"),
           std::make_pair(SecurityType::Option::Option, "Option"),
           std::make_pair(SecurityType::Option::MultiLeg, "MultiLeg"),
           std::make_pair(SecurityType::Option::SyntheticMultiLeg, "SyntheticMultiLeg"),
           std::make_pair(SecurityType::Option::Warrant, "Warrant"),
           std::make_pair(SecurityType::Option::MutualFund, "MutualFund"),
           std::make_pair(SecurityType::Option::CorporateBond, "CorporateBond"),
           std::make_pair(SecurityType::Option::ConvertibleBond, "ConvertibleBond"),
           std::make_pair(SecurityType::Option::RepurchaseAgreement, "RepurchaseAgreement"),
           std::make_pair(SecurityType::Option::Index, "Index"),
           std::make_pair(SecurityType::Option::ContractForDifference, "ContractForDifference"),
           std::make_pair(SecurityType::Option::Certificate, "Certificate"),
           std::make_pair(SecurityType::Option::FxSpot, "FxSpot"),
           std::make_pair(SecurityType::Option::Forward, "Forward"),
           std::make_pair(SecurityType::Option::FxForward, "FxForward"),
           std::make_pair(SecurityType::Option::FxNonDeliverableForward, "FxNonDeliverableForward"),
           std::make_pair(SecurityType::Option::FxSwap, "FxSwap"),
           std::make_pair(SecurityType::Option::FxNonDeliverableSwap, "FxNonDeliverableSwap")));
// clang-format on

struct CoreSideFormatting
    : public GivenExpectedFixture<Side::Option, std::string> {};

TEST_P(CoreSideFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreSideFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreSideFormatting,
    Values(std::make_pair(static_cast<Side::Option>(0xFF), "undefined"),
           std::make_pair(Side::Option::Buy, "Buy"),
           std::make_pair(Side::Option::Sell, "Sell"),
           std::make_pair(Side::Option::SellShort, "SellShort"),
           std::make_pair(Side::Option::SellShortExempt, "SellShortExempt")));
// clang-format on

struct CoreTimeInForceFormatting
    : public GivenExpectedFixture<TimeInForce::Option, std::string> {};

TEST_P(CoreTimeInForceFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreTimeInForceFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreTimeInForceFormatting,
    Values(std::make_pair(static_cast<TimeInForce::Option>(0xFF), "undefined"),
           std::make_pair(TimeInForce::Option::Day, "Day"),
           std::make_pair(TimeInForce::Option::ImmediateOrCancel, "ImmediateOrCancel"),
           std::make_pair(TimeInForce::Option::FillOrKill, "FillOrKill"),
           std::make_pair(TimeInForce::Option::GoodTillDate, "GoodTillDate"),
           std::make_pair(TimeInForce::Option::GoodTillCancel, "GoodTillCancel")));
// clang-format on

struct CoreMdEntryTypeFormatting
    : public GivenExpectedFixture<MdEntryType::Option, std::string> {};

TEST_P(CoreMdEntryTypeFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreMdEntryTypeFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreMdEntryTypeFormatting,
    Values(std::make_pair(static_cast<MdEntryType::Option>(0xFF), "undefined"),
           std::make_pair(MdEntryType::Option::Bid, "Bid"),
           std::make_pair(MdEntryType::Option::Offer, "Offer"),
           std::make_pair(MdEntryType::Option::Trade, "Trade"),
           std::make_pair(MdEntryType::Option::LowPrice, "LowPrice"),
           std::make_pair(MdEntryType::Option::MidPrice, "MidPrice"),
           std::make_pair(MdEntryType::Option::HighPrice, "HighPrice")));
// clang-format on

struct CoreMdSubscriptionRequestTypeFormatting
    : public GivenExpectedFixture<MdSubscriptionRequestType::Option,
                                  std::string> {};

TEST_P(CoreMdSubscriptionRequestTypeFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreMdSubscriptionRequestTypeFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreMdSubscriptionRequestTypeFormatting,
    Values(std::make_pair(static_cast<MdSubscriptionRequestType::Option>(0xFF), "undefined"),
           std::make_pair(MdSubscriptionRequestType::Option::Subscribe, "Subscribe"),
           std::make_pair(MdSubscriptionRequestType::Option::Unsubscribe, "Unsubscribe"),
           std::make_pair(MdSubscriptionRequestType::Option::Snapshot, "Snapshot")));
// clang-format on

struct CoreMarketEntryActionFormatting
    : public GivenExpectedFixture<MarketEntryAction::Option, std::string> {};

TEST_P(CoreMarketEntryActionFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreMarketEntryActionFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreMarketEntryActionFormatting,
    Values(std::make_pair(static_cast<MarketEntryAction::Option>(0xFF), "undefined"),
           std::make_pair(MarketEntryAction::Option::New, "New"),
           std::make_pair(MarketEntryAction::Option::Change, "Change"),
           std::make_pair(MarketEntryAction::Option::Delete, "Delete")));
// clang-format on

struct CoreMarketDataUpdateTypeFormatting
    : public GivenExpectedFixture<MarketDataUpdateType::Option, std::string> {};

TEST_P(CoreMarketDataUpdateTypeFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreMarketDataUpdateTypeFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreMarketDataUpdateTypeFormatting,
    Values(std::make_pair(static_cast<MarketDataUpdateType::Option>(0xFF), "undefined"),
           std::make_pair(MarketDataUpdateType::Option::Snapshot, "Snapshot"),
           std::make_pair(MarketDataUpdateType::Option::Incremental, "Incremental")));
// clang-format on

struct CoreMdRejectReasonFormatting
    : public GivenExpectedFixture<MdRejectReason::Option, std::string> {};

TEST_P(CoreMdRejectReasonFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreMdRejectReasonFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreMdRejectReasonFormatting,
    Values(std::make_pair(static_cast<MdRejectReason::Option>(0xFF), "undefined"),
           std::make_pair(MdRejectReason::Option::UnknownSymbol, "UnknownSymbol"),
           std::make_pair(MdRejectReason::Option::DuplicateMdReqId, "DuplicateMdReqId")));
// clang-format on

struct CoreTradingPhaseFormatting
    : public GivenExpectedFixture<TradingPhase::Option, std::string> {};

TEST_P(CoreTradingPhaseFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreTradingPhaseFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreTradingPhaseFormatting,
    Values(std::make_pair(static_cast<TradingPhase::Option>(0xFF), "undefined"),
           std::make_pair(TradingPhase::Option::Open, "Open"),
           std::make_pair(TradingPhase::Option::Closed, "Closed"),
           std::make_pair(TradingPhase::Option::PostTrading, "PostTrading"),
           std::make_pair(TradingPhase::Option::OpeningAuction, "OpeningAuction"),
           std::make_pair(TradingPhase::Option::IntradayAuction, "IntradayAuction"),
           std::make_pair(TradingPhase::Option::ClosingAuction, "ClosingAuction")));
// clang-format on

struct CoreTradingStatusFormatting
    : public GivenExpectedFixture<TradingStatus::Option, std::string> {};

TEST_P(CoreTradingStatusFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreTradingStatusFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreTradingStatusFormatting,
    Values(std::make_pair(static_cast<TradingStatus::Option>(0xFF), "undefined"),
           std::make_pair(TradingStatus::Option::Halt, "Halt"),
           std::make_pair(TradingStatus::Option::Resume, "Resume")));
// clang-format on

struct RejectedMessageTypeFormatting
    : public GivenExpectedFixture<RejectedMessageType, std::string> {};

TEST_P(RejectedMessageTypeFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(RejectedMessageTypeFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    RejectedMessageTypeFormatting,
    Values(std::make_pair(static_cast<RejectedMessageType::Option>(0xFF), "undefined"),
           std::make_pair(RejectedMessageType::Option::SecurityStatusRequest, "SecurityStatusRequest")));
// clang-format on

struct BusinessRejectReasonFormatting
    : public GivenExpectedFixture<BusinessRejectReason, std::string> {};

TEST_P(BusinessRejectReasonFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(BusinessRejectReasonFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    BusinessRejectReasonFormatting,
    Values(std::make_pair(static_cast<BusinessRejectReason::Option>(0xFF), "undefined"),
           std::make_pair(BusinessRejectReason::Option::Other, "Other"),
           std::make_pair(BusinessRejectReason::Option::UnknownId, "UnknownId"),
           std::make_pair(BusinessRejectReason::Option::UnknownSecurity, "UnknownSecurity")));
// clang-format on

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::core::test
