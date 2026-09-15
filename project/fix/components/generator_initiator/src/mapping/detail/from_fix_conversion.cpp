#include "ih/mapping/detail/from_fix_conversion.hpp"

#include <quickfix/Fields.h>
#include <quickfix/FixValues.h>

#include <string>
#include <string_view>
#include <unordered_map>

#include "common/custom_values.hpp"

namespace simulator::fix::generator_initiator::detail {

namespace {

template <typename KeyType, typename EnumType>
using EnumAssociationTable = std::unordered_map<KeyType, EnumType>;

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
auto make_market_entry_action_association()
    -> EnumAssociationTable<char, MarketEntryAction::Option> {
  // clang-format off
  return {
    {FIX::MDUpdateAction_NEW, MarketEntryAction::Option::New},
    {FIX::MDUpdateAction_CHANGE, MarketEntryAction::Option::Change},
    {FIX::MDUpdateAction_DELETE, MarketEntryAction::Option::Delete}};
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
auto make_md_entry_type_association()
    -> EnumAssociationTable<char, MdEntryType::Option> {
  // clang-format off
  return {
      {FIX::MDEntryType_BID, MdEntryType::Option::Bid},
      {FIX::MDEntryType_OFFER, MdEntryType::Option::Offer},
      {FIX::MDEntryType_TRADE, MdEntryType::Option::Trade},
      {FIX::MDEntryType_OPENING_PRICE, MdEntryType::Option::OpeningPrice},
      {FIX::MDEntryType_CLOSING_PRICE, MdEntryType::Option::ClosingPrice},
      {FIX::MDEntryType_SETTLEMENT_PRICE, MdEntryType::Option::SettlementPrice},
      {FIX::MDEntryType_TRADING_SESSION_HIGH_PRICE, MdEntryType::Option::HighPrice},
      {FIX::MDEntryType_TRADING_SESSION_LOW_PRICE, MdEntryType::Option::LowPrice},
      {FIX::MDEntryType_IMBALANCE, MdEntryType::Option::Imbalance},
      {FIX::MDEntryType_TRADE_VOLUME, MdEntryType::Option::TradeVolume},
      {FIX::MDEntryType_MID_PRICE, MdEntryType::Option::MidPrice},
      {FIX::MDEntryType_EARLY_PRICES, MdEntryType::Option::EarlyPrice},
      {FIX::MDEntryType_AUCTION_CLEARING_PRICE, MdEntryType::Option::AuctionClearingPrice},
      {FIX::MDEntryType_MARKET_BID, MdEntryType::Option::MarketBid},
      {FIX::MDEntryType_MARKET_OFFER, MdEntryType::Option::MarketOffer},
      {FIX::MDEntryType_PREVIOUS_CLOSING_PRICE, MdEntryType::Option::PreviousClosingPrice}};
  // clang-format on
}

[[nodiscard]]
auto make_md_reject_reason_association()
    -> EnumAssociationTable<char, MdRejectReason::Option> {
  // clang-format off
  return {
      {FIX::MDReqRejReason_UNKNOWN_SYMBOL, MdRejectReason::Option::UnknownSymbol},
      {FIX::MDReqRejReason_DUPLICATE_MDREQID, MdRejectReason::Option::DuplicateMdReqId}};
  // clang-format on
}

}  // namespace

auto FromFixConverter<Side::Option>::convert(const FIX::AggressorSide& field)
    -> Side::Option {
  const static auto association = make_side_mapping_association();

  const char value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  fix::detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<MarketEntryAction::Option>::convert(
    const FIX::MDUpdateAction& field) -> MarketEntryAction::Option {
  const static auto association = make_market_entry_action_association();

  const char value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  fix::detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<SecurityType::Option>::convert(
    const FIX::SecurityType& field) -> SecurityType::Option {
  const static auto association = make_security_type_mapping_association();

  const std::string& value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  fix::detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<SecurityIdSource::Option>::convert(
    const FIX::SecurityIDSource& field) -> SecurityIdSource::Option {
  const static auto association = make_security_id_source_mapping_association();

  const std::string& value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  fix::detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<MdEntryType::Option>::convert(
    const FIX::MDEntryType& field) -> MdEntryType::Option {
  const static auto association = make_md_entry_type_association();

  const char value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  fix::detail::from_fix_error::field_has_bad_value(field);
}

auto FromFixConverter<MdRejectReason::Option>::convert(
    const FIX::MDReqRejReason& field) -> MdRejectReason::Option {
  const static auto association = make_md_reject_reason_association();

  const char value = field.getValue();
  if (const auto iter = association.find(value); iter != association.end()) {
    return iter->second;
  }

  fix::detail::from_fix_error::field_has_bad_value(field);
}

}  // namespace simulator::fix::generator_initiator::detail