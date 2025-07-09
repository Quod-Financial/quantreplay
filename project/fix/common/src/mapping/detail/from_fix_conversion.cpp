#include "common/mapping/detail/from_fix_conversion.hpp"

#include <quickfix/FieldTypes.h>
#include <quickfix/Fields.h>
#include <quickfix/FixValues.h>

#include <chrono>
#include <string>
#include <string_view>
#include <unordered_map>

#include "common/custom_values.hpp"
#include "core/tools/string_checkers.hpp"

namespace simulator::fix::detail {

namespace {

template <typename KeyType, typename EnumType>
using EnumAssociationTable = std::unordered_map<KeyType, EnumType>;

[[nodiscard]]
auto make_order_type_mapping_association()
    -> EnumAssociationTable<char, OrderType::Option> {
  // clang-format off
  return {
    {FIX::OrdType_LIMIT, OrderType::Option::Limit},
    {FIX::OrdType_MARKET, OrderType::Option::Market}};
  // clang-format on
}

[[nodiscard]]
auto make_side_mapping_association()
    -> EnumAssociationTable<char, Side::Option> {
  // clang-format off
  return {
    {FIX::Side_BUY, Side::Option::Buy},
    {FIX::Side_SELL, Side::Option::Sell},
    {FIX::Side_SELL_SHORT, Side::Option::SellShort},
    {FIX::Side_SELL_SHORT_EXEMPT, Side::Option::SellShortExempt}};
  // clang-format on
}

[[nodiscard]]
auto make_time_in_force_mapping_association()
    -> EnumAssociationTable<char, TimeInForce::Option> {
  // clang-format off
  return {
      {FIX::TimeInForce_DAY, TimeInForce::Option::Day},
      {FIX::TimeInForce_IMMEDIATE_OR_CANCEL, TimeInForce::Option::ImmediateOrCancel},
      {FIX::TimeInForce_FILL_OR_KILL, TimeInForce::Option::FillOrKill},
      {FIX::TimeInForce_GOOD_TILL_DATE, TimeInForce::Option::GoodTillDate},
      {FIX::TimeInForce_GOOD_TILL_CANCEL, TimeInForce::Option::GoodTillCancel}};
  // clang-format on
}

auto make_security_type_mapping_association()
    -> EnumAssociationTable<std::string_view, SecurityType::Option> {
  // clang-format off
  return {
      {FIX::SecurityType_COMMON_STOCK, SecurityType::Option::CommonStock},
      {FIX::SecurityType_FUTURE, SecurityType::Option::Future},
      {FIX::SecurityType_OPTION, SecurityType::Option::Option},
      {FIX::SecurityType_MULTILEG_INSTRUMENT, SecurityType::Option::MultiLeg},
      {FIX::SecurityType_SYNTHETIC_MULTILEG_INSTRUMENT, SecurityType::Option::SyntheticMultiLeg},
      {FIX::SecurityType_WARRANT, SecurityType::Option::Warrant},
      {FIX::SecurityType_MUTUAL_FUND, SecurityType::Option::MutualFund},
      {FIX::SecurityType_CORPORATE_BOND, SecurityType::Option::CorporateBond},
      {FIX::SecurityType_CONVERTIBLE_BOND, SecurityType::Option::ConvertibleBond},
      {FIX::SecurityType_REPURCHASE_AGREEMENT, SecurityType::Option::RepurchaseAgreement},
      {FIX::SecurityType_INDEX, SecurityType::Option::Index},
      {FIX::SecurityType_CONTRACT_FOR_DIFFERENCE, SecurityType::Option::ContractForDifference},
      {FIX::SecurityType_CERTIFICATE_OF_DEPOSIT, SecurityType::Option::Certificate},
      {FIX::SecurityType_FX_SPOT, SecurityType::Option::FxSpot},
      {FIX::SecurityType_FORWARD, SecurityType::Option::Forward},
      {FIX::SecurityType_FX_FORWARD, SecurityType::Option::FxForward},
      {FIX::SecurityType_NON_DELIVERABLE_FORWARD, SecurityType::Option::FxNonDeliverableForward},
      {FIX::SecurityType_FX_SWAP, SecurityType::Option::FxSwap},
      {FIX::SecurityType_NON_DELIVERABLE_SWAP, SecurityType::Option::FxNonDeliverableSwap}};
  // clang-format on
}

[[nodiscard]]
auto make_security_id_source_mapping_association()
    -> EnumAssociationTable<std::string_view, SecurityIdSource::Option> {
  // clang-format off
  return {
      {FIX::SecurityIDSource_CUSIP, SecurityIdSource::Option::Cusip},
      {FIX::SecurityIDSource_SEDOL, SecurityIdSource::Option::Sedol},
      {FIX::SecurityIDSource_ISIN_NUMBER, SecurityIdSource::Option::Isin},
      {FIX::SecurityIDSource_RIC_CODE, SecurityIdSource::Option::Ric},
      {FIX::SecurityIDSource_EXCHANGE_SYMBOL, SecurityIdSource::Option::ExchangeSymbol},
      {FIX::SecurityIDSource_BLOOMBERG_SYMBOL, SecurityIdSource::Option::BloombergSymbol}};
  // clang-format on
}

[[nodiscard]]
auto make_party_id_source_mapping_association()
    -> EnumAssociationTable<char, PartyIdSource::Option> {
  // clang-format off
  return {
      {FIX::PartyIDSource_UK_NATIONAL_INSURANCE_OR_PENSION_NUMBER, PartyIdSource::Option::UKNationalInsuranceOrPensionNumber},
      {FIX::PartyIDSource_US_SOCIAL_SECURITY_NUMBER, PartyIdSource::Option::USSocialSecurityNumber},
      {FIX::PartyIDSource_US_EMPLOYER_OR_TAX_ID_NUMBER, PartyIdSource::Option::USEmployerOrTaxIDNumber},
      {FIX::PartyIDSource_AUSTRALIAN_BUSINESS_NUMBER, PartyIdSource::Option::AustralianBusinessNumber},
      {FIX::PartyIDSource_AUSTRALIAN_TAX_FILE_NUMBER, PartyIdSource::Option::AustralianTaxFileNumber},
      {FIX::PartyIDSource_TAX_ID, PartyIdSource::Option::TaxID},
      {FIX::PartyIDSource_KOREAN_INVESTOR_ID, PartyIdSource::Option::KoreanInvestorID},
      {FIX::PartyIDSource_TAIWANESE_QUALIFIED_FOREIGN_INVESTOR_ID_QFII_FID, PartyIdSource::Option::TaiwaneseForeignInvestorID},
      {FIX::PartyIDSource_TAIWANESE_TRADING_ACCOUNT, PartyIdSource::Option::TaiwaneseTradingAcct},
      {FIX::PartyIDSource_MALAYSIAN_CENTRAL_DEPOSITORY, PartyIdSource::Option::MalaysianCentralDepository},
      {FIX::PartyIDSource_CHINESE_INVESTOR_ID, PartyIdSource::Option::ChineseInvestorID},
      {FIX::PartyIDSource_DIRECTED_BROKER_THREE_CHARACTER_ACRONYM_AS_DEFINED_IN_ISITC_ETC_BEST_PRACTICE_GUIDELINES_DOCUMENT, PartyIdSource::Option::ISITCAcronym},
      {FIX::PartyIDSource_BIC, PartyIdSource::Option::BIC},
      {FIX::PartyIDSource_GENERALLY_ACCEPTED_MARKET_PARTICIPANT_IDENTIFIER, PartyIdSource::Option::GeneralIdentifier},
      {FIX::PartyIDSource_PROPRIETARY, PartyIdSource::Option::Proprietary},
      {FIX::PartyIDSource_ISO_COUNTRY_CODE, PartyIdSource::Option::ISOCountryCode},
      {FIX::PartyIDSource_SETTLEMENT_ENTITY_LOCATION, PartyIdSource::Option::SettlementEntityLocation},
      {FIX::PartyIDSource_MIC, PartyIdSource::Option::MIC},
      {FIX::PartyIDSource_CSD_PARTICIPANT_MEMBER_CODE, PartyIdSource::Option::CSDParticipant},
      {FIX::PartyIDSource_AUSTRALIAN_COMPANY_NUMBER, PartyIdSource::Option::AustralianCompanyNumber},
      {FIX::PartyIDSource_AUSTRALIAN_REGISTERED_BODY_NUMBER, PartyIdSource::Option::AustralianRegisteredBodyNumber},
      {FIX::PartyIDSource_CFTC_REPORTING_FIRM_IDENTIFIER, PartyIdSource::Option::CFTCReportingFirmIdentifier},
      {FIX::PartyIDSource_LEI, PartyIdSource::Option::LegalEntityIdentifier},
      {FIX::PartyIDSource_INTERIM_IDENTIFIER, PartyIdSource::Option::InterimIdentifier},
      {FIX::PartyIDSource_SHORT_CODE_IDENTIFIER, PartyIdSource::Option::ShortCodeIdentifier},
      {FIX::PartyIDSource_NATIONAL_ID_NATURAL_PERSON, PartyIdSource::Option::NationalIDNaturalPerson},
      {FIX::PartyIDSource_INDIA_PERMANENT_ACCOUNT_NUMBER, PartyIdSource::Option::IndiaPermanentAccountNumber},
      {FIX::PartyIDSource_FDID, PartyIdSource::Option::FDID},
      {FIX::PartyIDSource_SPASAID, PartyIdSource::Option::SPSAID},
      {FIX::PartyIDSource_MASTER_SPASAID, PartyIdSource::Option::MasterSPSAID}};
  // clang-format on
}

[[nodiscard]]
auto make_party_role_mapping_association()
    -> EnumAssociationTable<int, PartyRole::Option> {
  // clang-format off
  return {
      {FIX::PartyRole_EXECUTING_FIRM, PartyRole::Option::ExecutingFirm},
      {FIX::PartyRole_BROKER_OF_CREDIT, PartyRole::Option::BrokerOfCredit},
      {FIX::PartyRole_CLIENT_ID, PartyRole::Option::ClientID},
      {FIX::PartyRole_CLEARING_FIRM, PartyRole::Option::ClearingFirm},
      {FIX::PartyRole_INVESTOR_ID, PartyRole::Option::InvestorID},
      {FIX::PartyRole_INTRODUCING_FIRM, PartyRole::Option::IntroducingFirm},
      {FIX::PartyRole_ENTERING_FIRM, PartyRole::Option::EnteringFirm},
      {FIX::PartyRole_LOCATE, PartyRole::Option::Locate},
      {FIX::PartyRole_FUND_MANAGER_CLIENT_ID, PartyRole::Option::FundManagerClientID},
      {FIX::PartyRole_SETTLEMENT_LOCATION, PartyRole::Option::SettlementLocation},
      {FIX::PartyRole_ORDER_ORIGINATION_TRADER, PartyRole::Option::OrderOriginationTrader},
      {FIX::PartyRole_EXECUTING_TRADER, PartyRole::Option::ExecutingTrader},
      {FIX::PartyRole_ORDER_ORIGINATION_FIRM, PartyRole::Option::OrderOriginationFirm},
      {FIX::PartyRole_GIVEUP_CLEARING_FIRM, PartyRole::Option::GiveupClearingFirm},
      {FIX::PartyRole_CORRESPONDANT_CLEARING_FIRM, PartyRole::Option::CorrespondantClearingFirm},
      {FIX::PartyRole_EXECUTING_SYSTEM, PartyRole::Option::ExecutingSystem},
      {FIX::PartyRole_CONTRA_FIRM, PartyRole::Option::ContraFirm},
      {FIX::PartyRole_CONTRA_CLEARING_FIRM, PartyRole::Option::ContraClearingFirm},
      {FIX::PartyRole_SPONSORING_FIRM, PartyRole::Option::SponsoringFirm},
      {FIX::PartyRole_UNDERLYING_CONTRA_FIRM, PartyRole::Option::UnderlyingContraFirm},
      {FIX::PartyRole_CLEARING_ORGANIZATION, PartyRole::Option::ClearingOrganization},
      {FIX::PartyRole_EXCHANGE, PartyRole::Option::Exchange},
      {FIX::PartyRole_CUSTOMER_ACCOUNT, PartyRole::Option::CustomerAccount},
      {FIX::PartyRole_CORRESPONDENT_CLEARING_ORGANIZATION, PartyRole::Option::CorrespondentClearingOrganization},
      {FIX::PartyRole_CORRESPONDENT_BROKER, PartyRole::Option::CorrespondentBroker},
      {FIX::PartyRole_CUSTODIAN, PartyRole::Option::Custodian},
      {FIX::PartyRole_INTERMEDIARY, PartyRole::Option::Intermediary},
      {FIX::PartyRole_AGENT, PartyRole::Option::Agent},
      {FIX::PartyRole_SUB_CUSTODIAN, PartyRole::Option::SubCustodian},
      {FIX::PartyRole_BENEFICIARY, PartyRole::Option::Beneficiary},
      {FIX::PartyRole_INTERESTED_PARTY, PartyRole::Option::InterestedParty},
      {FIX::PartyRole_REGULATORY_BODY, PartyRole::Option::RegulatoryBody},
      {FIX::PartyRole_LIQUIDITY_PROVIDER, PartyRole::Option::LiquidityProvider},
      {FIX::PartyRole_ENTERING_TRADER, PartyRole::Option::EnteringTrader},
      {FIX::PartyRole_CONTRA_TRADER, PartyRole::Option::ContraTrader},
      {FIX::PartyRole_POSITION_ACCOUNT, PartyRole::Option::PositionAccount},
      {FIX::PartyRole_CONTRA_EXCHANGE, PartyRole::Option::ContraExchange},
      {FIX::PartyRole_INTERNAL_CARRY_ACCOUNT, PartyRole::Option::InternalCarryAccount},
      {FIX::PartyRole_ORDER_ENTRY_OPERATOR_ID, PartyRole::Option::OrderEntryOperatorID},
      {FIX::PartyRole_SECONDARY_ACCOUNT_NUMBER, PartyRole::Option::SecondaryAccountNumber},
      {FIX::PartyRole_FOREIGN_FIRM, PartyRole::Option::ForeignFirm},
      {FIX::PartyRole_THIRD_PARTY_ALLOCATION_FIRM, PartyRole::Option::ThirdPartyAllocationFirm},
      {FIX::PartyRole_CLAIMING_ACCOUNT, PartyRole::Option::ClaimingAccount},
      {FIX::PartyRole_ASSET_MANAGER, PartyRole::Option::AssetManager},
      {FIX::PartyRole_PLEDGOR_ACCOUNT, PartyRole::Option::PledgorAccount},
      {FIX::PartyRole_PLEDGEE_ACCOUNT, PartyRole::Option::PledgeeAccount},
      {FIX::PartyRole_LARGE_TRADER_REPORTABLE_ACCOUNT, PartyRole::Option::LargeTraderReportableAccount},
      {FIX::PartyRole_TRADER_MNEMONIC, PartyRole::Option::TraderMnemonic},
      {FIX::PartyRole_SENDER_LOCATION, PartyRole::Option::SenderLocation},
      {FIX::PartyRole_SESSION_ID, PartyRole::Option::SessionID},
      {FIX::PartyRole_ACCEPTABLE_COUNTERPARTY, PartyRole::Option::AcceptableCounterparty},
      {FIX::PartyRole_UNACCEPTABLE_COUNTERPARTY, PartyRole::Option::UnacceptableCounterparty},
      {FIX::PartyRole_ENTERING_UNIT, PartyRole::Option::EnteringUnit},
      {FIX::PartyRole_EXECUTING_UNIT, PartyRole::Option::ExecutingUnit},
      {FIX::PartyRole_INTRODUCING_BROKER, PartyRole::Option::IntroducingBroker},
      {FIX::PartyRole_QUOTE_ORIGINATOR, PartyRole::Option::QuoteOriginator},
      {FIX::PartyRole_REPORT_ORIGINATOR, PartyRole::Option::ReportOriginator},
      {FIX::PartyRole_SYSTEMATIC_INTERNALISER, PartyRole::Option::SystematicInternaliser},
      {FIX::PartyRole_MULTILATERAL_TRADING_FACILITY, PartyRole::Option::MultilateralTradingFacility},
      {FIX::PartyRole_REGULATED_MARKET, PartyRole::Option::RegulatedMarket},
      {FIX::PartyRole_MARKET_MAKER, PartyRole::Option::MarketMaker},
      {FIX::PartyRole_INVESTMENT_FIRM, PartyRole::Option::InvestmentFirm},
      {FIX::PartyRole_HOST_COMPETENT_AUTHORITY, PartyRole::Option::HostCompetentAuthority},
      {FIX::PartyRole_HOME_COMPETENT_AUTHORITY, PartyRole::Option::HomeCompetentAuthority},
      {FIX::PartyRole_COMPETENT_AUTHORITY_OF_THE_MOST_RELEVANT_MARKET_IN_TERMS_OF_LIQUIDITY, PartyRole::Option::CompetentAuthorityLiquidity},
      {FIX::PartyRole_COMPETENT_AUTHORITY_OF_THE_TRANSACTION, PartyRole::Option::CompetentAuthorityTransactionVenue},
      {FIX::PartyRole_REPORTING_INTERMEDIARY, PartyRole::Option::ReportingIntermediary},
      {FIX::PartyRole_EXECUTION_VENUE, PartyRole::Option::ExecutionVenue},
      {FIX::PartyRole_MARKET_DATA_ENTRY_ORIGINATOR, PartyRole::Option::MarketDataEntryOriginator},
      {FIX::PartyRole_LOCATION_ID, PartyRole::Option::LocationID},
      {FIX::PartyRole_DESK_ID, PartyRole::Option::DeskID},
      {FIX::PartyRole_MARKET_DATA_MARKET, PartyRole::Option::MarketDataMarket},
      {FIX::PartyRole_ALLOCATION_ENTITY, PartyRole::Option::AllocationEntity},
      {FIX::PartyRole_PRIME_BROKER_PROVIDING_GENERAL_TRADE_SERVICES, PartyRole::Option::PrimeBroker},
      {FIX::PartyRole_STEP_OUT_FIRM, PartyRole::Option::StepOutFirm},
      {FIX::PartyRole_BROKERCLEARINGID, PartyRole::Option::BrokerClearingID},
      {FIX::PartyRole_CENTRAL_REGISTRATION_DEPOSITORY, PartyRole::Option::CentralRegistrationDepository},
      {FIX::PartyRole_CLEARING_ACCOUNT, PartyRole::Option::ClearingAccount},
      {FIX::PartyRole_ACCEPTABLE_SETTLING_COUNTERPARTY, PartyRole::Option::AcceptableSettlingCounterparty},
      {FIX::PartyRole_UNACCEPTABLE_SETTLING_COUNTERPARTY, PartyRole::Option::UnacceptableSettlingCounterparty}};
  // clang-format on
}

[[nodiscard]]
auto make_md_entry_type_association()
    -> EnumAssociationTable<char, MdEntryType::Option> {
  // clang-format off
  return {
      {FIX::MDEntryType_BID, MdEntryType::Option::Bid},
      {FIX::MDEntryType_OFFER, MdEntryType::Option::Offer},
      {FIX::MDEntryType_TRADE, MdEntryType::Option::Trade},
      {FIX::MDEntryType_TRADING_SESSION_LOW_PRICE, MdEntryType::Option::LowPrice},
      {FIX::MDEntryType_MID_PRICE, MdEntryType::Option::MidPrice},
      {FIX::MDEntryType_TRADING_SESSION_HIGH_PRICE, MdEntryType::Option::HighPrice}};
  // clang-format on
}

[[nodiscard]]
auto make_market_data_update_type_association()
    -> EnumAssociationTable<int, MarketDataUpdateType::Option> {
  // clang-format off
  return {
        {FIX::MDUpdateType_FULL_REFRESH, MarketDataUpdateType::Option::Snapshot},
        {FIX::MDUpdateType_INCREMENTAL_REFRESH, MarketDataUpdateType::Option::Incremental}};
  // clang-format on
}

[[nodiscard]]
auto make_md_subscription_request_type_association()
    -> EnumAssociationTable<char, MdSubscriptionRequestType::Option> {
  // clang-format off
return {
    {FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES, MdSubscriptionRequestType::Option::Subscribe},
    {FIX::SubscriptionRequestType_DISABLE_PREVIOUS_SNAPSHOT_PLUS_UPDATE_REQUEST, MdSubscriptionRequestType::Option::Unsubscribe},
    {FIX::SubscriptionRequestType_SNAPSHOT, MdSubscriptionRequestType::Option::Snapshot}};
  // clang-format on
}

}  // namespace

auto FromFixConverter<std::chrono::sys_time<std::chrono::microseconds>>::
    convert(const FIX::UtcTimeStampField& field)
        -> std::chrono::sys_time<std::chrono::microseconds> {
  const FIX::UtcTimeStamp& timestamp = field.getValue();

  const std::chrono::year_month_day ymd{
      std::chrono::year{timestamp.getYear()},
      std::chrono::month{static_cast<unsigned>(timestamp.getMonth())},
      std::chrono::day{static_cast<unsigned>(timestamp.getDay())}};

  return std::chrono::sys_days{ymd} + std::chrono::hours{timestamp.getHour()} +
         std::chrono::minutes{timestamp.getMinute()} +
         std::chrono::seconds{timestamp.getSecond()} +
         std::chrono::microseconds{timestamp.getMicroecond()};
}

auto FromFixConverter<std::chrono::local_days>::convert(
    const FIX::LocalMktDateField& field) -> std::chrono::local_days {
  constexpr std::size_t FixLocalDateLen = 8;
  const std::string& fix_local_date = field.getValue();

  if (fix_local_date.size() != FixLocalDateLen ||
      !core::represents_base10_unsigned_int(fix_local_date)) {
    detail::from_fix_error::field_has_bad_value(field);
  }

  // std::stoi must not throw, because we've already checked
  // that the entire string represents a number
  const auto year = std::stoi(fix_local_date.substr(0, 4));
  const auto month = std::stoi(fix_local_date.substr(4, 2));
  const auto day = std::stoi(fix_local_date.substr(6, 2));

  return std::chrono::local_days{
      std::chrono::year{year} /
      std::chrono::month{static_cast<unsigned>(month)} /
      std::chrono::day{static_cast<unsigned>(day)}};
}

auto FromFixConverter<OrderType::Option>::convert(const FIX::OrdType& field)
    -> OrderType::Option {
  const static auto association = make_order_type_mapping_association();

  const char value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<Side::Option>::convert(const FIX::Side& field)
    -> Side::Option {
  const static auto association = make_side_mapping_association();

  const char value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<TimeInForce::Option>::convert(
    const FIX::TimeInForce& field) -> TimeInForce::Option {
  const static auto association = make_time_in_force_mapping_association();

  const char value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<SecurityType::Option>::convert(
    const FIX::SecurityType& field) -> SecurityType::Option {
  const static auto association = make_security_type_mapping_association();

  const std::string& value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<SecurityIdSource::Option>::convert(
    const FIX::SecurityIDSource& field) -> SecurityIdSource::Option {
  const static auto association = make_security_id_source_mapping_association();

  const std::string& value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<PartyIdSource::Option>::convert(
    const FIX::PartyIDSource& field) -> PartyIdSource::Option {
  const static auto association = make_party_id_source_mapping_association();

  const char value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<PartyIdSource::Option>::convert(
    const FIX::InstrumentPartyIDSource& field) -> PartyIdSource::Option {
  const static auto association = make_party_id_source_mapping_association();

  const char value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<PartyRole::Option>::convert(const FIX::PartyRole& field)
    -> PartyRole::Option {
  const static auto association = make_party_role_mapping_association();

  const int value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<PartyRole::Option>::convert(
    const FIX::InstrumentPartyRole& field) -> PartyRole::Option {
  const static auto association = make_party_role_mapping_association();

  const int value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<MdEntryType::Option>::convert(
    const FIX::MDEntryType& field) -> MdEntryType::Option {
  const static auto association = make_md_entry_type_association();

  const char value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<MarketDataUpdateType::Option>::convert(
    const FIX::MDUpdateType& field) -> MarketDataUpdateType::Option {
  const static auto association = make_market_data_update_type_association();

  const int value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<MdSubscriptionRequestType::Option>::convert(
    const FIX::SubscriptionRequestType& field)
    -> MdSubscriptionRequestType::Option {
  const static auto association =
      make_md_subscription_request_type_association();

  const char value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  detail::from_fix_error::field_has_bad_value(field);
}

}  // namespace simulator::fix::detail