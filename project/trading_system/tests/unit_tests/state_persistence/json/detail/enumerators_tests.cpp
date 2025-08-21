#include <gmock/gmock.h>

#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"
#include "test_utils/utils.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace testing;                          // NOLINT
using namespace simulator::trading_system::test;  // NOLINT

TEST(TradingSystemJsonEnumerators, OutputsErrorOnReadIncorrectPartyRole) {
  const rapidjson::Value value{"incorrect_value"};
  PartyRole role{PartyRole::Option::ExecutingFirm};

  ASSERT_THAT(read(value, role),
              ReturnsError("Unknown PartyRole string `incorrect_value'"));
}

TEST(TradingSystemJsonEnumerators, OutputsErrorOnWriteIncorrectPartyRole) {
  rapidjson::Document doc;
  rapidjson::Value value;
  constexpr PartyRole role{invalid_enum_value<PartyRole::Option>()};

  ASSERT_THAT(write(value, doc.GetAllocator(), role),
              ReturnsError("Unknown PartyRole value `255'"));
}

struct TradingSystemJsonPartyRoleConversion
    : public TestWithParam<std::pair<PartyRole, std::string>> {};

TEST_P(TradingSystemJsonPartyRoleConversion, ReadsPartyRole) {
  rapidjson::Value value;
  value.SetString(GetParam().second.c_str(),
                  static_cast<rapidjson::SizeType>(GetParam().second.size()));

  PartyRole role{PartyRole::Option::ExecutingFirm};

  ASSERT_TRUE(read(value, role).has_value());
  ASSERT_EQ(role, GetParam().first);
}

TEST_P(TradingSystemJsonPartyRoleConversion, WritesPartyRole) {
  rapidjson::Document doc;
  rapidjson::Value value;
  const PartyRole role{GetParam().first};

  ASSERT_TRUE(write(value, doc.GetAllocator(), role).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), GetParam().second.c_str());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Conversion,
    TradingSystemJsonPartyRoleConversion,
    Values(std::make_pair(PartyRole::Option::ExecutingFirm, "ExecutingFirm"),
           std::make_pair(PartyRole::Option::BrokerOfCredit, "BrokerOfCredit"),
           std::make_pair(PartyRole::Option::ClientID, "ClientID"),
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

TEST(TradingSystemJsonEnumerators, OutputsErrorOnReadIncorrectPartyIdSource) {
  const rapidjson::Value value{"incorrect_value"};
  PartyIdSource source{
      PartyIdSource::Option::UKNationalInsuranceOrPensionNumber};

  ASSERT_THAT(read(value, source),
              ReturnsError("Unknown PartyIdSource string `incorrect_value'"));
}

TEST(TradingSystemJsonEnumerators, OutputsErrorOnWriteIncorrectPartyIdSource) {
  rapidjson::Document doc;
  rapidjson::Value value;
  constexpr PartyIdSource source{invalid_enum_value<PartyIdSource::Option>()};

  ASSERT_THAT(write(value, doc.GetAllocator(), source),
              ReturnsError("Unknown PartyIdSource value `255'"));
}

struct TradingSystemMarketStatePartyIdSourceConversion
    : public TestWithParam<std::pair<PartyIdSource, std::string>> {};

TEST_P(TradingSystemMarketStatePartyIdSourceConversion, ReadsPartyIdSource) {
  rapidjson::Value value;
  value.SetString(GetParam().second.c_str(),
                  static_cast<rapidjson::SizeType>(GetParam().second.size()));

  PartyIdSource source{
      PartyIdSource::Option::UKNationalInsuranceOrPensionNumber};

  ASSERT_TRUE(read(value, source).has_value());
  ASSERT_EQ(source, GetParam().first);
}

TEST_P(TradingSystemMarketStatePartyIdSourceConversion, WritesPartyIdSource) {
  rapidjson::Document doc;
  rapidjson::Value value;
  const PartyIdSource role{GetParam().first};

  ASSERT_TRUE(write(value, doc.GetAllocator(), role).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), GetParam().second.c_str());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Conversion,
    TradingSystemMarketStatePartyIdSourceConversion,
    Values(std::make_pair(PartyIdSource::Option::UKNationalInsuranceOrPensionNumber, "UKNationalInsuranceOrPensionNumber"),
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

TEST(TradingSystemJsonEnumerators, OutputsErrorOnReadIncorrectSecurityType) {
  const rapidjson::Value value{"incorrect_value"};
  SecurityType type{SecurityType::Option::CommonStock};

  ASSERT_THAT(read(value, type),
              ReturnsError("Unknown SecurityType string `incorrect_value'"));
}

TEST(TradingSystemJsonEnumerators, OutputsErrorOnWriteIncorrectSecurityType) {
  rapidjson::Document doc;
  rapidjson::Value value;
  constexpr SecurityType type{invalid_enum_value<SecurityType::Option>()};

  ASSERT_THAT(write(value, doc.GetAllocator(), type),
              ReturnsError("Unknown SecurityType value `255'"));
}

struct TradingSystemMarketStateSecurityTypeConversion
    : public TestWithParam<std::pair<SecurityType, std::string>> {};

TEST_P(TradingSystemMarketStateSecurityTypeConversion, ReadsSecurityType) {
  rapidjson::Value value;
  value.SetString(GetParam().second.c_str(),
                  static_cast<rapidjson::SizeType>(GetParam().second.size()));
  SecurityType type{SecurityType::Option::CorporateBond};
  ASSERT_TRUE(read(value, type).has_value());
  ASSERT_EQ(type, GetParam().first);
}

TEST_P(TradingSystemMarketStateSecurityTypeConversion, WritesSecurityType) {
  rapidjson::Document doc;
  rapidjson::Value value;
  const SecurityType type{GetParam().first};

  ASSERT_TRUE(write(value, doc.GetAllocator(), type).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), GetParam().second.c_str());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Conversion,
    TradingSystemMarketStateSecurityTypeConversion,
    Values(std::make_pair(SecurityType::Option::CommonStock, "CommonStock"),
           std::make_pair(SecurityType::Option::Future, "Future"),
           std::make_pair(SecurityType::Option::Option, "Option"),
           std::make_pair(SecurityType::Option::MultiLeg, "MultilegInstrument"),
           std::make_pair(SecurityType::Option::SyntheticMultiLeg, "SyntheticMultiLeg"),
           std::make_pair(SecurityType::Option::Warrant, "Warrant"),
           std::make_pair(SecurityType::Option::MutualFund, "MutualFund"),
           std::make_pair(SecurityType::Option::CorporateBond, "CorporateBond"),
           std::make_pair(SecurityType::Option::ConvertibleBond, "ConvertibleBond"),
           std::make_pair(SecurityType::Option::RepurchaseAgreement, "RepurchaseAgreement"),
           std::make_pair(SecurityType::Option::Index, "Index"),
           std::make_pair(SecurityType::Option::ContractForDifference, "ContractForDifference"),
           std::make_pair(SecurityType::Option::Certificate, "Certificate"),
           std::make_pair(SecurityType::Option::FxSpot, "FXSpot"),
           std::make_pair(SecurityType::Option::Forward, "Forward"),
           std::make_pair(SecurityType::Option::FxForward, "FXForward"),
           std::make_pair(SecurityType::Option::FxNonDeliverableForward, "NonDeliverableForward"),
           std::make_pair(SecurityType::Option::FxSwap, "FXSwap"),
           std::make_pair(SecurityType::Option::FxNonDeliverableSwap, "NonDeliverableSwap")));
// clang-format on

TEST(TradingSystemJsonEnumerators, OutputsErrorOnReadIncorrectAggressorSide) {
  const rapidjson::Value value{"incorrect_value"};
  AggressorSide side{AggressorSide::Option::Buy};

  ASSERT_THAT(read(value, side),
              ReturnsError("Unknown AggressorSide string `incorrect_value'"));
}

TEST(TradingSystemJsonEnumerators, OutputsErrorOnWriteIncorrectAggressorSide) {
  rapidjson::Document doc;
  rapidjson::Value value;
  constexpr AggressorSide side{invalid_enum_value<AggressorSide::Option>()};

  ASSERT_THAT(write(value, doc.GetAllocator(), side),
              ReturnsError("Unknown AggressorSide value `255'"));
}

struct TradingSystemMarketStateAggressorSideConversion
    : public TestWithParam<std::pair<AggressorSide, std::string>> {};

TEST_P(TradingSystemMarketStateAggressorSideConversion, ReadsAggressorSide) {
  rapidjson::Value value;
  value.SetString(GetParam().second.c_str(),
                  static_cast<rapidjson::SizeType>(GetParam().second.size()));
  AggressorSide side{AggressorSide::Option::Buy};
  ASSERT_TRUE(read(value, side).has_value());
  ASSERT_EQ(side, GetParam().first);
}

TEST_P(TradingSystemMarketStateAggressorSideConversion, WritesAggressorSide) {
  rapidjson::Document doc;
  rapidjson::Value value;
  const AggressorSide side{GetParam().first};

  ASSERT_TRUE(write(value, doc.GetAllocator(), side).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), GetParam().second.c_str());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Conversion,
    TradingSystemMarketStateAggressorSideConversion,
    Values(std::make_pair(AggressorSide::Option::Buy, "Buy"),
           std::make_pair(AggressorSide::Option::Sell, "Sell"),
           std::make_pair(AggressorSide::Option::SellShort, "SellShort"),
           std::make_pair(AggressorSide::Option::SellShortExempt, "SellShortExempt")));
// clang-format on

TEST(TradingSystemJsonEnumerators, OutputsErrorOnReadIncorrectTradingPhase) {
  const rapidjson::Value value{"incorrect_value"};
  TradingPhase phase{TradingPhase::Option::Open};

  ASSERT_THAT(read(value, phase),
              ReturnsError("Unknown TradingPhase string `incorrect_value'"));
}

TEST(TradingSystemJsonEnumerators, OutputsErrorOnWriteIncorrectTradingPhase) {
  rapidjson::Document doc;
  rapidjson::Value value;
  constexpr TradingPhase phase{invalid_enum_value<TradingPhase::Option>()};

  ASSERT_THAT(write(value, doc.GetAllocator(), phase),
              ReturnsError("Unknown TradingPhase value `255'"));
}

struct TradingSystemMarketStateTradingPhaseConversion
    : public TestWithParam<std::pair<TradingPhase, std::string>> {};

TEST_P(TradingSystemMarketStateTradingPhaseConversion, ReadsTradingPhase) {
  rapidjson::Value value;
  value.SetString(GetParam().second.c_str(),
                  static_cast<rapidjson::SizeType>(GetParam().second.size()));
  TradingPhase phase{TradingPhase::Option::Open};
  ASSERT_TRUE(read(value, phase).has_value());
  ASSERT_EQ(phase, GetParam().first);
}

TEST_P(TradingSystemMarketStateTradingPhaseConversion, WritesTradingPhase) {
  rapidjson::Document doc;
  rapidjson::Value value;
  const TradingPhase phase{GetParam().first};

  ASSERT_TRUE(write(value, doc.GetAllocator(), phase).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), GetParam().second.c_str());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Conversion,
    TradingSystemMarketStateTradingPhaseConversion,
    Values(std::make_pair(TradingPhase::Option::Open, "Continuous"),
           std::make_pair(TradingPhase::Option::Closed, "OutOfMainSessionTrading"),
           std::make_pair(TradingPhase::Option::PostTrading, "PostTrading"),
           std::make_pair(TradingPhase::Option::OpeningAuction, "OpeningOrOpeningAuction"),
           std::make_pair(TradingPhase::Option::IntradayAuction, "ScheduledIntradayAuction"),
           std::make_pair(TradingPhase::Option::ClosingAuction, "ClosingOrClosingAuction")));
// clang-format on

TEST(TradingSystemJsonEnumerators, OutputsErrorOnReadIncorrectTradingStatus) {
  const rapidjson::Value value{"incorrect_value"};
  TradingStatus status{TradingStatus::Option::Halt};

  ASSERT_THAT(read(value, status),
              ReturnsError("Unknown TradingStatus string `incorrect_value'"));
}

TEST(TradingSystemJsonEnumerators, OutputsErrorOnWriteIncorrectTradingStatus) {
  rapidjson::Document doc;
  rapidjson::Value value;
  constexpr TradingStatus status{invalid_enum_value<TradingStatus::Option>()};

  ASSERT_THAT(write(value, doc.GetAllocator(), status),
              ReturnsError("Unknown TradingStatus value `255'"));
}

struct TradingSystemMarketStateTradingStatusConversion
    : public TestWithParam<std::pair<TradingStatus, std::string>> {};

TEST_P(TradingSystemMarketStateTradingStatusConversion, ReadsTradingStatus) {
  rapidjson::Value value;
  value.SetString(GetParam().second.c_str(),
                  static_cast<rapidjson::SizeType>(GetParam().second.size()));
  TradingStatus status{TradingStatus::Option::Halt};
  ASSERT_TRUE(read(value, status).has_value());
  ASSERT_EQ(status, GetParam().first);
}

TEST_P(TradingSystemMarketStateTradingStatusConversion, WritesTradingStatus) {
  rapidjson::Document doc;
  rapidjson::Value value;
  const TradingStatus status{GetParam().first};

  ASSERT_TRUE(write(value, doc.GetAllocator(), status).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), GetParam().second.c_str());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Conversion,
    TradingSystemMarketStateTradingStatusConversion,
    Values(std::make_pair(TradingStatus::Option::Halt, "TradingHalt"),
           std::make_pair(TradingStatus::Option::Resume, "Resume")));
// clang-format on

TEST(TradingSystemJsonEnumerators,
     OutputsErrorOnReadIncorrectSecurityIdSource) {
  const rapidjson::Value value{"incorrect_value"};
  SecurityIdSource source{SecurityIdSource::Option::Cusip};

  ASSERT_THAT(
      read(value, source),
      ReturnsError("Unknown SecurityIdSource string `incorrect_value'"));
}

TEST(TradingSystemJsonEnumerators,
     OutputsErrorOnWriteIncorrectSecurityIdSource) {
  rapidjson::Document doc;
  rapidjson::Value value;
  constexpr SecurityIdSource source{
      invalid_enum_value<SecurityIdSource::Option>()};

  ASSERT_THAT(write(value, doc.GetAllocator(), source),
              ReturnsError("Unknown SecurityIdSource value `255'"));
}

struct TradingSystemMarketStateSecurityIdSourceConversion
    : public TestWithParam<std::pair<SecurityIdSource, std::string>> {};

TEST_P(TradingSystemMarketStateSecurityIdSourceConversion,
       ReadsSecurityIdSource) {
  rapidjson::Value value;
  value.SetString(GetParam().second.c_str(),
                  static_cast<rapidjson::SizeType>(GetParam().second.size()));
  SecurityIdSource source{SecurityIdSource::Option::Cusip};
  ASSERT_TRUE(read(value, source).has_value());
  ASSERT_EQ(source, GetParam().first);
}

TEST_P(TradingSystemMarketStateSecurityIdSourceConversion,
       WritesSecurityIdSource) {
  rapidjson::Document doc;
  rapidjson::Value value;
  const SecurityIdSource source{GetParam().first};

  ASSERT_TRUE(write(value, doc.GetAllocator(), source).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), GetParam().second.c_str());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Conversion,
    TradingSystemMarketStateSecurityIdSourceConversion,
    Values(std::make_pair(SecurityIdSource::Option::Cusip, "CUSIP"),
           std::make_pair(SecurityIdSource::Option::Sedol, "SEDOL"),
           std::make_pair(SecurityIdSource::Option::Isin, "ISIN"),
           std::make_pair(SecurityIdSource::Option::Ric, "RIC"),
           std::make_pair(SecurityIdSource::Option::ExchangeSymbol, "ExchangeSymbol"),
           std::make_pair(SecurityIdSource::Option::BloombergSymbol, "BloombergSymbol")));
// clang-format on

TEST(TradingSystemJsonEnumerators, OutputsErrorOnReadIncorrectTimeInForce) {
  const rapidjson::Value value{"incorrect_value"};
  TimeInForce time_in_force{TimeInForce::Option::Day};

  ASSERT_THAT(read(value, time_in_force),
              ReturnsError("Unknown TimeInForce string `incorrect_value'"));
}

TEST(TradingSystemJsonEnumerators, OutputsErrorOnWriteIncorrectTimeInForce) {
  rapidjson::Document doc;
  rapidjson::Value value;
  constexpr TimeInForce time_in_force{
      invalid_enum_value<TimeInForce::Option>()};

  ASSERT_THAT(write(value, doc.GetAllocator(), time_in_force),
              ReturnsError("Unknown TimeInForce value `255'"));
}

struct TradingSystemMarketStateTimeInForceConversion
    : public TestWithParam<std::pair<TimeInForce, std::string>> {};

TEST_P(TradingSystemMarketStateTimeInForceConversion, ReadsTimeInForce) {
  rapidjson::Value value;
  value.SetString(GetParam().second.c_str(),
                  static_cast<rapidjson::SizeType>(GetParam().second.size()));
  TimeInForce time_in_force{TimeInForce::Option::Day};
  ASSERT_TRUE(read(value, time_in_force).has_value());
  ASSERT_EQ(time_in_force, GetParam().first);
}

TEST_P(TradingSystemMarketStateTimeInForceConversion, WritesTimeInForce) {
  rapidjson::Document doc;
  rapidjson::Value value;
  const TimeInForce time_in_force{GetParam().first};

  ASSERT_TRUE(write(value, doc.GetAllocator(), time_in_force).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), GetParam().second.c_str());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    TradingSystemMarketStateTimeInForceConversion,
    Values(std::make_pair(TimeInForce::Option::Day, "Day"),
           std::make_pair(TimeInForce::Option::ImmediateOrCancel, "ImmediateOrCancel"),
           std::make_pair(TimeInForce::Option::FillOrKill, "FillOrKill"),
           std::make_pair(TimeInForce::Option::GoodTillDate, "GoodTillDate"),
           std::make_pair(TimeInForce::Option::GoodTillCancel, "GoodTillCancel")));
// clang-format on

TEST(TradingSystemJsonEnumerators, OutputsErrorOnReadIncorrectSide) {
  const rapidjson::Value value{"incorrect_value"};
  Side side{Side::Option::Buy};
  ASSERT_THAT(read(value, side),
              ReturnsError("Unknown Side string `incorrect_value'"));
}

TEST(TradingSystemJsonEnumerators, OutputsErrorOnWriteIncorrectSide) {
  rapidjson::Document doc;
  rapidjson::Value value;
  constexpr Side side{invalid_enum_value<Side::Option>()};

  ASSERT_THAT(write(value, doc.GetAllocator(), side),
              ReturnsError("Unknown Side value `255'"));
}

struct TradingSystemMarketStateSideConversion
    : public TestWithParam<std::pair<Side, std::string>> {};

TEST_P(TradingSystemMarketStateSideConversion, ReadsSide) {
  rapidjson::Value value;
  value.SetString(GetParam().second.c_str(),
                  static_cast<rapidjson::SizeType>(GetParam().second.size()));
  Side side{Side::Option::Buy};
  ASSERT_TRUE(read(value, side).has_value());
  ASSERT_EQ(side, GetParam().first);
}

TEST_P(TradingSystemMarketStateSideConversion, WritesSide) {
  rapidjson::Document doc;
  rapidjson::Value value;
  const Side side{GetParam().first};

  ASSERT_TRUE(write(value, doc.GetAllocator(), side).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), GetParam().second.c_str());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    TradingSystemMarketStateSideConversion,
    Values(std::make_pair(Side::Option::Buy, "Buy"),
           std::make_pair(Side::Option::Sell, "Sell"),
           std::make_pair(Side::Option::SellShort, "SellShort"),
           std::make_pair(Side::Option::SellShortExempt, "SellShortExempt")));
// clang-format on

TEST(TradingSystemJsonEnumerators, OutputsErrorOnReadIncorrectOrderStatus) {
  const rapidjson::Value value{"incorrect_value"};
  OrderStatus status{OrderStatus::Option::New};

  ASSERT_THAT(read(value, status),
              ReturnsError("Unknown OrderStatus string `incorrect_value'"));
}

TEST(TradingSystemJsonEnumerators, OutputsErrorOnWriteIncorrectOrderStatus) {
  rapidjson::Document doc;
  rapidjson::Value value;
  constexpr OrderStatus status{invalid_enum_value<OrderStatus::Option>()};

  ASSERT_THAT(write(value, doc.GetAllocator(), status),
              ReturnsError("Unknown OrderStatus value `255'"));
}

struct TradingSystemMarketStateOrderStatusConversion
    : public TestWithParam<std::pair<OrderStatus, std::string>> {};

TEST_P(TradingSystemMarketStateOrderStatusConversion, ReadsOrderStatus) {
  rapidjson::Value value;
  value.SetString(GetParam().second.c_str(),
                  static_cast<rapidjson::SizeType>(GetParam().second.size()));
  OrderStatus status{OrderStatus::Option::New};
  ASSERT_TRUE(read(value, status).has_value());
  ASSERT_EQ(status, GetParam().first);
}

TEST_P(TradingSystemMarketStateOrderStatusConversion, WritesOrderStatus) {
  rapidjson::Document doc;
  rapidjson::Value value;
  const OrderStatus status{GetParam().first};

  ASSERT_TRUE(write(value, doc.GetAllocator(), status).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), GetParam().second.c_str());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    TradingSystemMarketStateOrderStatusConversion,
    Values(std::make_pair(OrderStatus::Option::New, "New"),
           std::make_pair(OrderStatus::Option::PartiallyFilled, "PartiallyFilled"),
           std::make_pair(OrderStatus::Option::Filled, "Filled"),
           std::make_pair(OrderStatus::Option::Modified, "Replaced"),
           std::make_pair(OrderStatus::Option::Cancelled, "Canceled"),
           std::make_pair(OrderStatus::Option::Rejected, "Rejected")));
// clang-format on

TEST(TradingSystemJsonEnumerators, OutputsErrorOnReadIncorrectSessionType) {
  const rapidjson::Value value{"incorrect_value"};
  market_state::SessionType type{market_state::SessionType::Fix};

  ASSERT_THAT(read(value, type),
              ReturnsError("Unknown SessionType string `incorrect_value'"));
}

TEST(TradingSystemJsonEnumerators, OutputsErrorOnWriteIncorrectSessionType) {
  rapidjson::Document doc;
  rapidjson::Value value;
  constexpr market_state::SessionType type{
      invalid_enum_value<market_state::SessionType>()};

  ASSERT_THAT(write(value, doc.GetAllocator(), type),
              ReturnsError("Unknown SessionType value `255'"));
}

struct TradingSystemMarketStateSessionTypeConversion
    : public TestWithParam<std::pair<market_state::SessionType, std::string>> {
};

TEST_P(TradingSystemMarketStateSessionTypeConversion, ReadsSessionType) {
  rapidjson::Value value;
  value.SetString(GetParam().second.c_str(),
                  static_cast<rapidjson::SizeType>(GetParam().second.size()));
  market_state::SessionType type{market_state::SessionType::Fix};
  ASSERT_TRUE(read(value, type).has_value());
  ASSERT_EQ(type, GetParam().first);
}

TEST_P(TradingSystemMarketStateSessionTypeConversion, WritesSessionType) {
  rapidjson::Document doc;
  rapidjson::Value value;
  const market_state::SessionType type{GetParam().first};

  ASSERT_TRUE(write(value, doc.GetAllocator(), type).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), GetParam().second.c_str());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    TradingSystemMarketStateSessionTypeConversion,
    Values(std::make_pair(market_state::SessionType::Fix, "Fix"),
           std::make_pair(market_state::SessionType::Generator, "Generator")));
// clang-format on

}  // namespace
}  // namespace simulator::trading_system::json::test
