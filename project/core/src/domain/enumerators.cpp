#include "core/domain/enumerators.hpp"

#include <fmt/format.h>

#include <map>
#include <optional>
#include <ostream>
#include <string_view>

#include "core/common/enum_converter.hpp"

namespace simulator::core::enumerators {

template <>
EnumConverter<ExecutionType> EnumConverter<ExecutionType>::instance_{
    {{enumerators::ExecutionType::OrderPlaced, "OrderPlaced"},
     {enumerators::ExecutionType::OrderModified, "OrderModified"},
     {enumerators::ExecutionType::OrderCancelled, "OrderCancelled"},
     {enumerators::ExecutionType::Rejected, "Rejected"},
     {enumerators::ExecutionType::OrderTraded, "OrderTraded"}}};

template <>
EnumConverter<OrderStatus> EnumConverter<OrderStatus>::instance_{
    {{enumerators::OrderStatus::New, "New"},
     {enumerators::OrderStatus::PartiallyFilled, "PartiallyFilled"},
     {enumerators::OrderStatus::Filled, "Filled"},
     {enumerators::OrderStatus::Modified, "Modified"},
     {enumerators::OrderStatus::Cancelled, "Cancelled"},
     {enumerators::OrderStatus::Rejected, "Rejected"}}};

template <>
EnumConverter<OrderType> EnumConverter<OrderType>::instance_{
    {{enumerators::OrderType::Limit, "Limit"},
     {enumerators::OrderType::Market, "Market"}}};

// clang-format off
template <>
EnumConverter<PartyIdentifierSource> EnumConverter<
    PartyIdentifierSource>::instance_{
    {{enumerators::PartyIdentifierSource::UKNationalInsuranceOrPensionNumber, "UKNationalInsuranceOrPensionNumber"},
     {enumerators::PartyIdentifierSource::USSocialSecurityNumber, "USSocialSecurityNumber"},
     {enumerators::PartyIdentifierSource::USEmployerOrTaxIDNumber, "USEmployerOrTaxIDNumber"},
     {enumerators::PartyIdentifierSource::AustralianBusinessNumber, "AustralianBusinessNumber"},
     {enumerators::PartyIdentifierSource::AustralianTaxFileNumber, "AustralianTaxFileNumber"},
     {enumerators::PartyIdentifierSource::TaxID, "TaxID"},
     {enumerators::PartyIdentifierSource::KoreanInvestorID, "KoreanInvestorID"},
     {enumerators::PartyIdentifierSource::TaiwaneseForeignInvestorID, "TaiwaneseForeignInvestorID"},
     {enumerators::PartyIdentifierSource::TaiwaneseTradingAcct, "TaiwaneseTradingAcct"},
     {enumerators::PartyIdentifierSource::MalaysianCentralDepository, "MalaysianCentralDepository"},
     {enumerators::PartyIdentifierSource::ChineseInvestorID, "ChineseInvestorID"},
     {enumerators::PartyIdentifierSource::ISITCAcronym, "ISITCAcronym"},
     {enumerators::PartyIdentifierSource::BIC, "BIC"},
     {enumerators::PartyIdentifierSource::GeneralIdentifier, "GeneralIdentifier"},
     {enumerators::PartyIdentifierSource::Proprietary, "Proprietary"},
     {enumerators::PartyIdentifierSource::ISOCountryCode, "ISOCountryCode"},
     {enumerators::PartyIdentifierSource::SettlementEntityLocation, "SettlementEntityLocation"},
     {enumerators::PartyIdentifierSource::MIC, "MIC"},
     {enumerators::PartyIdentifierSource::CSDParticipant, "CSDParticipant"},
     {enumerators::PartyIdentifierSource::AustralianCompanyNumber, "AustralianCompanyNumber"},
     {enumerators::PartyIdentifierSource::AustralianRegisteredBodyNumber, "AustralianRegisteredBodyNumber"},
     {enumerators::PartyIdentifierSource::CFTCReportingFirmIdentifier, "CFTCReportingFirmIdentifier"},
     {enumerators::PartyIdentifierSource::LegalEntityIdentifier, "LegalEntityIdentifier"},
     {enumerators::PartyIdentifierSource::InterimIdentifier, "InterimIdentifier"},
     {enumerators::PartyIdentifierSource::ShortCodeIdentifier, "ShortCodeIdentifier"},
     {enumerators::PartyIdentifierSource::NationalIDNaturalPerson, "NationalIDNaturalPerson"},
     {enumerators::PartyIdentifierSource::IndiaPermanentAccountNumber, "IndiaPermanentAccountNumber"},
     {enumerators::PartyIdentifierSource::FDID, "FDID"},
     {enumerators::PartyIdentifierSource::SPSAID, "SPSAID"},
     {enumerators::PartyIdentifierSource::MasterSPSAID, "MasterSPSAID"}}};
// clang-format on

// clang-format off
template <>
EnumConverter<PartyRole> EnumConverter<PartyRole>::instance_{
    {{enumerators::PartyRole::ExecutingFirm, "ExecutingFirm"},
     {enumerators::PartyRole::BrokerOfCredit, "BrokerOfCredit"},
     {enumerators::PartyRole::ClientID, "ClientID"},
     {enumerators::PartyRole::ClearingFirm, "ClearingFirm"},
     {enumerators::PartyRole::InvestorID, "InvestorID"},
     {enumerators::PartyRole::IntroducingFirm, "IntroducingFirm"},
     {enumerators::PartyRole::EnteringFirm, "EnteringFirm"},
     {enumerators::PartyRole::Locate, "Locate"},
     {enumerators::PartyRole::FundManagerClientID, "FundManagerClientID"},
     {enumerators::PartyRole::SettlementLocation, "SettlementLocation"},
     {enumerators::PartyRole::OrderOriginationTrader, "OrderOriginationTrader"},
     {enumerators::PartyRole::ExecutingTrader, "ExecutingTrader"},
     {enumerators::PartyRole::OrderOriginationFirm, "OrderOriginationFirm"},
     {enumerators::PartyRole::CorrespondantClearingFirm,
      "CorrespondantClearingFirm"},
     {enumerators::PartyRole::ExecutingSystem, "ExecutingSystem"},
     {enumerators::PartyRole::ContraFirm, "ContraFirm"},
     {enumerators::PartyRole::ContraClearingFirm, "ContraClearingFirm"},
     {enumerators::PartyRole::SponsoringFirm, "SponsoringFirm"},
     {enumerators::PartyRole::UnderlyingContraFirm, "UnderlyingContraFirm"},
     {enumerators::PartyRole::ClearingOrganization, "ClearingOrganization"},
     {enumerators::PartyRole::Exchange, "Exchange"},
     {enumerators::PartyRole::CustomerAccount, "CustomerAccount"},
     {enumerators::PartyRole::CorrespondentClearingOrganization,
      "CorrespondentClearingOrganization"},
     {enumerators::PartyRole::CorrespondentBroker, "CorrespondentBroker"},
     {enumerators::PartyRole::Buyer, "Buyer"},
     {enumerators::PartyRole::Custodian, "Custodian"},
     {enumerators::PartyRole::Intermediary, "Intermediary"},
     {enumerators::PartyRole::Agent, "Agent"},
     {enumerators::PartyRole::SubCustodian, "SubCustodian"},
     {enumerators::PartyRole::Beneficiary, "Beneficiary"},
     {enumerators::PartyRole::InterestedParty, "InterestedParty"},
     {enumerators::PartyRole::RegulatoryBody, "RegulatoryBody"},
     {enumerators::PartyRole::LiquidityProvider, "LiquidityProvider"},
     {enumerators::PartyRole::EnteringTrader, "EnteringTrader"},
     {enumerators::PartyRole::ContraTrader, "ContraTrader"},
     {enumerators::PartyRole::PositionAccount, "PositionAccount"},
     {enumerators::PartyRole::ContraInvestorID, "ContraInvestorID"},
     {enumerators::PartyRole::TransferToFirm, "TransferToFirm"},
     {enumerators::PartyRole::ContraPositionAccount, "ContraPositionAccount"},
     {enumerators::PartyRole::ContraExchange, "ContraExchange"},
     {enumerators::PartyRole::InternalCarryAccount, "InternalCarryAccount"},
     {enumerators::PartyRole::OrderEntryOperatorID, "OrderEntryOperatorID"},
     {enumerators::PartyRole::SecondaryAccountNumber, "SecondaryAccountNumber"},
     {enumerators::PartyRole::ForeignFirm, "ForeignFirm"},
     {enumerators::PartyRole::ThirdPartyAllocationFirm,
      "ThirdPartyAllocationFirm"},
     {enumerators::PartyRole::ClaimingAccount, "ClaimingAccount"},
     {enumerators::PartyRole::AssetManager, "AssetManager"},
     {enumerators::PartyRole::PledgorAccount, "PledgorAccount"},
     {enumerators::PartyRole::PledgeeAccount, "PledgeeAccount"},
     {enumerators::PartyRole::LargeTraderReportableAccount,
      "LargeTraderReportableAccount"},
     {enumerators::PartyRole::TraderMnemonic, "TraderMnemonic"},
     {enumerators::PartyRole::SenderLocation, "SenderLocation"},
     {enumerators::PartyRole::SessionID, "SessionID"},
     {enumerators::PartyRole::AcceptableCounterparty, "AcceptableCounterparty"},
     {enumerators::PartyRole::UnacceptableCounterparty,
      "UnacceptableCounterparty"},
     {enumerators::PartyRole::EnteringUnit, "EnteringUnit"},
     {enumerators::PartyRole::ExecutingUnit, "ExecutingUnit"},
     {enumerators::PartyRole::IntroducingBroker, "IntroducingBroker"},
     {enumerators::PartyRole::QuoteOriginator, "QuoteOriginator"},
     {enumerators::PartyRole::ReportOriginator, "ReportOriginator"},
     {enumerators::PartyRole::SystematicInternaliser, "SystematicInternaliser"},
     {enumerators::PartyRole::MultilateralTradingFacility,
      "MultilateralTradingFacility"},
     {enumerators::PartyRole::RegulatedMarket, "RegulatedMarket"},
     {enumerators::PartyRole::MarketMaker, "MarketMaker"},
     {enumerators::PartyRole::InvestmentFirm, "InvestmentFirm"},
     {enumerators::PartyRole::HostCompetentAuthority, "HostCompetentAuthority"},
     {enumerators::PartyRole::HomeCompetentAuthority, "HomeCompetentAuthority"},
     {enumerators::PartyRole::CompetentAuthorityLiquidity,
      "CompetentAuthorityLiquidity"},
     {enumerators::PartyRole::CompetentAuthorityTransactionVenue,
      "CompetentAuthorityTransactionVenue"},
     {enumerators::PartyRole::ReportingIntermediary, "ReportingIntermediary"},
     {enumerators::PartyRole::ExecutionVenue, "ExecutionVenue"},
     {enumerators::PartyRole::MarketDataEntryOriginator,
      "MarketDataEntryOriginator"},
     {enumerators::PartyRole::LocationID, "LocationID"},
     {enumerators::PartyRole::DeskID, "DeskID"},
     {enumerators::PartyRole::MarketDataMarket, "MarketDataMarket"},
     {enumerators::PartyRole::AllocationEntity, "AllocationEntity"},
     {enumerators::PartyRole::PrimeBroker, "PrimeBroker"},
     {enumerators::PartyRole::StepOutFirm, "StepOutFirm"},
     {enumerators::PartyRole::BrokerClearingID, "BrokerClearingID"},
     {enumerators::PartyRole::CentralRegistrationDepository,
      "CentralRegistrationDepository"},
     {enumerators::PartyRole::ClearingAccount, "ClearingAccount"},
     {enumerators::PartyRole::AcceptableSettlingCounterparty,
      "AcceptableSettlingCounterparty"},
     {enumerators::PartyRole::UnacceptableSettlingCounterparty,
      "UnacceptableSettlingCounterparty"},
     {enumerators::PartyRole::CLSMemberBank, "CLSMemberBank"},
     {enumerators::PartyRole::InConcertGroup, "InConcertGroup"},
     {enumerators::PartyRole::InConcertControllingEntity,
      "InConcertControllingEntity"},
     {enumerators::PartyRole::LargePositionsReportingAccount,
      "LargePositionsReportingAccount"},
     {enumerators::PartyRole::SettlementFirm, "SettlementFirm"},
     {enumerators::PartyRole::SettlementAccount, "SettlementAccount"},
     {enumerators::PartyRole::ReportingMarketCenter, "ReportingMarketCenter"},
     {enumerators::PartyRole::RelatedReportingMarketCenter,
      "RelatedReportingMarketCenter"},
     {enumerators::PartyRole::AwayMarket, "AwayMarket"},
     {enumerators::PartyRole::GiveupTradingFirm, "GiveupTradingFirm"},
     {enumerators::PartyRole::TakeupTradingFirm, "TakeupTradingFirm"},
     {enumerators::PartyRole::GiveupClearingFirm, "GiveupClearingFirm"},
     {enumerators::PartyRole::TakeupClearingFirm, "TakeupClearingFirm"},
     {enumerators::PartyRole::OriginatingMarket, "OriginatingMarket"},
     {enumerators::PartyRole::MarginAccount, "MarginAccount"},
     {enumerators::PartyRole::CollateralAssetAccount, "CollateralAssetAccount"},
     {enumerators::PartyRole::DataRepository, "DataRepository"},
     {enumerators::PartyRole::CalculationAgent, "CalculationAgent"},
     {enumerators::PartyRole::ExerciseNoticeSender, "ExerciseNoticeSender"},
     {enumerators::PartyRole::ExerciseNoticeReceiver, "ExerciseNoticeReceiver"},
     {enumerators::PartyRole::RateReferenceBank, "RateReferenceBank"},
     {enumerators::PartyRole::Correspondent, "Correspondent"},
     {enumerators::PartyRole::BeneficiaryBank, "BeneficiaryBank"},
     {enumerators::PartyRole::Borrower, "Borrower"},
     {enumerators::PartyRole::PrimaryObligator, "PrimaryObligator"},
     {enumerators::PartyRole::Guarantor, "Guarantor"},
     {enumerators::PartyRole::ExcludedReferenceEntity,
      "ExcludedReferenceEntity"},
     {enumerators::PartyRole::DeterminingParty, "DeterminingParty"},
     {enumerators::PartyRole::HedgingParty, "HedgingParty"},
     {enumerators::PartyRole::ReportingEntity, "ReportingEntity"},
     {enumerators::PartyRole::SalesPerson, "SalesPerson"},
     {enumerators::PartyRole::Operator, "Operator"},
     {enumerators::PartyRole::CSD, "CSD"},
     {enumerators::PartyRole::ICSD, "ICSD"},
     {enumerators::PartyRole::TradingSubAccount, "TradingSubAccount"},
     {enumerators::PartyRole::InvestmentDecisionMaker,
      "InvestmentDecisionMaker"},
     {enumerators::PartyRole::PublishingIntermediary, "PublishingIntermediary"},
     {enumerators::PartyRole::CSDParticipant, "CSDParticipant"},
     {enumerators::PartyRole::Issuer, "Issuer"},
     {enumerators::PartyRole::ContraCustomerAccount, "ContraCustomerAccount"},
     {enumerators::PartyRole::ContraInvestmentDecisionMaker,
      "ContraInvestmentDecisionMaker"}}};
// clang-format on

// clang-format off
template <>
EnumConverter<SecurityIdSource> EnumConverter<SecurityIdSource>::instance_{
    {{enumerators::SecurityIdSource::Cusip, "CUSIP"},
     {enumerators::SecurityIdSource::Sedol, "SEDOL"},
     {enumerators::SecurityIdSource::Isin, "ISIN"},
     {enumerators::SecurityIdSource::Ric, "RIC"},
     {enumerators::SecurityIdSource::ExchangeSymbol, "ExchangeSymbol"},
     {enumerators::SecurityIdSource::BloombergSymbol, "BloombergSymbol"}}};
// clang-format on

// clang-format off
template <>
EnumConverter<SecurityType> EnumConverter<SecurityType>::instance_{
    {{enumerators::SecurityType::CommonStock, "CommonStock"},
     {enumerators::SecurityType::Future, "Future"},
     {enumerators::SecurityType::Option, "Option"},
     {enumerators::SecurityType::MultiLeg, "MultiLeg"},
     {enumerators::SecurityType::SyntheticMultiLeg, "SyntheticMultiLeg"},
     {enumerators::SecurityType::Warrant, "Warrant"},
     {enumerators::SecurityType::MutualFund, "MutualFund"},
     {enumerators::SecurityType::CorporateBond, "CorporateBond"},
     {enumerators::SecurityType::ConvertibleBond, "ConvertibleBond"},
     {enumerators::SecurityType::RepurchaseAgreement, "RepurchaseAgreement"},
     {enumerators::SecurityType::Index, "Index"},
     {enumerators::SecurityType::ContractForDifference, "ContractForDifference"},
     {enumerators::SecurityType::Certificate, "Certificate"},
     {enumerators::SecurityType::FxSpot, "FxSpot"},
     {enumerators::SecurityType::Forward, "Forward"},
     {enumerators::SecurityType::FxForward, "FxForward"},
     {enumerators::SecurityType::FxNonDeliverableForward, "FxNonDeliverableForward"},
     {enumerators::SecurityType::FxSwap, "FxSwap"},
     {enumerators::SecurityType::FxNonDeliverableSwap, "FxNonDeliverableSwap"}}};
// clang-format on

template <>
EnumConverter<Side> EnumConverter<Side>::instance_{
    {{enumerators::Side::Buy, "Buy"},
     {enumerators::Side::Sell, "Sell"},
     {enumerators::Side::SellShort, "SellShort"},
     {enumerators::Side::SellShortExempt, "SellShortExempt"}}};

template <>
EnumConverter<TimeInForce> EnumConverter<TimeInForce>::instance_{
    {{enumerators::TimeInForce::Day, "Day"},
     {enumerators::TimeInForce::ImmediateOrCancel, "ImmediateOrCancel"},
     {enumerators::TimeInForce::FillOrKill, "FillOrKill"},
     {enumerators::TimeInForce::GoodTillDate, "GoodTillDate"},
     {enumerators::TimeInForce::GoodTillCancel, "GoodTillCancel"}}};

template <>
EnumConverter<MdEntryType> EnumConverter<MdEntryType>::instance_{
    {{enumerators::MdEntryType::Bid, "Bid"},
     {enumerators::MdEntryType::Offer, "Offer"},
     {enumerators::MdEntryType::Trade, "Trade"},
     {enumerators::MdEntryType::LowPrice, "LowPrice"},
     {enumerators::MdEntryType::MidPrice, "MidPrice"},
     {enumerators::MdEntryType::HighPrice, "HighPrice"}}};

template <>
EnumConverter<MdSubscriptionRequestType>
    EnumConverter<MdSubscriptionRequestType>::instance_{
        {{enumerators::MdSubscriptionRequestType::Subscribe, "Subscribe"},
         {enumerators::MdSubscriptionRequestType::Unsubscribe, "Unsubscribe"},
         {enumerators::MdSubscriptionRequestType::Snapshot, "Snapshot"}}};

template <>
EnumConverter<MarketEntryAction> EnumConverter<MarketEntryAction>::instance_{
    {{enumerators::MarketEntryAction::New, "New"},
     {enumerators::MarketEntryAction::Change, "Change"},
     {enumerators::MarketEntryAction::Delete, "Delete"}}};

template <>
EnumConverter<MarketDataUpdateType>
    EnumConverter<MarketDataUpdateType>::instance_{
        {{enumerators::MarketDataUpdateType::Snapshot, "Snapshot"},
         {enumerators::MarketDataUpdateType::Incremental, "Incremental"}}};

template <>
EnumConverter<MdRejectReason> EnumConverter<MdRejectReason>::instance_{
    {{enumerators::MdRejectReason::UnknownSymbol, "UnknownSymbol"},
     {enumerators::MdRejectReason::DuplicateMdReqId, "DuplicateMdReqId"}}};

template <>
EnumConverter<TradingStatus> EnumConverter<TradingStatus>::instance_{
    {{enumerators::TradingStatus::Halt, "Halt"},
     {enumerators::TradingStatus::Resume, "Resume"}}};

// clang-format off
template <>
EnumConverter<RejectedMessageType>
    EnumConverter<RejectedMessageType>::instance_{
        {{enumerators::RejectedMessageType::SecurityStatusRequest, "SecurityStatusRequest"}}};
// clang-format on

// clang-format off
template <>
EnumConverter<BusinessRejectReason>
    EnumConverter<BusinessRejectReason>::instance_{
        {{enumerators::BusinessRejectReason::Other, "Other"},
         {enumerators::BusinessRejectReason::UnknownId, "UnknownId"},
         {enumerators::BusinessRejectReason::UnknownSecurity, "UnknownSecurity"}}};
// clang-format on

template <>
EnumConverter<TradingPhase> EnumConverter<TradingPhase>::instance_{
    {{enumerators::TradingPhase::Open, "Open"},
     {enumerators::TradingPhase::Closed, "Closed"},
     {enumerators::TradingPhase::PostTrading, "PostTrading"},
     {enumerators::TradingPhase::OpeningAuction, "OpeningAuction"},
     {enumerators::TradingPhase::IntradayAuction, "IntradayAuction"},
     {enumerators::TradingPhase::ClosingAuction, "ClosingAuction"}}};

auto convert_security_type_from_string(std::string_view value) noexcept
    -> std::optional<SecurityType> {
  static const std::map<std::string_view, SecurityType> association{
      {"CS", SecurityType::CommonStock},
      {"FUT", SecurityType::Future},
      {"OPT", SecurityType::Option},
      {"MLEG", SecurityType::MultiLeg},
      {"SML", SecurityType::SyntheticMultiLeg},
      {"WAR", SecurityType::Warrant},
      {"MF", SecurityType::MutualFund},
      {"CORP", SecurityType::CorporateBond},
      {"CB", SecurityType::ConvertibleBond},
      {"REPO", SecurityType::RepurchaseAgreement},
      {"INDEX", SecurityType::Index},
      {"CFD", SecurityType::ContractForDifference},
      {"CD", SecurityType::Certificate},
      {"FXSPOT", SecurityType::FxSpot},
      {"FORWARD", SecurityType::Forward},
      {"FXFWD", SecurityType::FxForward},
      {"FXNDF", SecurityType::FxNonDeliverableForward},
      {"FXSWAP", SecurityType::FxSwap},
      {"FXNDS", SecurityType::FxNonDeliverableSwap}};

  const auto iter = association.find(value);
  return iter != association.end()
             ? std::make_optional<SecurityType>(iter->second)
             : std::nullopt;
}

auto convert_party_role_from_string(std::string_view value) noexcept
    -> std::optional<PartyRole> {
  try {
    return std::make_optional(core::EnumConverter<PartyRole>::value(value));
  } catch (const std::runtime_error& ex) {
    return std::nullopt;
  }
}

auto operator<<(std::ostream& stream, ExecutionType execution_type)
    -> std::ostream& {
  return stream << fmt::to_string(execution_type);
}

auto operator<<(std::ostream& stream, OrderStatus order_status)
    -> std::ostream& {
  return stream << fmt::to_string(order_status);
}

auto operator<<(std::ostream& stream, OrderType order_type) -> std::ostream& {
  return stream << fmt::to_string(order_type);
}

auto operator<<(std::ostream& stream, PartyIdentifierSource party_id_source)
    -> std::ostream& {
  return stream << fmt::to_string(party_id_source);
}

auto operator<<(std::ostream& stream, PartyRole party_role) -> std::ostream& {
  return stream << fmt::to_string(party_role);
}

auto operator<<(std::ostream& stream, SecurityIdSource security_id_source)
    -> std::ostream& {
  return stream << fmt::to_string(security_id_source);
}

auto operator<<(std::ostream& stream, SecurityType security_type)
    -> std::ostream& {
  return stream << fmt::to_string(security_type);
}

auto operator<<(std::ostream& stream, Side side) -> std::ostream& {
  return stream << fmt::to_string(side);
}

auto operator<<(std::ostream& stream, TimeInForce time_in_force)
    -> std::ostream& {
  return stream << fmt::to_string(time_in_force);
}

auto operator<<(std::ostream& stream, MdEntryType md_entry_type)
    -> std::ostream& {
  return stream << fmt::to_string(md_entry_type);
}

auto operator<<(std::ostream& stream, MdSubscriptionRequestType request_type)
    -> std::ostream& {
  return stream << fmt::to_string(request_type);
}

auto operator<<(std::ostream& stream, MdRejectReason reject_reason)
    -> std::ostream& {
  return stream << fmt::to_string(reject_reason);
}

auto operator<<(std::ostream& stream, MarketEntryAction action)
    -> std::ostream& {
  return stream << fmt::to_string(action);
}

auto operator<<(std::ostream& stream, MarketDataUpdateType type)
    -> std::ostream& {
  return stream << fmt::to_string(type);
}

auto operator<<(std::ostream& stream, TradingPhase phase) -> std::ostream& {
  return stream << fmt::to_string(phase);
}

auto operator<<(std::ostream& stream, TradingStatus status) -> std::ostream& {
  return stream << fmt::to_string(status);
}

auto operator<<(std::ostream& stream, RejectedMessageType type)
    -> std::ostream& {
  return stream << fmt::to_string(type);
}

auto operator<<(std::ostream& stream, BusinessRejectReason reason)
    -> std::ostream& {
  return stream << fmt::to_string(reason);
}

}  // namespace simulator::core::enumerators

auto fmt::formatter<simulator::core::enumerators::ExecutionType>::format(
    formattable execution_type, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using ExecutionType = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<ExecutionType>::str(execution_type),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::OrderStatus>::format(
    formattable order_status, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using OrderStatus = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<OrderStatus>::str(order_status),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::OrderType>::format(
    formattable order_type, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using OrderType = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<OrderType>::str(order_type), context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::PartyIdentifierSource>::
    format(formattable party_id_source, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using PartyIdentifierSource = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<PartyIdentifierSource>::str(
            party_id_source),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::PartyRole>::format(
    formattable party_role, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using PartyRole = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<PartyRole>::str(party_role), context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::SecurityIdSource>::format(
    formattable security_id_source, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using SecurityIdSource = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<SecurityIdSource>::str(
            security_id_source),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::SecurityType>::format(
    formattable security_type, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using SecurityType = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<SecurityType>::str(security_type),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::Side>::format(
    formattable side, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using Side = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<Side>::str(side), context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::TimeInForce>::format(
    formattable time_in_force, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using TimeInForce = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<TimeInForce>::str(time_in_force),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::MdEntryType>::format(
    formattable md_entry_type, fmt::format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using MdEntryType = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<MdEntryType>::str(md_entry_type),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::MdSubscriptionRequestType>::
    format(formattable subscription_type, fmt::format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using MdSubscriptionRequestType = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<MdSubscriptionRequestType>::str(
            subscription_type),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::MdRejectReason>::format(
    formattable reject_reason, fmt::format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using MdRejectReason = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<MdRejectReason>::str(reject_reason),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::MarketEntryAction>::format(
    formattable action, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using MarketEntryAction = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<MarketEntryAction>::str(action),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::MarketDataUpdateType>::format(
    formattable type, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using MarketDataUpdateType =
      simulator::core::enumerators::MarketDataUpdateType;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<MarketDataUpdateType>::str(type),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::TradingPhase>::format(
    formattable phase, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using TradingPhase = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<TradingPhase>::str(phase), context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::TradingStatus>::format(
    formattable status, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using TradingStatus = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<TradingStatus>::str(status), context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::RejectedMessageType>::format(
    formattable type, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using RejectedMessageType = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<RejectedMessageType>::str(type),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}

auto fmt::formatter<simulator::core::enumerators::BusinessRejectReason>::format(
    formattable reason, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using BusinessRejectReason = formattable;

  try {
    return base_formatter::format(
        simulator::core::EnumConverter<BusinessRejectReason>::str(reason),
        context);
  } catch (const std::runtime_error& ex) {
    return base_formatter::format("undefined", context);
  }
}