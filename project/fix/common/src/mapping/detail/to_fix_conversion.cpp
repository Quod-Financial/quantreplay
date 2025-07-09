#include "common/mapping/detail/to_fix_conversion.hpp"

#include <fmt/format.h>
#include <quickfix/FixValues.h>

#include <string>
#include <unordered_map>

#include "common/custom_values.hpp"
#include "core/common/meta.hpp"
#include "core/domain/enumerators.hpp"

namespace simulator::fix::detail {

namespace {

template <typename KeyType, typename EnumType>
using EnumAssociationTable = std::unordered_map<KeyType, EnumType>;

[[nodiscard]]
auto make_order_status_association()
    -> EnumAssociationTable<core::enumerators::OrderStatus, char> {
  using core::enumerators::OrderStatus;

  // clang-format off
  return {
    {OrderStatus::New, FIX::OrdStatus_NEW},
    {OrderStatus::Modified, FIX::OrdStatus_REPLACED},
    {OrderStatus::Cancelled, FIX::OrdStatus_CANCELED},
    {OrderStatus::PartiallyFilled, FIX::OrdStatus_PARTIALLY_FILLED},
    {OrderStatus::Filled, FIX::OrdStatus_FILLED},
    {OrderStatus::Rejected, FIX::OrdStatus_REJECTED}};
  // clang-format on
}

[[nodiscard]]
auto make_order_type_association()
    -> EnumAssociationTable<core::enumerators::OrderType, char> {
  using core::enumerators::OrderType;

  // clang-format off
  return {
    {OrderType::Limit, FIX::OrdType_LIMIT},
    {OrderType::Market, FIX::OrdType_MARKET}};
  // clang-format on
}

[[nodiscard]]
auto make_side_association()
    -> EnumAssociationTable<core::enumerators::Side, char> {
  using core::enumerators::Side;

  // clang-format off
  return {
    {Side::Buy, FIX::Side_BUY},
    {Side::Sell, FIX::Side_SELL},
    {Side::SellShort, FIX::Side_SELL_SHORT},
    {Side::SellShortExempt, FIX::Side_SELL_SHORT_EXEMPT}};
  // clang-format on
}

[[nodiscard]]
auto make_time_in_force_association()
    -> EnumAssociationTable<core::enumerators::TimeInForce, char> {
  using core::enumerators::TimeInForce;

  // clang-format off
  return {
    {TimeInForce::Day, FIX::TimeInForce_DAY},
    {TimeInForce::ImmediateOrCancel, FIX::TimeInForce_IMMEDIATE_OR_CANCEL},
    {TimeInForce::FillOrKill, FIX::TimeInForce_FILL_OR_KILL},
    {TimeInForce::GoodTillDate, FIX::TimeInForce_GOOD_TILL_DATE},
    {TimeInForce::GoodTillCancel, FIX::TimeInForce_GOOD_TILL_CANCEL}};
  // clang-format on
}

[[nodiscard]]
auto make_security_type_association()
    -> EnumAssociationTable<core::enumerators::SecurityType, std::string> {
  using core::enumerators::SecurityType;

  // clang-format off
  return {
    {SecurityType::CommonStock, FIX::SecurityType_COMMON_STOCK},
    {SecurityType::Future, FIX::SecurityType_FUTURE},
    {SecurityType::Option, FIX::SecurityType_OPTION},
    {SecurityType::MultiLeg, FIX::SecurityType_MULTILEG_INSTRUMENT},
    {SecurityType::SyntheticMultiLeg, FIX::SecurityType_SYNTHETIC_MULTILEG_INSTRUMENT},
    {SecurityType::Warrant, FIX::SecurityType_WARRANT},
    {SecurityType::MutualFund, FIX::SecurityType_MUTUAL_FUND},
    {SecurityType::CorporateBond, FIX::SecurityType_CORPORATE_BOND},
    {SecurityType::ConvertibleBond, FIX::SecurityType_CONVERTIBLE_BOND},
    {SecurityType::RepurchaseAgreement, FIX::SecurityType_REPURCHASE_AGREEMENT},
    {SecurityType::Index, FIX::SecurityType_INDEX},
    {SecurityType::ContractForDifference, FIX::SecurityType_CONTRACT_FOR_DIFFERENCE},
    {SecurityType::Certificate, FIX::SecurityType_CERTIFICATE_OF_DEPOSIT},
    {SecurityType::FxSpot, FIX::SecurityType_FX_SPOT},
    {SecurityType::Forward, FIX::SecurityType_FORWARD},
    {SecurityType::FxForward, FIX::SecurityType_FX_FORWARD},
    {SecurityType::FxNonDeliverableForward, FIX::SecurityType_NON_DELIVERABLE_FORWARD},
    {SecurityType::FxSwap, FIX::SecurityType_FX_SWAP},
    {SecurityType::FxNonDeliverableSwap, FIX::SecurityType_NON_DELIVERABLE_SWAP}};
  // clang-format on
}

[[nodiscard]]
auto make_security_id_source_association()
    -> EnumAssociationTable<core::enumerators::SecurityIdSource, std::string> {
  using core::enumerators::SecurityIdSource;

  // clang-format off
  return {
    {SecurityIdSource::Cusip, FIX::SecurityIDSource_CUSIP},
    {SecurityIdSource::Sedol, FIX::SecurityIDSource_SEDOL},
    {SecurityIdSource::Isin, FIX::SecurityIDSource_ISIN_NUMBER},
    {SecurityIdSource::Ric, FIX::SecurityIDSource_RIC_CODE},
    {SecurityIdSource::ExchangeSymbol, FIX::SecurityIDSource_EXCHANGE_SYMBOL},
    {SecurityIdSource::BloombergSymbol, FIX::SecurityIDSource_BLOOMBERG_SYMBOL}};
  // clang-format on
}

[[nodiscard]]
auto make_party_id_source_association()
    -> EnumAssociationTable<core::enumerators::PartyIdentifierSource, char> {
  using core::enumerators::PartyIdentifierSource;

  // clang-format off
  return {
    {PartyIdentifierSource::UKNationalInsuranceOrPensionNumber, FIX::PartyIDSource_UK_NATIONAL_INSURANCE_OR_PENSION_NUMBER},
    {PartyIdentifierSource::USSocialSecurityNumber, FIX::PartyIDSource_US_SOCIAL_SECURITY_NUMBER},
    {PartyIdentifierSource::USEmployerOrTaxIDNumber, FIX::PartyIDSource_US_EMPLOYER_OR_TAX_ID_NUMBER},
    {PartyIdentifierSource::AustralianBusinessNumber, FIX::PartyIDSource_AUSTRALIAN_BUSINESS_NUMBER},
    {PartyIdentifierSource::AustralianTaxFileNumber, FIX::PartyIDSource_AUSTRALIAN_TAX_FILE_NUMBER},
    {PartyIdentifierSource::TaxID, FIX::PartyIDSource_TAX_ID},
    {PartyIdentifierSource::KoreanInvestorID, FIX::PartyIDSource_KOREAN_INVESTOR_ID},
    {PartyIdentifierSource::TaiwaneseForeignInvestorID, FIX::PartyIDSource_TAIWANESE_QUALIFIED_FOREIGN_INVESTOR_ID_QFII_FID},
    {PartyIdentifierSource::TaiwaneseTradingAcct, FIX::PartyIDSource_TAIWANESE_TRADING_ACCOUNT},
    {PartyIdentifierSource::MalaysianCentralDepository, FIX::PartyIDSource_MALAYSIAN_CENTRAL_DEPOSITORY},
    {PartyIdentifierSource::ChineseInvestorID, FIX::PartyIDSource_CHINESE_INVESTOR_ID},
    {PartyIdentifierSource::ISITCAcronym, FIX::PartyIDSource_DIRECTED_BROKER_THREE_CHARACTER_ACRONYM_AS_DEFINED_IN_ISITC_ETC_BEST_PRACTICE_GUIDELINES_DOCUMENT},
    {PartyIdentifierSource::BIC, FIX::PartyIDSource_BIC},
    {PartyIdentifierSource::GeneralIdentifier, FIX::PartyIDSource_GENERALLY_ACCEPTED_MARKET_PARTICIPANT_IDENTIFIER},
    {PartyIdentifierSource::Proprietary, FIX::PartyIDSource_PROPRIETARY},
    {PartyIdentifierSource::ISOCountryCode, FIX::PartyIDSource_ISO_COUNTRY_CODE},
    {PartyIdentifierSource::SettlementEntityLocation, FIX::PartyIDSource_SETTLEMENT_ENTITY_LOCATION},
    {PartyIdentifierSource::MIC, FIX::PartyIDSource_MIC},
    {PartyIdentifierSource::CSDParticipant, FIX::PartyIDSource_CSD_PARTICIPANT_MEMBER_CODE},
    {PartyIdentifierSource::AustralianCompanyNumber, FIX::PartyIDSource_AUSTRALIAN_COMPANY_NUMBER},
    {PartyIdentifierSource::AustralianRegisteredBodyNumber, FIX::PartyIDSource_AUSTRALIAN_REGISTERED_BODY_NUMBER},
    {PartyIdentifierSource::CFTCReportingFirmIdentifier, FIX::PartyIDSource_CFTC_REPORTING_FIRM_IDENTIFIER},
    {PartyIdentifierSource::LegalEntityIdentifier, FIX::PartyIDSource_LEI},
    {PartyIdentifierSource::InterimIdentifier, FIX::PartyIDSource_INTERIM_IDENTIFIER},
    {PartyIdentifierSource::ShortCodeIdentifier, FIX::PartyIDSource_SHORT_CODE_IDENTIFIER},
    {PartyIdentifierSource::NationalIDNaturalPerson, FIX::PartyIDSource_NATIONAL_ID_NATURAL_PERSON},
    {PartyIdentifierSource::IndiaPermanentAccountNumber, FIX::PartyIDSource_INDIA_PERMANENT_ACCOUNT_NUMBER},
    {PartyIdentifierSource::FDID, FIX::PartyIDSource_FDID},
    {PartyIdentifierSource::SPSAID, FIX::PartyIDSource_SPASAID},
    {PartyIdentifierSource::MasterSPSAID, FIX::PartyIDSource_MASTER_SPASAID}};
  // clang-format on
}

[[nodiscard]]
auto make_party_role_mapping_association()
    -> EnumAssociationTable<core::enumerators::PartyRole, int> {
  using core::enumerators::PartyRole;

  // clang-format off
  return {
      {PartyRole::ExecutingFirm, FIX::PartyRole_EXECUTING_FIRM},
      {PartyRole::BrokerOfCredit, FIX::PartyRole_BROKER_OF_CREDIT},
      {PartyRole::ClientID, FIX::PartyRole_CLIENT_ID},
      {PartyRole::ClearingFirm, FIX::PartyRole_CLEARING_FIRM},
      {PartyRole::InvestorID, FIX::PartyRole_INVESTOR_ID},
      {PartyRole::IntroducingFirm, FIX::PartyRole_INTRODUCING_FIRM},
      {PartyRole::EnteringFirm, FIX::PartyRole_ENTERING_FIRM},
      {PartyRole::Locate, FIX::PartyRole_LOCATE},
      {PartyRole::FundManagerClientID, FIX::PartyRole_FUND_MANAGER_CLIENT_ID},
      {PartyRole::SettlementLocation, FIX::PartyRole_SETTLEMENT_LOCATION},
      {PartyRole::OrderOriginationTrader, FIX::PartyRole_ORDER_ORIGINATION_TRADER},
      {PartyRole::ExecutingTrader, FIX::PartyRole_EXECUTING_TRADER},
      {PartyRole::OrderOriginationFirm, FIX::PartyRole_ORDER_ORIGINATION_FIRM},
      {PartyRole::GiveupClearingFirm, FIX::PartyRole_GIVEUP_CLEARING_FIRM},
      {PartyRole::CorrespondantClearingFirm, FIX::PartyRole_CORRESPONDANT_CLEARING_FIRM},
      {PartyRole::ExecutingSystem, FIX::PartyRole_EXECUTING_SYSTEM},
      {PartyRole::ContraFirm, FIX::PartyRole_CONTRA_FIRM},
      {PartyRole::ContraClearingFirm, FIX::PartyRole_CONTRA_CLEARING_FIRM},
      {PartyRole::SponsoringFirm, FIX::PartyRole_SPONSORING_FIRM},
      {PartyRole::UnderlyingContraFirm, FIX::PartyRole_UNDERLYING_CONTRA_FIRM},
      {PartyRole::ClearingOrganization, FIX::PartyRole_CLEARING_ORGANIZATION},
      {PartyRole::Exchange, FIX::PartyRole_EXCHANGE},
      {PartyRole::CustomerAccount, FIX::PartyRole_CUSTOMER_ACCOUNT},
      {PartyRole::CorrespondentClearingOrganization, FIX::PartyRole_CORRESPONDENT_CLEARING_ORGANIZATION},
      {PartyRole::CorrespondentBroker, FIX::PartyRole_CORRESPONDENT_BROKER},
      {PartyRole::Custodian, FIX::PartyRole_CUSTODIAN},
      {PartyRole::Intermediary, FIX::PartyRole_INTERMEDIARY},
      {PartyRole::Agent, FIX::PartyRole_AGENT},
      {PartyRole::SubCustodian, FIX::PartyRole_SUB_CUSTODIAN},
      {PartyRole::Beneficiary, FIX::PartyRole_BENEFICIARY},
      {PartyRole::InterestedParty, FIX::PartyRole_INTERESTED_PARTY},
      {PartyRole::RegulatoryBody, FIX::PartyRole_REGULATORY_BODY},
      {PartyRole::LiquidityProvider, FIX::PartyRole_LIQUIDITY_PROVIDER},
      {PartyRole::EnteringTrader, FIX::PartyRole_ENTERING_TRADER},
      {PartyRole::ContraTrader, FIX::PartyRole_CONTRA_TRADER},
      {PartyRole::PositionAccount, FIX::PartyRole_POSITION_ACCOUNT},
      {PartyRole::ContraExchange, FIX::PartyRole_CONTRA_EXCHANGE},
      {PartyRole::InternalCarryAccount, FIX::PartyRole_INTERNAL_CARRY_ACCOUNT},
      {PartyRole::OrderEntryOperatorID, FIX::PartyRole_ORDER_ENTRY_OPERATOR_ID},
      {PartyRole::SecondaryAccountNumber, FIX::PartyRole_SECONDARY_ACCOUNT_NUMBER},
      {PartyRole::ForeignFirm, FIX::PartyRole_FOREIGN_FIRM},
      {PartyRole::ThirdPartyAllocationFirm, FIX::PartyRole_THIRD_PARTY_ALLOCATION_FIRM},
      {PartyRole::ClaimingAccount, FIX::PartyRole_CLAIMING_ACCOUNT},
      {PartyRole::AssetManager, FIX::PartyRole_ASSET_MANAGER},
      {PartyRole::PledgorAccount, FIX::PartyRole_PLEDGOR_ACCOUNT},
      {PartyRole::PledgeeAccount, FIX::PartyRole_PLEDGEE_ACCOUNT},
      {PartyRole::LargeTraderReportableAccount, FIX::PartyRole_LARGE_TRADER_REPORTABLE_ACCOUNT},
      {PartyRole::TraderMnemonic, FIX::PartyRole_TRADER_MNEMONIC},
      {PartyRole::SenderLocation, FIX::PartyRole_SENDER_LOCATION},
      {PartyRole::SessionID, FIX::PartyRole_SESSION_ID},
      {PartyRole::AcceptableCounterparty, FIX::PartyRole_ACCEPTABLE_COUNTERPARTY},
      {PartyRole::UnacceptableCounterparty, FIX::PartyRole_UNACCEPTABLE_COUNTERPARTY},
      {PartyRole::EnteringUnit, FIX::PartyRole_ENTERING_UNIT},
      {PartyRole::ExecutingUnit, FIX::PartyRole_EXECUTING_UNIT},
      {PartyRole::IntroducingBroker, FIX::PartyRole_INTRODUCING_BROKER},
      {PartyRole::QuoteOriginator, FIX::PartyRole_QUOTE_ORIGINATOR},
      {PartyRole::ReportOriginator, FIX::PartyRole_REPORT_ORIGINATOR},
      {PartyRole::SystematicInternaliser, FIX::PartyRole_SYSTEMATIC_INTERNALISER},
      {PartyRole::MultilateralTradingFacility, FIX::PartyRole_MULTILATERAL_TRADING_FACILITY},
      {PartyRole::RegulatedMarket, FIX::PartyRole_REGULATED_MARKET},
      {PartyRole::MarketMaker, FIX::PartyRole_MARKET_MAKER},
      {PartyRole::InvestmentFirm, FIX::PartyRole_INVESTMENT_FIRM},
      {PartyRole::HostCompetentAuthority, FIX::PartyRole_HOST_COMPETENT_AUTHORITY},
      {PartyRole::HomeCompetentAuthority, FIX::PartyRole_HOME_COMPETENT_AUTHORITY},
      {PartyRole::CompetentAuthorityLiquidity, FIX::PartyRole_COMPETENT_AUTHORITY_OF_THE_MOST_RELEVANT_MARKET_IN_TERMS_OF_LIQUIDITY},
      {PartyRole::CompetentAuthorityTransactionVenue, FIX::PartyRole_COMPETENT_AUTHORITY_OF_THE_TRANSACTION},
      {PartyRole::ReportingIntermediary, FIX::PartyRole_REPORTING_INTERMEDIARY},
      {PartyRole::ExecutionVenue, FIX::PartyRole_EXECUTION_VENUE},
      {PartyRole::MarketDataEntryOriginator, FIX::PartyRole_MARKET_DATA_ENTRY_ORIGINATOR},
      {PartyRole::LocationID, FIX::PartyRole_LOCATION_ID},
      {PartyRole::DeskID, FIX::PartyRole_DESK_ID},
      {PartyRole::MarketDataMarket, FIX::PartyRole_MARKET_DATA_MARKET},
      {PartyRole::AllocationEntity, FIX::PartyRole_ALLOCATION_ENTITY},
      {PartyRole::PrimeBroker, FIX::PartyRole_PRIME_BROKER_PROVIDING_GENERAL_TRADE_SERVICES},
      {PartyRole::StepOutFirm, FIX::PartyRole_STEP_OUT_FIRM},
      {PartyRole::BrokerClearingID, FIX::PartyRole_BROKERCLEARINGID},
      {PartyRole::CentralRegistrationDepository, FIX::PartyRole_CENTRAL_REGISTRATION_DEPOSITORY},
      {PartyRole::ClearingAccount, FIX::PartyRole_CLEARING_ACCOUNT},
      {PartyRole::AcceptableSettlingCounterparty, FIX::PartyRole_ACCEPTABLE_SETTLING_COUNTERPARTY},
      {PartyRole::UnacceptableSettlingCounterparty, FIX::PartyRole_UNACCEPTABLE_SETTLING_COUNTERPARTY}};
  // clang-format on
}

[[nodiscard]]
auto make_execution_type_association()
    -> EnumAssociationTable<core::enumerators::ExecutionType, char> {
  using core::enumerators::ExecutionType;

  // clang-format off
  return {
    {ExecutionType::OrderPlaced, FIX::ExecType_NEW},
    {ExecutionType::OrderModified, FIX::ExecType_REPLACED},
    {ExecutionType::OrderCancelled, FIX::ExecType_CANCELED},
    {ExecutionType::Rejected, FIX::ExecType_REJECTED},
    {ExecutionType::OrderTraded, FIX::ExecType_TRADE}};
  // clang-format on
}

[[nodiscard]]
auto make_md_reject_reason_association()
    -> EnumAssociationTable<core::enumerators::MdRejectReason, char> {
  using core::enumerators::MdRejectReason;

  // clang-format off
  return {
    {MdRejectReason::UnknownSymbol, FIX::MDReqRejReason_UNKNOWN_SYMBOL},
    {MdRejectReason::DuplicateMdReqId, FIX::MDReqRejReason_DUPLICATE_MDREQID}};
  // clang-format on
}

[[nodiscard]]
auto make_md_entry_type_association()
    -> EnumAssociationTable<core::enumerators::MdEntryType, char> {
  using core::enumerators::MdEntryType;

  // clang-format off
  return {
      {MdEntryType::Bid, FIX::MDEntryType_BID},
      {MdEntryType::Offer, FIX::MDEntryType_OFFER},
      {MdEntryType::Trade, FIX::MDEntryType_TRADE},
      {MdEntryType::LowPrice, FIX::MDEntryType_TRADING_SESSION_LOW_PRICE},
      {MdEntryType::MidPrice, FIX::MDEntryType_MID_PRICE},
      {MdEntryType::HighPrice, FIX::MDEntryType_TRADING_SESSION_HIGH_PRICE}};
  // clang-format on
}

[[nodiscard]]
auto make_market_entry_action_association()
    -> EnumAssociationTable<core::enumerators::MarketEntryAction, char> {
  using core::enumerators::MarketEntryAction;

  // clang-format off
  return {
    {MarketEntryAction::New, FIX::MDUpdateAction_NEW},
    {MarketEntryAction::Change, FIX::MDUpdateAction_CHANGE},
    {MarketEntryAction::Delete, FIX::MDUpdateAction_DELETE}};
  // clang-format on
}

[[nodiscard]]
auto make_trading_phase_association()
    -> EnumAssociationTable<core::enumerators::TradingPhase, std::string> {
  using core::enumerators::TradingPhase;

  // clang-format off
  return {
      {TradingPhase::Open, FIX::TradingSessionSubID_OPEN},
      {TradingPhase::Closed, FIX::TradingSessionSubID_CLOSED},
      {TradingPhase::PostTrading, FIX::TradingSessionSubID_POST_TRADING},
      {TradingPhase::OpeningAuction, FIX::TradingSessionSubID_OPENING_OR_OPENING_AUCTION},
      {TradingPhase::IntradayAuction, FIX::TradingSessionSubID_INTRADAY_AUCTION},
      {TradingPhase::ClosingAuction, FIX::TradingSessionSubID_CLOSING_OR_CLOSING_AUCTION}};
  // clang-format on
}

[[nodiscard]]
auto make_trading_status_association()
    -> EnumAssociationTable<core::enumerators::TradingStatus, int> {
  using core::enumerators::TradingStatus;

  // clang-format off
  return {
    {TradingStatus::Halt, FIX::SecurityTradingStatus_TRADING_HALT},
    {TradingStatus::Resume, FIX::SecurityTradingStatus_RESUME}};
  // clang-format on
}

[[nodiscard]]
auto make_rejected_message_type_association()
    -> EnumAssociationTable<core::enumerators::RejectedMessageType,
                            std::string> {
  using core::enumerators::RejectedMessageType;

  // clang-format off
  return {
    {RejectedMessageType::SecurityStatusRequest, FIX::MsgType_SecurityStatusRequest}};
  // clang-format on
}

[[nodiscard]]
auto make_business_reject_reason_association()
    -> EnumAssociationTable<core::enumerators::BusinessRejectReason, int> {
  using core::enumerators::BusinessRejectReason;

  // clang-format off
  return {
    {BusinessRejectReason::Other, FIX::BusinessRejectReason_OTHER},
    {BusinessRejectReason::UnknownId, FIX::BusinessRejectReason_UNKNOWN_ID},
    {BusinessRejectReason::UnknownSecurity, FIX::BusinessRejectReason_UNKNOWN_SECURITY}};
  // clang-format on
}

}  // namespace

auto ToFixConverter<FIX::OrdStatus>::convert(
    core::enumerators::OrderStatus value) -> char {
  static const auto association = make_order_status_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given OrderStatus value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::OrdType>::convert(core::enumerators::OrderType value)
    -> char {
  static const auto association = make_order_type_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given OrderType value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::Side>::convert(core::enumerators::Side value) -> char {
  static const auto association = make_side_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(fmt::format(
      "cannot convert given Side value '{}'", core::underlying_cast(value)));
}

auto ToFixConverter<FIX::AggressorSide>::convert(core::enumerators::Side value)
    -> char {
  static const auto association = make_side_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(fmt::format(
      "cannot convert given Side value '{}'", core::underlying_cast(value)));
}

auto ToFixConverter<FIX::TimeInForce>::convert(
    core::enumerators::TimeInForce value) -> char {
  static const auto association = make_time_in_force_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given TimeInForce value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::SecurityType>::convert(
    core::enumerators::SecurityType value) -> std::string {
  static const auto association = make_security_type_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given SecurityType value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::SecurityIDSource>::convert(
    core::enumerators::SecurityIdSource value) -> std::string {
  static const auto association = make_security_id_source_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given SecurityIdSource value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::PartyIDSource>::convert(
    core::enumerators::PartyIdentifierSource value) -> char {
  static const auto association = make_party_id_source_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given PartyIdentifierSource value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::InstrumentPartyIDSource>::convert(
    core::enumerators::PartyIdentifierSource value) -> char {
  static const auto association = make_party_id_source_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given PartyIdentifierSource value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::PartyRole>::convert(core::enumerators::PartyRole value)
    -> int {
  static const auto association = make_party_role_mapping_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given PartyRole value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::InstrumentPartyRole>::convert(
    core::enumerators::PartyRole value) -> int {
  static const auto association = make_party_role_mapping_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given PartyRole value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::ExecType>::convert(
    core::enumerators::ExecutionType value) -> char {
  static const auto association = make_execution_type_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given ExecutionType value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::MDReqRejReason>::convert(
    core::enumerators::MdRejectReason value) -> char {
  static const auto association = make_md_reject_reason_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given MdRejectReason value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::MDEntryType>::convert(
    core::enumerators::MdEntryType value) -> char {
  static const auto association = make_md_entry_type_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given MdEntryType value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::MDUpdateAction>::convert(
    core::enumerators::MarketEntryAction value) -> char {
  static const auto association = make_market_entry_action_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given MarketEntryAction value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::TradingSessionSubID>::convert(
    core::enumerators::TradingPhase value) -> std::string {
  static const auto association = make_trading_phase_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given TradingPhase value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::SecurityTradingStatus>::convert(
    core::enumerators::TradingStatus value) -> int {
  static const auto association = make_trading_status_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given TradingStatus value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::RefMsgType>::convert(
    core::enumerators::RejectedMessageType value) -> std::string {
  static const auto association = make_rejected_message_type_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given RejectedMessageType value '{}'",
                  core::underlying_cast(value)));
}

auto ToFixConverter<FIX::BusinessRejectReason>::convert(
    core::enumerators::BusinessRejectReason value) -> int {
  static const auto association = make_business_reject_reason_association();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  throw std::invalid_argument(
      fmt::format("cannot convert given BusinessRejectReason value '{}'",
                  core::underlying_cast(value)));
}

}  // namespace simulator::fix::detail