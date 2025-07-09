#ifndef SIMULATOR_CORE_DOMAIN_ENUMERATORS_HPP_
#define SIMULATOR_CORE_DOMAIN_ENUMERATORS_HPP_

#include <fmt/format.h>

#include <iosfwd>
#include <optional>
#include <string_view>

namespace simulator::core::enumerators {

enum class ExecutionType : std::uint8_t {
  OrderPlaced,
  OrderModified,
  OrderCancelled,
  Rejected,
  OrderTraded
};

enum class OrderStatus : std::uint8_t {
  New,
  PartiallyFilled,
  Filled,
  Modified,
  Cancelled,
  Rejected
};

enum class OrderType : std::uint8_t { Limit, Market };

enum class PartyIdentifierSource : std::uint8_t {
  UKNationalInsuranceOrPensionNumber,
  USSocialSecurityNumber,
  USEmployerOrTaxIDNumber,
  AustralianBusinessNumber,
  AustralianTaxFileNumber,
  TaxID,
  KoreanInvestorID,
  TaiwaneseForeignInvestorID,
  TaiwaneseTradingAcct,
  MalaysianCentralDepository,
  ChineseInvestorID,
  ISITCAcronym,
  BIC,
  GeneralIdentifier,
  Proprietary,
  ISOCountryCode,
  SettlementEntityLocation,
  MIC,
  CSDParticipant,
  AustralianCompanyNumber,
  AustralianRegisteredBodyNumber,
  CFTCReportingFirmIdentifier,
  LegalEntityIdentifier,
  InterimIdentifier,
  ShortCodeIdentifier,
  NationalIDNaturalPerson,
  IndiaPermanentAccountNumber,
  FDID,
  SPSAID,
  MasterSPSAID
};

enum class PartyRole : std::uint8_t {
  ExecutingFirm,
  BrokerOfCredit,
  ClientID,
  ClearingFirm,
  InvestorID,
  IntroducingFirm,
  EnteringFirm,
  Locate,
  FundManagerClientID,
  SettlementLocation,
  OrderOriginationTrader,
  ExecutingTrader,
  OrderOriginationFirm,
  CorrespondantClearingFirm,
  ExecutingSystem,
  ContraFirm,
  ContraClearingFirm,
  SponsoringFirm,
  UnderlyingContraFirm,
  ClearingOrganization,
  Exchange,
  CustomerAccount,
  CorrespondentClearingOrganization,
  CorrespondentBroker,
  Buyer,
  Custodian,
  Intermediary,
  Agent,
  SubCustodian,
  Beneficiary,
  InterestedParty,
  RegulatoryBody,
  LiquidityProvider,
  EnteringTrader,
  ContraTrader,
  PositionAccount,
  ContraInvestorID,
  TransferToFirm,
  ContraPositionAccount,
  ContraExchange,
  InternalCarryAccount,
  OrderEntryOperatorID,
  SecondaryAccountNumber,
  ForeignFirm,
  ThirdPartyAllocationFirm,
  ClaimingAccount,
  AssetManager,
  PledgorAccount,
  PledgeeAccount,
  LargeTraderReportableAccount,
  TraderMnemonic,
  SenderLocation,
  SessionID,
  AcceptableCounterparty,
  UnacceptableCounterparty,
  EnteringUnit,
  ExecutingUnit,
  IntroducingBroker,
  QuoteOriginator,
  ReportOriginator,
  SystematicInternaliser,
  MultilateralTradingFacility,
  RegulatedMarket,
  MarketMaker,
  InvestmentFirm,
  HostCompetentAuthority,
  HomeCompetentAuthority,
  CompetentAuthorityLiquidity,
  CompetentAuthorityTransactionVenue,
  ReportingIntermediary,
  ExecutionVenue,
  MarketDataEntryOriginator,
  LocationID,
  DeskID,
  MarketDataMarket,
  AllocationEntity,
  PrimeBroker,
  StepOutFirm,
  BrokerClearingID,
  CentralRegistrationDepository,
  ClearingAccount,
  AcceptableSettlingCounterparty,
  UnacceptableSettlingCounterparty,
  CLSMemberBank,
  InConcertGroup,
  InConcertControllingEntity,
  LargePositionsReportingAccount,
  SettlementFirm,
  SettlementAccount,
  ReportingMarketCenter,
  RelatedReportingMarketCenter,
  AwayMarket,
  GiveupTradingFirm,
  TakeupTradingFirm,
  GiveupClearingFirm,
  TakeupClearingFirm,
  OriginatingMarket,
  MarginAccount,
  CollateralAssetAccount,
  DataRepository,
  CalculationAgent,
  ExerciseNoticeSender,
  ExerciseNoticeReceiver,
  RateReferenceBank,
  Correspondent,
  BeneficiaryBank,
  Borrower,
  PrimaryObligator,
  Guarantor,
  ExcludedReferenceEntity,
  DeterminingParty,
  HedgingParty,
  ReportingEntity,
  SalesPerson,
  Operator,
  CSD,
  ICSD,
  TradingSubAccount,
  InvestmentDecisionMaker,
  PublishingIntermediary,
  CSDParticipant,
  Issuer,
  ContraCustomerAccount,
  ContraInvestmentDecisionMaker
};

enum class SecurityIdSource : std::uint8_t {
  Cusip,
  Sedol,
  Isin,
  Ric,
  ExchangeSymbol,
  BloombergSymbol
};

enum class SecurityType : std::uint8_t {
  CommonStock,
  Future,
  Option,
  MultiLeg,
  SyntheticMultiLeg,
  Warrant,
  MutualFund,
  CorporateBond,
  ConvertibleBond,
  RepurchaseAgreement,
  Index,
  ContractForDifference,
  Certificate,
  FxSpot,
  Forward,
  FxForward,
  FxNonDeliverableForward,
  FxSwap,
  FxNonDeliverableSwap
};

enum class Side : std::uint8_t { Buy, Sell, SellShort, SellShortExempt };

enum class TimeInForce : std::uint8_t {
  Day,
  ImmediateOrCancel,
  FillOrKill,
  GoodTillDate,
  GoodTillCancel
};

enum class MdEntryType : std::uint8_t {
  Bid,
  Offer,
  Trade,
  LowPrice,
  MidPrice,
  HighPrice
};

enum class MdSubscriptionRequestType : std::uint8_t {
  Subscribe,
  Unsubscribe,
  Snapshot
};

enum class MarketEntryAction : std::uint8_t { New, Change, Delete };

enum class MarketDataUpdateType : std::uint8_t { Snapshot, Incremental };

enum class MdRejectReason : std::uint8_t { UnknownSymbol, DuplicateMdReqId };

enum class TradingPhase : std::uint8_t {
  Open,
  Closed,
  PostTrading,
  OpeningAuction,
  IntradayAuction,
  ClosingAuction
};

enum class TradingStatus : std::uint8_t { Halt, Resume };

enum class RejectedMessageType : std::uint8_t { SecurityStatusRequest };

enum class BusinessRejectReason : std::uint8_t {
  Other,
  UnknownId,
  UnknownSecurity
};

[[nodiscard]]
auto convert_security_type_from_string(std::string_view value) noexcept
    -> std::optional<SecurityType>;

[[nodiscard]]
auto convert_party_role_from_string(std::string_view value) noexcept
    -> std::optional<PartyRole>;

auto operator<<(std::ostream& stream, ExecutionType execution_type)
    -> std::ostream&;

auto operator<<(std::ostream& stream, OrderStatus order_status)
    -> std::ostream&;

auto operator<<(std::ostream& stream, OrderType order_type) -> std::ostream&;

auto operator<<(std::ostream& stream, PartyIdentifierSource party_id_source)
    -> std::ostream&;

auto operator<<(std::ostream& stream, PartyRole party_role) -> std::ostream&;

auto operator<<(std::ostream& stream, SecurityIdSource security_id_source)
    -> std::ostream&;

auto operator<<(std::ostream& stream, SecurityType security_type)
    -> std::ostream&;

auto operator<<(std::ostream& stream, Side side) -> std::ostream&;

auto operator<<(std::ostream& stream, TimeInForce time_in_force)
    -> std::ostream&;

auto operator<<(std::ostream& stream, MdEntryType md_entry_type)
    -> std::ostream&;

auto operator<<(std::ostream& stream,
                MdSubscriptionRequestType subscription_type) -> std::ostream&;

auto operator<<(std::ostream& stream, MdRejectReason reject_reason)
    -> std::ostream&;

auto operator<<(std::ostream& stream, MarketEntryAction action)
    -> std::ostream&;

auto operator<<(std::ostream& stream, MarketDataUpdateType type)
    -> std::ostream&;

auto operator<<(std::ostream& stream, TradingPhase phase) -> std::ostream&;

auto operator<<(std::ostream& stream, TradingStatus status) -> std::ostream&;

auto operator<<(std::ostream& stream, RejectedMessageType type)
    -> std::ostream&;

auto operator<<(std::ostream& stream, BusinessRejectReason reason)
    -> std::ostream&;

}  // namespace simulator::core::enumerators

template <>
struct fmt::formatter<simulator::core::enumerators::ExecutionType>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::ExecutionType;

  auto format(formattable execution_type, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::OrderStatus>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::OrderStatus;

  auto format(formattable order_status, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::OrderType>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::OrderType;

  auto format(formattable order_type, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::PartyIdentifierSource>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::PartyIdentifierSource;

  auto format(formattable party_id_source, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::PartyRole>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::PartyRole;

  auto format(formattable party_role, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::SecurityIdSource>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::SecurityIdSource;

  auto format(formattable security_id_source, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::SecurityType>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::SecurityType;

  auto format(formattable security_type, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::Side>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::Side;

  auto format(formattable side, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::TimeInForce>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::TimeInForce;

  auto format(formattable time_in_force, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::MdEntryType>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::MdEntryType;

  auto format(formattable md_entry_type, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::MdSubscriptionRequestType>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::MdSubscriptionRequestType;

  auto format(formattable subscription_type, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::MdRejectReason>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::MdRejectReason;

  auto format(formattable reject_reason, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::MarketDataUpdateType>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::MarketDataUpdateType;

  auto format(formattable type, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::MarketEntryAction>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::MarketEntryAction;

  auto format(formattable action, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::TradingPhase>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::TradingPhase;

  auto format(formattable phase, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::TradingStatus>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::TradingStatus;

  auto format(formattable status, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::RejectedMessageType>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::RejectedMessageType;

  auto format(formattable status, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::enumerators::BusinessRejectReason>
    : formatter<std::string_view> {
  using formattable = simulator::core::enumerators::BusinessRejectReason;

  auto format(formattable reason, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_CORE_DOMAIN_ENUMERATORS_HPP_
