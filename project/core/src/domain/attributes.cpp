#pragma GCC diagnostic push
// Bug in <fmt> or GCC, see https://github.com/fmtlib/fmt/issues/2708
//                      and https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88443
// Ignoring false-positive? -Wstringop-overflow warnings
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#include <fmt/format.h>
#pragma GCC diagnostic pop

#include "core/domain/attributes.hpp"

SIMULATOR_DEFINE_ATTRIBUTE(simulator, Price, Arithmetic);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, Quantity, Arithmetic);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, MarketDepth, Arithmetic);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, RequesterInstrumentId, Arithmetic);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, ShortSaleExemptionReason, Arithmetic);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, SeqNum, Arithmetic);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, ExecutionType);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, OrderStatus);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, OrderType);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, PartyIdSource);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, PartyRole);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, SecurityIdSource);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, SecurityType);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, Side);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, TimeInForce);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, MdEntryType);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, MdSubscriptionRequestType);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, MdRejectReason);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, MarketEntryAction);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, MarketDataUpdateType);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, TradingPhase);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, TradingStatus);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, RejectedMessageType);
SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(simulator, BusinessRejectReason);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, ExpireDate, LocalDate);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, ExpireTime, UtcTimestamp);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, MarketEntryTime, UtcTimestamp);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, BusinessRejectRefId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, ClientOrderId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, Currency, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, ExecutionId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, OrigClientOrderId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, PartyId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, RejectText, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, SecurityExchange, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, SecurityId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, Symbol, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, VenueOrderId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, MdRequestId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, MarketEntryId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, BuyerId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, SellerId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, SecurityStatusReqId, Literal);

SIMULATOR_DEFINE_ATTRIBUTE(simulator, OrderPrice, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, BestBidPrice, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, BestOfferPrice, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, ExecutionPrice, Derived);

SIMULATOR_DEFINE_ATTRIBUTE(simulator, CumExecutedQuantity, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, LeavesQuantity, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, OrderQuantity, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, ExecutedQuantity, Derived);

SIMULATOR_DEFINE_ATTRIBUTE(simulator, CurrentBidDepth, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, CurrentOfferDepth, Derived);

SIMULATOR_DEFINE_ATTRIBUTE(simulator, AggressorSide, Derived);

SIMULATOR_DEFINE_ATTRIBUTE(simulator, BaseCurrency, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator, PriceCurrency, Derived);