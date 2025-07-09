#ifndef SIMULATOR_CORE_DOMAIN_ATTRIBUTES_HPP_
#define SIMULATOR_CORE_DOMAIN_ATTRIBUTES_HPP_

#include <fmt/base.h>

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

#include "core/common/attribute.hpp"
#include "core/common/name.hpp"
#include "core/domain/enumerators.hpp"

// Primary attributes

namespace simulator::tag {

struct ClientOrderId {
  constexpr static core::Name name{.singular = "ClientOrderID",
                                   .plural = "ClientOrderIDs"};
};

struct Currency {
  constexpr static core::Name name{.singular = "Currency",
                                   .plural = "Currencies"};
};

struct ExecutionId {
  constexpr static core::Name name{.singular = "ExecutionID",
                                   .plural = "ExecutionIDs"};
};

struct ExecutionType {
  using value_type = core::enumerators::ExecutionType;
  constexpr static core::Name name{.singular = "ExecutionType",
                                   .plural = "ExecutionTypes"};
};

struct ExpireDate {
  constexpr static core::Name name{.singular = "ExpireDate",
                                   .plural = "ExpireDates"};
};

struct ExpireTime {
  constexpr static core::Name name{.singular = "ExpireTime",
                                   .plural = "ExpireTimes"};
};

struct OrderStatus {
  using value_type = core::enumerators::OrderStatus;
  constexpr static core::Name name{.singular = "OrderStatus",
                                   .plural = "OrderStatuses"};
};

struct OrderType {
  using value_type = core::enumerators::OrderType;
  constexpr static core::Name name{.singular = "OrderType",
                                   .plural = "OrderTypes"};
};

struct OrigClientOrderId {
  constexpr static core::Name name{.singular = "OrigClientOrderID",
                                   .plural = "OrigClientOrderIDs"};
};

struct PartyId {
  constexpr static core::Name name{.singular = "PartyID", .plural = "PartyIDs"};
};

struct PartyIdSource {
  using value_type = core::enumerators::PartyIdentifierSource;
  constexpr static core::Name name{.singular = "PartyIDSource",
                                   .plural = "PartyIDSources"};
};

struct PartyRole {
  using value_type = core::enumerators::PartyRole;
  constexpr static core::Name name{.singular = "PartyRole",
                                   .plural = "PartyRoles"};
};

struct Price {
  using value_type = double;
  constexpr static core::Name name{.singular = "Price", .plural = "Prices"};
};

struct Quantity {
  using value_type = double;
  constexpr static core::Name name{.singular = "Quantity",
                                   .plural = "Quantities"};
};

struct MarketDepth {
  using value_type = std::uint32_t;
  constexpr static core::Name name{.singular = "MarketDepth",
                                   .plural = "MarketDepths"};
};

struct RequesterInstrumentId {
  using value_type = std::uint64_t;
  constexpr static core::Name name{.singular = "RequesterInstrumentID",
                                   .plural = "RequesterInstrumentID"};
};

struct ShortSaleExemptionReason {
  using value_type = std::int32_t;
  constexpr static core::Name name{.singular = "SellShortExemptReason",
                                   .plural = "SellShortExemptReasons"};
};

struct RejectText {
  constexpr static core::Name name{.singular = "RejectText",
                                   .plural = "RejectTexts"};
};

struct SecurityExchange {
  constexpr static core::Name name{.singular = "SecurityExchange",
                                   .plural = "SecurityExchanges"};
};

struct SecurityId {
  constexpr static core::Name name{.singular = "SecurityID",
                                   .plural = "SecurityIDs"};
};

struct SecurityIdSource {
  using value_type = core::enumerators::SecurityIdSource;
  constexpr static core::Name name{.singular = "SecurityIDSource",
                                   .plural = "SecurityIDsSources"};
};

struct SecurityType {
  using value_type = core::enumerators::SecurityType;
  constexpr static core::Name name{.singular = "SecurityType",
                                   .plural = "SecurityTypes"};
};

struct Side {
  using value_type = core::enumerators::Side;
  constexpr static core::Name name{.singular = "Side", .plural = "Sides"};
};

struct Symbol {
  constexpr static core::Name name{.singular = "Symbol", .plural = "Symbols"};
};

struct TimeInForce {
  using value_type = core::enumerators::TimeInForce;
  constexpr static core::Name name{.singular = "TimeInForce",
                                   .plural = "TimesInForce"};
};

struct MarketEntryTime {
  constexpr static core::Name name{.singular = "MarketEntryTime",
                                   .plural = "MarketEntryTimes"};
};

struct VenueOrderId {
  constexpr static core::Name name{.singular = "VenueOrderID",
                                   .plural = "VenueOrderIDs"};
};

struct MdEntryType {
  using value_type = core::enumerators::MdEntryType;
  constexpr static core::Name name{.singular = "MDEntryType",
                                   .plural = "MDEntryTypes"};
};

struct MdSubscriptionRequestType {
  using value_type = core::enumerators::MdSubscriptionRequestType;
  constexpr static core::Name name{.singular = "MDSubscriptionRequestType",
                                   .plural = "MDSubscriptionRequestTypes"};
};

struct TradingPhase {
  using value_type = core::enumerators::TradingPhase;
  constexpr static core::Name name{.singular = "TradingPhase",
                                   .plural = "TradingPhases"};
};

struct TradingStatus {
  using value_type = core::enumerators::TradingStatus;
  constexpr static core::Name name{.singular = "TradingStatus",
                                   .plural = "TradingStatuses"};
};

struct MarketEntryAction {
  using value_type = core::enumerators::MarketEntryAction;
  constexpr static core::Name name{.singular = "MarketEntryAction",
                                   .plural = "MarketEntryActions"};
};

struct MarketDataUpdateType {
  using value_type = core::enumerators::MarketDataUpdateType;
  constexpr static core::Name name{.singular = "MarketDataUpdateType",
                                   .plural = "MarketDataUpdateTypes"};
};

struct MdRejectReason {
  using value_type = core::enumerators::MdRejectReason;
  constexpr static core::Name name{.singular = "MDRejectReason",
                                   .plural = "MDRejectReasons"};
};

struct MdRequestId {
  constexpr static core::Name name{.singular = "MDRequestID",
                                   .plural = "MDRequestIDs"};
};

struct MarketEntryId {
  constexpr static core::Name name{.singular = "MarketEntryID",
                                   .plural = "MarketEntryIDs"};
};

struct BuyerId {
  constexpr static core::Name name{.singular = "BuyerID", .plural = "BuyerIDs"};
};

struct SellerId {
  constexpr static core::Name name{.singular = "SellerID",
                                   .plural = "SellerIDs"};
};

struct SecurityStatusReqId {
  constexpr static core::Name name{.singular = "SecurityStatusReqID",
                                   .plural = "SecurityStatusReqIDs"};
};

struct SeqNum {
  using value_type = std::uint64_t;
  constexpr static core::Name name{.singular = "SeqNum", .plural = "SeqNums"};
};

struct RejectedMessageType {
  using value_type = core::enumerators::RejectedMessageType;
  constexpr static core::Name name{.singular = "RejectedMessageType",
                                   .plural = "RejectedMessageTypes"};
};

struct BusinessRejectReason {
  using value_type = core::enumerators::BusinessRejectReason;
  constexpr static core::Name name{.singular = "BusinessRejectReason",
                                   .plural = "BusinessRejectReasons"};
};

struct BusinessRejectRefId {
  constexpr static core::Name name{.singular = "BusinessRejectRefID",
                                   .plural = "BusinessRejectRefIDs"};
};

}  // namespace simulator::tag

SIMULATOR_DECLARE_ATTRIBUTE(simulator, Price, Arithmetic);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, Quantity, Arithmetic);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, MarketDepth, Arithmetic);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, RequesterInstrumentId, Arithmetic);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, ShortSaleExemptionReason, Arithmetic);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, SeqNum, Arithmetic);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, ExecutionType);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, OrderStatus);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, OrderType);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, PartyIdSource);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, PartyRole);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, SecurityIdSource);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, SecurityType);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, Side);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, TimeInForce);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, MdEntryType);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, MdSubscriptionRequestType);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, MdRejectReason);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, MarketEntryAction);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, MarketDataUpdateType);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, TradingPhase);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, TradingStatus);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, RejectedMessageType);
SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(simulator, BusinessRejectReason);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, ExpireDate, LocalDate);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, ExpireTime, UtcTimestamp);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, MarketEntryTime, UtcTimestamp);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, BusinessRejectRefId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, ClientOrderId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, Currency, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, ExecutionId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, OrigClientOrderId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, PartyId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, RejectText, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, SecurityExchange, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, SecurityId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, Symbol, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, VenueOrderId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, MdRequestId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, MarketEntryId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, BuyerId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, SellerId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, SecurityStatusReqId, Literal);

// Derived attributes

namespace simulator::tag {

struct OrderPrice {
  using primary_type = simulator::Price;
  constexpr static core::Name name{.singular = "OrderPrice",
                                   .plural = "OrderPrices"};
};

struct BestBidPrice {
  using primary_type = simulator::Price;
  constexpr static core::Name name{.singular = "BestBidPrice",
                                   .plural = "BestBidPrices"};
};

struct BestOfferPrice {
  using primary_type = simulator::Price;
  constexpr static core::Name name{.singular = "BestOfferPrice",
                                   .plural = "BestOfferPrices"};
};

struct ExecutionPrice {
  using primary_type = simulator::Price;
  constexpr static core::Name name{.singular = "ExecutionPrice",
                                   .plural = "ExecutionPrices"};
};

struct CumExecutedQuantity {
  using primary_type = simulator::Quantity;
  constexpr static core::Name name{.singular = "CumExecutedQuantity",
                                   .plural = "CumExecutedQuantities"};
};

struct OrderQuantity {
  using primary_type = simulator::Quantity;
  constexpr static core::Name name{.singular = "OrderQuantity",
                                   .plural = "OrderQuantities"};
};

struct LeavesQuantity {
  using primary_type = simulator::Quantity;
  constexpr static core::Name name{.singular = "LeavesQuantity",
                                   .plural = "LeavesQuantities"};
};

struct ExecutedQuantity {
  using primary_type = simulator::Quantity;
  constexpr static core::Name name{.singular = "ExecutedQuantity",
                                   .plural = "ExecutedQuantities"};
};

struct CurrentBidDepth {
  using primary_type = simulator::MarketDepth;
  constexpr static core::Name name{.singular = "CurrentBidDepth",
                                   .plural = "CurrentBidDepths"};
};

struct CurrentOfferDepth {
  using primary_type = simulator::MarketDepth;
  constexpr static core::Name name{.singular = "CurrentOfferDepth",
                                   .plural = "CurrentOfferDepths"};
};

struct AggressorSide {
  using primary_type = simulator::Side;
  constexpr static core::Name name{.singular = "AggressorSide",
                                   .plural = "AggressorSides"};
};

struct BaseCurrency {
  using primary_type = simulator::Currency;
  constexpr static core::Name name{.singular = "BaseCurrency",
                                   .plural = "BaseCurrencies"};
};

struct PriceCurrency {
  using primary_type = simulator::Currency;
  constexpr static core::Name name{.singular = "PriceCurrency",
                                   .plural = "PriceCurrencies"};
};

}  // namespace simulator::tag

SIMULATOR_DECLARE_ATTRIBUTE(simulator, OrderPrice, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, BestBidPrice, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, BestOfferPrice, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, ExecutionPrice, Derived);

SIMULATOR_DECLARE_ATTRIBUTE(simulator, CumExecutedQuantity, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, LeavesQuantity, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, OrderQuantity, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, ExecutedQuantity, Derived);

SIMULATOR_DECLARE_ATTRIBUTE(simulator, CurrentBidDepth, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, CurrentOfferDepth, Derived);

SIMULATOR_DECLARE_ATTRIBUTE(simulator, AggressorSide, Derived);

SIMULATOR_DECLARE_ATTRIBUTE(simulator, BaseCurrency, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator, PriceCurrency, Derived);

namespace simulator {

[[nodiscard]] inline auto convert_security_type_from_string(
    std::string_view value) noexcept -> std::optional<SecurityType> {
  const std::optional<core::enumerators::SecurityType> enumerator =
      core::enumerators::convert_security_type_from_string(value);
  return enumerator.has_value() ? std::make_optional<SecurityType>(*enumerator)
                                : std::nullopt;
}

[[nodiscard]] inline auto convert_party_role_from_string(
    std::string_view value) noexcept -> std::optional<PartyRole> {
  const std::optional<core::enumerators::PartyRole> enumerator =
      core::enumerators::convert_party_role_from_string(value);
  return enumerator.has_value() ? std::make_optional<PartyRole>(*enumerator)
                                : std::nullopt;
}

}  // namespace simulator

#endif  // SIMULATOR_CORE_DOMAIN_ATTRIBUTES_HPP_