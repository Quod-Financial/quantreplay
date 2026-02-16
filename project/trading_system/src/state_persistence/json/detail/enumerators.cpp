#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"

namespace simulator::trading_system::json {

namespace {

template <typename E>
  requires std::is_enum_v<E>
struct BiMap {
  constexpr BiMap(
      const std::initializer_list<std::pair<E, std::string_view>>& data) {
    for (auto& pair : data) {
      value_to_str[pair.first] = pair.second;
      str_to_value[pair.second] = pair.first;
    }
  }

  std::unordered_map<E, std::string_view> value_to_str;
  std::unordered_map<std::string_view, E> str_to_value;
};

const BiMap<PartyRole::Option> party_role_bimap{
    {PartyRole::Option::ExecutingFirm, "ExecutingFirm"},
    {PartyRole::Option::BrokerOfCredit, "BrokerOfCredit"},
    {PartyRole::Option::ClientID, "ClientID"},
    {PartyRole::Option::ClearingFirm, "ClearingFirm"},
    {PartyRole::Option::InvestorID, "InvestorID"},
    {PartyRole::Option::IntroducingFirm, "IntroducingFirm"},
    {PartyRole::Option::EnteringFirm, "EnteringFirm"},
    {PartyRole::Option::Locate, "Locate"},
    {PartyRole::Option::FundManagerClientID, "FundManagerClientID"},
    {PartyRole::Option::SettlementLocation, "SettlementLocation"},
    {PartyRole::Option::OrderOriginationTrader, "OrderOriginationTrader"},
    {PartyRole::Option::ExecutingTrader, "ExecutingTrader"},
    {PartyRole::Option::OrderOriginationFirm, "OrderOriginationFirm"},
    {PartyRole::Option::GiveupClearingFirmDepr, "GiveupClearingFirmDepr"},
    {PartyRole::Option::CorrespondantClearingFirm, "CorrespondantClearingFirm"},
    {PartyRole::Option::ExecutingSystem, "ExecutingSystem"},
    {PartyRole::Option::ContraFirm, "ContraFirm"},
    {PartyRole::Option::ContraClearingFirm, "ContraClearingFirm"},
    {PartyRole::Option::SponsoringFirm, "SponsoringFirm"},
    {PartyRole::Option::UnderlyingContraFirm, "UnderlyingContraFirm"},
    {PartyRole::Option::ClearingOrganization, "ClearingOrganization"},
    {PartyRole::Option::Exchange, "Exchange"},
    {PartyRole::Option::CustomerAccount, "CustomerAccount"},
    {PartyRole::Option::CorrespondentClearingOrganization,
     "CorrespondentClearingOrganization"},
    {PartyRole::Option::CorrespondentBroker, "CorrespondentBroker"},
    {PartyRole::Option::Buyer, "Buyer"},
    {PartyRole::Option::Custodian, "Custodian"},
    {PartyRole::Option::Intermediary, "Intermediary"},
    {PartyRole::Option::Agent, "Agent"},
    {PartyRole::Option::SubCustodian, "SubCustodian"},
    {PartyRole::Option::Beneficiary, "Beneficiary"},
    {PartyRole::Option::InterestedParty, "InterestedParty"},
    {PartyRole::Option::RegulatoryBody, "RegulatoryBody"},
    {PartyRole::Option::LiquidityProvider, "LiquidityProvider"},
    {PartyRole::Option::EnteringTrader, "EnteringTrader"},
    {PartyRole::Option::ContraTrader, "ContraTrader"},
    {PartyRole::Option::PositionAccount, "PositionAccount"},
    {PartyRole::Option::ContraInvestorID, "ContraInvestorID"},
    {PartyRole::Option::TransferToFirm, "TransferToFirm"},
    {PartyRole::Option::ContraPositionAccount, "ContraPositionAccount"},
    {PartyRole::Option::ContraExchange, "ContraExchange"},
    {PartyRole::Option::InternalCarryAccount, "InternalCarryAccount"},
    {PartyRole::Option::OrderEntryOperatorID, "OrderEntryOperatorID"},
    {PartyRole::Option::SecondaryAccountNumber, "SecondaryAccountNumber"},
    {PartyRole::Option::ForeignFirm, "ForeignFirm"},
    {PartyRole::Option::ThirdPartyAllocationFirm, "ThirdPartyAllocationFirm"},
    {PartyRole::Option::ClaimingAccount, "ClaimingAccount"},
    {PartyRole::Option::AssetManager, "AssetManager"},
    {PartyRole::Option::PledgorAccount, "PledgorAccount"},
    {PartyRole::Option::PledgeeAccount, "PledgeeAccount"},
    {PartyRole::Option::LargeTraderReportableAccount,
     "LargeTraderReportableAccount"},
    {PartyRole::Option::TraderMnemonic, "TraderMnemonic"},
    {PartyRole::Option::SenderLocation, "SenderLocation"},
    {PartyRole::Option::SessionID, "SessionID"},
    {PartyRole::Option::AcceptableCounterparty, "AcceptableCounterparty"},
    {PartyRole::Option::UnacceptableCounterparty, "UnacceptableCounterparty"},
    {PartyRole::Option::EnteringUnit, "EnteringUnit"},
    {PartyRole::Option::ExecutingUnit, "ExecutingUnit"},
    {PartyRole::Option::IntroducingBroker, "IntroducingBroker"},
    {PartyRole::Option::QuoteOriginator, "QuoteOriginator"},
    {PartyRole::Option::ReportOriginator, "ReportOriginator"},
    {PartyRole::Option::SystematicInternaliser, "SystematicInternaliser"},
    {PartyRole::Option::MultilateralTradingFacility,
     "MultilateralTradingFacility"},
    {PartyRole::Option::RegulatedMarket, "RegulatedMarket"},
    {PartyRole::Option::MarketMaker, "MarketMaker"},
    {PartyRole::Option::InvestmentFirm, "InvestmentFirm"},
    {PartyRole::Option::HostCompetentAuthority, "HostCompetentAuthority"},
    {PartyRole::Option::HomeCompetentAuthority, "HomeCompetentAuthority"},
    {PartyRole::Option::CompetentAuthorityLiquidity,
     "CompetentAuthorityLiquidity"},
    {PartyRole::Option::CompetentAuthorityTransactionVenue,
     "CompetentAuthorityTransactionVenue"},
    {PartyRole::Option::ReportingIntermediary, "ReportingIntermediary"},
    {PartyRole::Option::ExecutionVenue, "ExecutionVenue"},
    {PartyRole::Option::MarketDataEntryOriginator, "MarketDataEntryOriginator"},
    {PartyRole::Option::LocationID, "LocationID"},
    {PartyRole::Option::DeskID, "DeskID"},
    {PartyRole::Option::MarketDataMarket, "MarketDataMarket"},
    {PartyRole::Option::AllocationEntity, "AllocationEntity"},
    {PartyRole::Option::PrimeBroker, "PrimeBroker"},
    {PartyRole::Option::StepOutFirm, "StepOutFirm"},
    {PartyRole::Option::BrokerClearingID, "BrokerClearingID"},
    {PartyRole::Option::CentralRegistrationDepository,
     "CentralRegistrationDepository"},
    {PartyRole::Option::ClearingAccount, "ClearingAccount"},
    {PartyRole::Option::AcceptableSettlingCounterparty,
     "AcceptableSettlingCounterparty"},
    {PartyRole::Option::UnacceptableSettlingCounterparty,
     "UnacceptableSettlingCounterparty"},
    {PartyRole::Option::CLSMemberBank, "CLSMemberBank"},
    {PartyRole::Option::InConcertGroup, "InConcertGroup"},
    {PartyRole::Option::InConcertControllingEntity,
     "InConcertControllingEntity"},
    {PartyRole::Option::LargePositionsReportingAccount,
     "LargePositionsReportingAccount"},
    {PartyRole::Option::SettlementFirm, "SettlementFirm"},
    {PartyRole::Option::SettlementAccount, "SettlementAccount"},
    {PartyRole::Option::ReportingMarketCenter, "ReportingMarketCenter"},
    {PartyRole::Option::RelatedReportingMarketCenter,
     "RelatedReportingMarketCenter"},
    {PartyRole::Option::AwayMarket, "AwayMarket"},
    {PartyRole::Option::GiveupTradingFirm, "GiveupTradingFirm"},
    {PartyRole::Option::TakeupTradingFirm, "TakeupTradingFirm"},
    {PartyRole::Option::GiveupClearingFirm, "GiveupClearingFirm"},
    {PartyRole::Option::TakeupClearingFirm, "TakeupClearingFirm"},
    {PartyRole::Option::OriginatingMarket, "OriginatingMarket"},
    {PartyRole::Option::MarginAccount, "MarginAccount"},
    {PartyRole::Option::CollateralAssetAccount, "CollateralAssetAccount"},
    {PartyRole::Option::DataRepository, "DataRepository"},
    {PartyRole::Option::CalculationAgent, "CalculationAgent"},
    {PartyRole::Option::ExerciseNoticeSender, "ExerciseNoticeSender"},
    {PartyRole::Option::ExerciseNoticeReceiver, "ExerciseNoticeReceiver"},
    {PartyRole::Option::RateReferenceBank, "RateReferenceBank"},
    {PartyRole::Option::Correspondent, "Correspondent"},
    {PartyRole::Option::BeneficiaryBank, "BeneficiaryBank"},
    {PartyRole::Option::Borrower, "Borrower"},
    {PartyRole::Option::PrimaryObligator, "PrimaryObligator"},
    {PartyRole::Option::Guarantor, "Guarantor"},
    {PartyRole::Option::ExcludedReferenceEntity, "ExcludedReferenceEntity"},
    {PartyRole::Option::DeterminingParty, "DeterminingParty"},
    {PartyRole::Option::HedgingParty, "HedgingParty"},
    {PartyRole::Option::ReportingEntity, "ReportingEntity"},
    {PartyRole::Option::SalesPerson, "SalesPerson"},
    {PartyRole::Option::Operator, "Operator"},
    {PartyRole::Option::CSD, "CSD"},
    {PartyRole::Option::ICSD, "ICSD"},
    {PartyRole::Option::TradingSubAccount, "TradingSubAccount"},
    {PartyRole::Option::InvestmentDecisionMaker, "InvestmentDecisionMaker"},
    {PartyRole::Option::PublishingIntermediary, "PublishingIntermediary"},
    {PartyRole::Option::CSDParticipant, "CSDParticipant"},
    {PartyRole::Option::Issuer, "Issuer"},
    {PartyRole::Option::ContraCustomerAccount, "ContraCustomerAccount"},
    {PartyRole::Option::ContraInvestmentDecisionMaker,
     "ContraInvestmentDecisionMaker"},
    {PartyRole::Option::AuthorizingPerson, "AuthorizingPerson"},
    {PartyRole::Option::PrimaryPlaceOfListing, "PrimaryPlaceOfListing"},
    {PartyRole::Option::SecondaryPlaceOfListing, "SecondaryPlaceOfListing"}};

const BiMap<PartyIdSource::Option> party_id_source_bimap{
    {{PartyIdSource::Option::UKNationalInsuranceOrPensionNumber,
      "UKNationalInsuranceOrPensionNumber"},
     {PartyIdSource::Option::USSocialSecurityNumber, "USSocialSecurityNumber"},
     {PartyIdSource::Option::USEmployerOrTaxIDNumber,
      "USEmployerOrTaxIDNumber"},
     {PartyIdSource::Option::AustralianBusinessNumber,
      "AustralianBusinessNumber"},
     {PartyIdSource::Option::AustralianTaxFileNumber,
      "AustralianTaxFileNumber"},
     {PartyIdSource::Option::TaxID, "TaxID"},
     {PartyIdSource::Option::KoreanInvestorID, "KoreanInvestorID"},
     {PartyIdSource::Option::TaiwaneseForeignInvestorID,
      "TaiwaneseForeignInvestorID"},
     {PartyIdSource::Option::TaiwaneseTradingAcct, "TaiwaneseTradingAcct"},
     {PartyIdSource::Option::MalaysianCentralDepository,
      "MalaysianCentralDepository"},
     {PartyIdSource::Option::ChineseInvestorID, "ChineseInvestorID"},
     {PartyIdSource::Option::ISITCAcronym, "ISITCAcronym"},
     {PartyIdSource::Option::BIC, "BIC"},
     {PartyIdSource::Option::GeneralIdentifier, "GeneralIdentifier"},
     {PartyIdSource::Option::Proprietary, "Proprietary"},
     {PartyIdSource::Option::ISOCountryCode, "ISOCountryCode"},
     {PartyIdSource::Option::SettlementEntityLocation,
      "SettlementEntityLocation"},
     {PartyIdSource::Option::MIC, "MIC"},
     {PartyIdSource::Option::CSDParticipant, "CSDParticipant"},
     {PartyIdSource::Option::AustralianCompanyNumber,
      "AustralianCompanyNumber"},
     {PartyIdSource::Option::AustralianRegisteredBodyNumber,
      "AustralianRegisteredBodyNumber"},
     {PartyIdSource::Option::CFTCReportingFirmIdentifier,
      "CFTCReportingFirmIdentifier"},
     {PartyIdSource::Option::LegalEntityIdentifier, "LegalEntityIdentifier"},
     {PartyIdSource::Option::InterimIdentifier, "InterimIdentifier"},
     {PartyIdSource::Option::ShortCodeIdentifier, "ShortCodeIdentifier"},
     {PartyIdSource::Option::NationalIDNaturalPerson,
      "NationalIDNaturalPerson"},
     {PartyIdSource::Option::IndiaPermanentAccountNumber,
      "IndiaPermanentAccountNumber"},
     {PartyIdSource::Option::FDID, "FDID"},
     {PartyIdSource::Option::SPSAID, "SPSAID"},
     {PartyIdSource::Option::MasterSPSAID, "MasterSPSAID"},
     {PartyIdSource::Option::KoreaShortSellingRegistrationNumber, "KoreaShortSellingRegistrationNumber"}}};

const BiMap<SecurityType::Option> security_type_bimap{
    {{SecurityType::Option::CommonStock, "CommonStock"},
     {SecurityType::Option::Future, "Future"},
     {SecurityType::Option::Option, "Option"},
     {SecurityType::Option::MultiLeg, "MultilegInstrument"},
     {SecurityType::Option::SyntheticMultiLeg, "SyntheticMultiLeg"},
     {SecurityType::Option::Warrant, "Warrant"},
     {SecurityType::Option::MutualFund, "MutualFund"},
     {SecurityType::Option::CorporateBond, "CorporateBond"},
     {SecurityType::Option::ConvertibleBond, "ConvertibleBond"},
     {SecurityType::Option::RepurchaseAgreement, "RepurchaseAgreement"},
     {SecurityType::Option::Index, "Index"},
     {SecurityType::Option::ContractForDifference, "ContractForDifference"},
     {SecurityType::Option::Certificate, "Certificate"},
     {SecurityType::Option::FxSpot, "FXSpot"},
     {SecurityType::Option::Forward, "Forward"},
     {SecurityType::Option::FxForward, "FXForward"},
     {SecurityType::Option::FxNonDeliverableForward, "NonDeliverableForward"},
     {SecurityType::Option::FxSwap, "FXSwap"},
     {SecurityType::Option::FxNonDeliverableSwap, "NonDeliverableSwap"}}};

const BiMap<AggressorSide::Option> aggressor_side_bimap{
    {{AggressorSide::Option::Buy, "Buy"},
     {AggressorSide::Option::Sell, "Sell"},
     {AggressorSide::Option::SellShort, "SellShort"},
     {AggressorSide::Option::SellShortExempt, "SellShortExempt"}}};

const BiMap<TradingPhase::Option> trading_phase_bimap{
    {{TradingPhase::Option::Open, "Continuous"},
     {TradingPhase::Option::Closed, "OutOfMainSessionTrading"},
     {TradingPhase::Option::PostTrading, "PostTrading"},
     {TradingPhase::Option::OpeningAuction, "OpeningOrOpeningAuction"},
     {TradingPhase::Option::IntradayAuction, "ScheduledIntradayAuction"},
     {TradingPhase::Option::ClosingAuction, "ClosingOrClosingAuction"}}};

const BiMap<TradingStatus::Option> trading_status_bimap{
    {{TradingStatus::Option::Halt, "TradingHalt"},
     {TradingStatus::Option::Resume, "Resume"}}};

const BiMap<SecurityIdSource::Option> security_id_source_bimap{
    {{SecurityIdSource::Option::Cusip, "CUSIP"},
     {SecurityIdSource::Option::Sedol, "SEDOL"},
     {SecurityIdSource::Option::Isin, "ISIN"},
     {SecurityIdSource::Option::Ric, "RIC"},
     {SecurityIdSource::Option::ExchangeSymbol, "ExchangeSymbol"},
     {SecurityIdSource::Option::BloombergSymbol, "BloombergSymbol"}}};

const BiMap<TimeInForce::Option> time_in_force_bimap{
    {{TimeInForce::Option::Day, "Day"},
     {TimeInForce::Option::ImmediateOrCancel, "ImmediateOrCancel"},
     {TimeInForce::Option::FillOrKill, "FillOrKill"},
     {TimeInForce::Option::GoodTillDate, "GoodTillDate"},
     {TimeInForce::Option::GoodTillCancel, "GoodTillCancel"}}};

const BiMap<Side::Option> side_bimap{
    {{Side::Option::Buy, "Buy"},
     {Side::Option::Sell, "Sell"},
     {Side::Option::SellShort, "SellShort"},
     {Side::Option::SellShortExempt, "SellShortExempt"}}};

const BiMap<OrderStatus::Option> order_status_bimap{
    {{OrderStatus::Option::New, "New"},
     {OrderStatus::Option::PartiallyFilled, "PartiallyFilled"},
     {OrderStatus::Option::Filled, "Filled"},
     {OrderStatus::Option::Modified, "Replaced"},
     {OrderStatus::Option::Cancelled, "Canceled"},
     {OrderStatus::Option::Rejected, "Rejected"}}};

const BiMap<market_state::SessionType> session_type_bimap{
    {{market_state::SessionType::Fix, "Fix"},
     {market_state::SessionType::Generator, "Generator"}}};

auto convert(PartyRole role) -> tl::expected<std::string_view, std::string> {
  if (const auto iter = party_role_bimap.value_to_str.find(role.value());
      iter != std::end(party_role_bimap.value_to_str)) {
    return iter->second;
  }
  return tl::unexpected(fmt::format(
      "Unknown PartyRole value `{}'",
      static_cast<std::underlying_type_t<PartyRole::Option>>(role.value())));
}

auto convert(const std::string_view& str, PartyRole& role)
    -> tl::expected<void, std::string> {
  if (const auto iter = party_role_bimap.str_to_value.find(str);
      iter != std::end(party_role_bimap.str_to_value)) {
    role = iter->second;
    return {};
  }
  return tl::unexpected(fmt::format("Unknown PartyRole string `{}'", str));
}

auto convert(PartyIdSource source)
    -> tl::expected<std::string_view, std::string> {
  if (const auto iter = party_id_source_bimap.value_to_str.find(source.value());
      iter != std::end(party_id_source_bimap.value_to_str)) {
    return iter->second;
  }
  return tl::unexpected(
      fmt::format("Unknown PartyIdSource value `{}'",
                  static_cast<std::underlying_type_t<PartyIdSource::Option>>(
                      source.value())));
}

auto convert(const std::string_view& str, PartyIdSource& source)
    -> tl::expected<void, std::string> {
  if (const auto iter = party_id_source_bimap.str_to_value.find(str);
      iter != std::end(party_id_source_bimap.str_to_value)) {
    source = iter->second;
    return {};
  }
  return tl::unexpected(fmt::format("Unknown PartyIdSource string `{}'", str));
}

auto convert(SecurityType type) -> tl::expected<std::string_view, std::string> {
  if (const auto iter = security_type_bimap.value_to_str.find(type.value());
      iter != std::end(security_type_bimap.value_to_str)) {
    return iter->second;
  }
  return tl::unexpected(fmt::format(
      "Unknown SecurityType value `{}'",
      static_cast<std::underlying_type_t<SecurityType::Option>>(type.value())));
}

auto convert(const std::string_view& str, SecurityType& type)
    -> tl::expected<void, std::string> {
  if (const auto iter = security_type_bimap.str_to_value.find(str);
      iter != std::end(security_type_bimap.str_to_value)) {
    type = iter->second;
    return {};
  }
  return tl::unexpected(fmt::format("Unknown SecurityType string `{}'", str));
}

auto convert(AggressorSide side)
    -> tl::expected<std::string_view, std::string> {
  if (const auto iter = aggressor_side_bimap.value_to_str.find(side.value());
      iter != std::end(aggressor_side_bimap.value_to_str)) {
    return iter->second;
  }
  return tl::unexpected(
      fmt::format("Unknown AggressorSide value `{}'",
                  static_cast<std::underlying_type_t<AggressorSide::Option>>(
                      side.value())));
}

auto convert(const std::string_view& str, AggressorSide& side)
    -> tl::expected<void, std::string> {
  if (const auto iter = aggressor_side_bimap.str_to_value.find(str);
      iter != std::end(aggressor_side_bimap.str_to_value)) {
    side = iter->second;
    return {};
  }
  return tl::unexpected(fmt::format("Unknown AggressorSide string `{}'", str));
}

auto convert(TradingPhase phase)
    -> tl::expected<std::string_view, std::string> {
  if (const auto iter = trading_phase_bimap.value_to_str.find(phase.value());
      iter != std::end(trading_phase_bimap.value_to_str)) {
    return iter->second;
  }
  return tl::unexpected(
      fmt::format("Unknown TradingPhase value `{}'",
                  static_cast<std::underlying_type_t<TradingPhase::Option>>(
                      phase.value())));
}

auto convert(const std::string_view& str, TradingPhase& phase)
    -> tl::expected<void, std::string> {
  if (const auto iter = trading_phase_bimap.str_to_value.find(str);
      iter != std::end(trading_phase_bimap.str_to_value)) {
    phase = iter->second;
    return {};
  }
  return tl::unexpected(fmt::format("Unknown TradingPhase string `{}'", str));
}

auto convert(TradingStatus status)
    -> tl::expected<std::string_view, std::string> {
  if (const auto iter = trading_status_bimap.value_to_str.find(status.value());
      iter != std::end(trading_status_bimap.value_to_str)) {
    return iter->second;
  }
  return tl::unexpected(
      fmt::format("Unknown TradingStatus value `{}'",
                  static_cast<std::underlying_type_t<TradingStatus::Option>>(
                      status.value())));
}

auto convert(const std::string_view& str, TradingStatus& status)
    -> tl::expected<void, std::string> {
  if (const auto iter = trading_status_bimap.str_to_value.find(str);
      iter != std::end(trading_status_bimap.str_to_value)) {
    status = iter->second;
    return {};
  }
  return tl::unexpected(fmt::format("Unknown TradingStatus string `{}'", str));
}

auto convert(SecurityIdSource source)
    -> tl::expected<std::string_view, std::string> {
  if (const auto iter =
          security_id_source_bimap.value_to_str.find(source.value());
      iter != std::end(security_id_source_bimap.value_to_str)) {
    return iter->second;
  }
  return tl::unexpected(
      fmt::format("Unknown SecurityIdSource value `{}'",
                  static_cast<std::underlying_type_t<SecurityIdSource::Option>>(
                      source.value())));
}

auto convert(const std::string_view& str, SecurityIdSource& source)
    -> tl::expected<void, std::string> {
  if (const auto iter = security_id_source_bimap.str_to_value.find(str);
      iter != std::end(security_id_source_bimap.str_to_value)) {
    source = iter->second;
    return {};
  }
  return tl::unexpected(
      fmt::format("Unknown SecurityIdSource string `{}'", str));
}

auto convert(TimeInForce time_in_force)
    -> tl::expected<std::string_view, std::string> {
  if (const auto iter =
          time_in_force_bimap.value_to_str.find(time_in_force.value());
      iter != std::end(time_in_force_bimap.value_to_str)) {
    return iter->second;
  }
  return tl::unexpected(
      fmt::format("Unknown TimeInForce value `{}'",
                  static_cast<std::underlying_type_t<TimeInForce::Option>>(
                      time_in_force.value())));
}

auto convert(const std::string_view& str, TimeInForce& time_in_force)
    -> tl::expected<void, std::string> {
  if (const auto iter = time_in_force_bimap.str_to_value.find(str);
      iter != std::end(time_in_force_bimap.str_to_value)) {
    time_in_force = iter->second;
    return {};
  }
  return tl::unexpected(fmt::format("Unknown TimeInForce string `{}'", str));
}

auto convert(Side side) -> tl::expected<std::string_view, std::string> {
  if (const auto iter = side_bimap.value_to_str.find(side.value());
      iter != std::end(side_bimap.value_to_str)) {
    return iter->second;
  }
  return tl::unexpected(fmt::format(
      "Unknown Side value `{}'",
      static_cast<std::underlying_type_t<Side::Option>>(side.value())));
}

auto convert(const std::string_view& str, Side& side)
    -> tl::expected<void, std::string> {
  if (const auto iter = side_bimap.str_to_value.find(str);
      iter != std::end(side_bimap.str_to_value)) {
    side = iter->second;
    return {};
  }
  return tl::unexpected(fmt::format("Unknown Side string `{}'", str));
}

auto convert(OrderStatus status)
    -> tl::expected<std::string_view, std::string> {
  if (const auto iter = order_status_bimap.value_to_str.find(status.value());
      iter != std::end(order_status_bimap.value_to_str)) {
    return iter->second;
  }
  return tl::unexpected(
      fmt::format("Unknown OrderStatus value `{}'",
                  static_cast<std::underlying_type_t<OrderStatus::Option>>(
                      status.value())));
}

auto convert(const std::string_view& str, OrderStatus& status)
    -> tl::expected<void, std::string> {
  if (const auto iter = order_status_bimap.str_to_value.find(str);
      iter != std::end(order_status_bimap.str_to_value)) {
    status = iter->second;
    return {};
  }
  return tl::unexpected(fmt::format("Unknown OrderStatus string `{}'", str));
}

auto convert(market_state::SessionType type)
    -> tl::expected<std::string_view, std::string> {
  if (const auto iter = session_type_bimap.value_to_str.find(type);
      iter != std::end(session_type_bimap.value_to_str)) {
    return iter->second;
  }
  return tl::unexpected(fmt::format(
      "Unknown SessionType value `{}'",
      static_cast<std::underlying_type_t<market_state::SessionType>>(type)));
}

auto convert(const std::string_view& str, market_state::SessionType& type)
    -> tl::expected<void, std::string> {
  if (const auto iter = session_type_bimap.str_to_value.find(str);
      iter != std::end(session_type_bimap.str_to_value)) {
    type = iter->second;
    return {};
  }
  return tl::unexpected(fmt::format("Unknown SessionType string `{}'", str));
}

}  // namespace

auto read(const rapidjson::Value& json_value, PartyRole& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;
  return read(json_value, str).and_then([&] { return convert(str, dest); });
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const PartyRole& source) -> tl::expected<void, std::string> {
  return convert(source).and_then(
      [&](std::string_view s) -> tl::expected<void, std::string> {
        return write(json_value, allocator, s);
      });
}

auto read(const rapidjson::Value& json_value, PartyIdSource& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;
  return read(json_value, str).and_then([&] { return convert(str, dest); });
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const PartyIdSource& source) -> tl::expected<void, std::string> {
  return convert(source).and_then(
      [&](std::string_view s) -> tl::expected<void, std::string> {
        return write(json_value, allocator, s);
      });
}

auto read(const rapidjson::Value& json_value, SecurityType& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;
  return read(json_value, str).and_then([&] { return convert(str, dest); });
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const SecurityType& source) -> tl::expected<void, std::string> {
  return convert(source).and_then(
      [&](std::string_view s) -> tl::expected<void, std::string> {
        return write(json_value, allocator, s);
      });
}

auto read(const rapidjson::Value& json_value, AggressorSide& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;
  return read(json_value, str).and_then([&] { return convert(str, dest); });
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const AggressorSide& source) -> tl::expected<void, std::string> {
  return convert(source).and_then(
      [&](std::string_view s) -> tl::expected<void, std::string> {
        return write(json_value, allocator, s);
      });
}

auto read(const rapidjson::Value& json_value, TradingPhase& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;
  return read(json_value, str).and_then([&] { return convert(str, dest); });
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const TradingPhase& source) -> tl::expected<void, std::string> {
  return convert(source).and_then(
      [&](std::string_view s) -> tl::expected<void, std::string> {
        return write(json_value, allocator, s);
      });
}

auto read(const rapidjson::Value& json_value, TradingStatus& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;
  return read(json_value, str).and_then([&] { return convert(str, dest); });
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const TradingStatus& source) -> tl::expected<void, std::string> {
  return convert(source).and_then(
      [&](std::string_view s) -> tl::expected<void, std::string> {
        return write(json_value, allocator, s);
      });
}

auto read(const rapidjson::Value& json_value, SecurityIdSource& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;
  return read(json_value, str).and_then([&] { return convert(str, dest); });
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const SecurityIdSource& source) -> tl::expected<void, std::string> {
  return convert(source).and_then(
      [&](std::string_view s) -> tl::expected<void, std::string> {
        return write(json_value, allocator, s);
      });
}

auto read(const rapidjson::Value& json_value, TimeInForce& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;
  return read(json_value, str).and_then([&] { return convert(str, dest); });
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const TimeInForce& source) -> tl::expected<void, std::string> {
  return convert(source).and_then(
      [&](std::string_view s) -> tl::expected<void, std::string> {
        return write(json_value, allocator, s);
      });
}

auto read(const rapidjson::Value& json_value, Side& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;
  return read(json_value, str).and_then([&] { return convert(str, dest); });
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const Side& source) -> tl::expected<void, std::string> {
  return convert(source).and_then(
      [&](std::string_view s) -> tl::expected<void, std::string> {
        return write(json_value, allocator, s);
      });
}

auto read(const rapidjson::Value& json_value, OrderStatus& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;
  return read(json_value, str).and_then([&] { return convert(str, dest); });
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const OrderStatus& source) -> tl::expected<void, std::string> {
  return convert(source).and_then(
      [&](std::string_view s) -> tl::expected<void, std::string> {
        return write(json_value, allocator, s);
      });
}

auto read(const rapidjson::Value& json_value, market_state::SessionType& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;
  return read(json_value, str).and_then([&] { return convert(str, dest); });
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::SessionType& source)
    -> tl::expected<void, std::string> {
  return convert(source).and_then(
      [&](std::string_view s) -> tl::expected<void, std::string> {
        return write(json_value, allocator, s);
      });
}

}  // namespace simulator::trading_system::json
