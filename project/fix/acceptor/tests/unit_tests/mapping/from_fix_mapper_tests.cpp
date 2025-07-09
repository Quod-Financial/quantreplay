#include <gmock/gmock.h>
#include <quickfix/FieldMap.h>
#include <quickfix/FieldTypes.h>
#include <quickfix/Fields.h>
#include <quickfix/fix50sp2/MarketDataRequest.h>
#include <quickfix/fix50sp2/NewOrderSingle.h>
#include <quickfix/fix50sp2/OrderCancelReplaceRequest.h>
#include <quickfix/fix50sp2/OrderCancelRequest.h>

#include <chrono>

#include "common/custom_fields.hpp"
#include "ih/mapping/from_fix_mapper.hpp"

namespace simulator::fix::acceptor::test {
namespace {

using namespace std::chrono_literals;
using namespace testing;  // NOLINT

template <typename FixMessageType, typename InternalMessage>
struct FromFixMapperFixture : public Test {
  template <typename FixFieldType>
  static auto set_field(const FixFieldType& fix_field,
                        FIX::FieldMap& destination) -> void {
    destination.setField(fix_field);
  }

  template <typename FixFieldType>
  auto set_field(const FixFieldType& fix_field) -> void {
    set_field(fix_field, fix_message);
  }

  static auto make_protocol_session() -> protocol::Session {
    return protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{"Sender"},
                               protocol::fix::TargetCompId{"Target"}}};
  }

  FixMessageType fix_message;
  InternalMessage internal_message{make_protocol_session()};
};

struct AcceptorFromFixInstrumentMapping
    : public FromFixMapperFixture<FIX50SP2::NewOrderSingle,
                                  protocol::OrderPlacementRequest> {};

TEST_F(AcceptorFromFixInstrumentMapping, MapsInstrumentSymbol) {
  set_field(FIX::Symbol{"AAPL"});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.instrument.symbol, Optional(Eq(Symbol{"AAPL"})));
}

TEST_F(AcceptorFromFixInstrumentMapping, MapsInstrumentSecurityType) {
  set_field(FIX::SecurityType{FIX::SecurityType_FX_FORWARD});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.instrument.security_type,
              Optional(Eq(SecurityType{SecurityType::Option::FxForward})));
}

TEST_F(AcceptorFromFixInstrumentMapping, MapsInstrumentSecurityExchange) {
  set_field(FIX::SecurityExchange{"XNYSE"});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.instrument.security_exchange,
              Optional(Eq(SecurityExchange{"XNYSE"})));
}

TEST_F(AcceptorFromFixInstrumentMapping, MapsInstrumentCurrency) {
  set_field(FIX::Currency{"USD"});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.instrument.currency,
              Optional(Eq(Currency{"USD"})));
}

TEST_F(AcceptorFromFixInstrumentMapping, MapsInstrumentSecurityIdentifier) {
  set_field(FIX::SecurityID{"US0378331005"});
  set_field(FIX::SecurityIDSource{FIX::SecurityIDSource_ISIN_NUMBER});

  FromFixMapper::map(fix_message, internal_message);

  EXPECT_THAT(internal_message.instrument.security_id,
              Optional(Eq(SecurityId{"US0378331005"})));

  EXPECT_THAT(internal_message.instrument.security_id_source,
              Optional(Eq(SecurityIdSource::Option::Isin)));
}

TEST_F(AcceptorFromFixInstrumentMapping, MapsPartyFromMessagePartiesList) {
  fix_message.addGroup([] {
    FIX50SP2::NewOrderSingle::NoPartyIDs party;
    set_field(FIX::PartyID{"IP"}, party);
    set_field(FIX::PartyRole{FIX::PartyRole_CLIENT_ID}, party);
    set_field(FIX::PartyIDSource{FIX::PartyIDSource_PROPRIETARY_CUSTOM_CODE},
              party);
    return party;
  }());

  FromFixMapper::map(fix_message, internal_message);

  EXPECT_THAT(
      internal_message.instrument.parties,
      ElementsAre(AllOf(
          Property(&Party::party_id, Eq(PartyId{"IP"})),
          Property(&Party::role, Eq(PartyRole::Option::ClientID)),
          Property(&Party::source, Eq(PartyIdSource::Option::Proprietary)))));
}

struct AcceptorFromFixPartiesMapping
    : public FromFixMapperFixture<FIX50SP2::NewOrderSingle,
                                  protocol::OrderPlacementRequest> {};

TEST_F(AcceptorFromFixPartiesMapping, IgnoresEmptyPartiesGroup) {
  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.parties, IsEmpty());
}

TEST_F(AcceptorFromFixPartiesMapping, MapsPartyFromFixMessage) {
  fix_message.addGroup([] {
    FIX50SP2::NewOrderSingle::NoPartyIDs party;
    set_field(FIX::PartyID{"Party1"}, party);
    set_field(FIX::PartyRole{FIX::PartyRole_CLIENT_ID}, party);
    set_field(FIX::PartyIDSource{FIX::PartyIDSource_PROPRIETARY_CUSTOM_CODE},
              party);
    return party;
  }());

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.parties, SizeIs(1));
  EXPECT_THAT(internal_message.parties[0].party_id(), Eq(PartyId{"Party1"}));
  EXPECT_THAT(internal_message.parties[0].source(),
              Eq(PartyIdSource::Option::Proprietary));
  EXPECT_THAT(internal_message.parties[0].role(),
              Eq(PartyRole::Option::ClientID));
}

TEST_F(AcceptorFromFixPartiesMapping, MapsMultiplePartiesFromFixMessage) {
  fix_message.addGroup([] {
    FIX50SP2::NewOrderSingle::NoPartyIDs party;
    set_field(FIX::PartyID{"P1"}, party);
    set_field(FIX::PartyRole{FIX::PartyRole_CLIENT_ID}, party);
    set_field(FIX::PartyIDSource{FIX::PartyIDSource_PROPRIETARY_CUSTOM_CODE},
              party);
    return party;
  }());

  fix_message.addGroup([] {
    FIX50SP2::NewOrderSingle::NoPartyIDs party;
    set_field(FIX::PartyID{"P2"}, party);
    set_field(FIX::PartyRole{FIX::PartyRole_CLIENT_ID}, party);
    set_field(FIX::PartyIDSource{FIX::PartyIDSource_PROPRIETARY_CUSTOM_CODE},
              party);
    return party;
  }());

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.parties, SizeIs(2));
}

struct AcceptorFromFixNewOrderSingleMapping
    : public FromFixMapperFixture<FIX50SP2::NewOrderSingle,
                                  protocol::OrderPlacementRequest> {};

TEST_F(AcceptorFromFixNewOrderSingleMapping, MapsClientOrderId) {
  set_field(FIX::ClOrdID{"client_order_id_"});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.client_order_id,
              Optional(Eq(ClientOrderId{"client_order_id_"})));
}

TEST_F(AcceptorFromFixNewOrderSingleMapping, MapsOrderType) {
  set_field(FIX::OrdType{FIX::OrdType_LIMIT});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.order_type,
              Optional(Eq(OrderType::Option::Limit)));
}

TEST_F(AcceptorFromFixNewOrderSingleMapping, MapsSide) {
  set_field(FIX::Side{FIX::Side_BUY});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(AcceptorFromFixNewOrderSingleMapping, MapsTimeInForce) {
  set_field(FIX::TimeInForce{FIX::TimeInForce_DAY});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.time_in_force,
              Optional(Eq(TimeInForce::Option::Day)));
}

TEST_F(AcceptorFromFixNewOrderSingleMapping, MapsOrderQuantity) {
  set_field(FIX::OrderQty{100});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.order_quantity,
              Optional(Eq(OrderQuantity{100})));
}

TEST_F(AcceptorFromFixNewOrderSingleMapping, MapsOrderPrice) {
  set_field(FIX::Price{120.0});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.order_price, Optional(Eq(OrderPrice{120.0})));
}

TEST_F(AcceptorFromFixNewOrderSingleMapping, MapsExpireTime) {
  // 2024-09-01 12:01:02.138567
  set_field(
      FIX::ExpireTime{FIX::UtcTimeStamp{12, 1, 2, 138567, 1, 9, 2024, 6}, 6});

  FromFixMapper::map(fix_message, internal_message);

  using sys_us = std::chrono::sys_time<std::chrono::microseconds>;
  ASSERT_THAT(
      internal_message.expire_time,
      Optional(Property(&ExpireTime::value, Eq(sys_us{1725192062138567us}))));
}

TEST_F(AcceptorFromFixNewOrderSingleMapping, MapsExpireDate) {
  set_field(FIX::ExpireDate{"20240901"});

  FromFixMapper::map(fix_message, internal_message);

  using local_days = std::chrono::local_days;
  ASSERT_THAT(internal_message.expire_date,
              Optional(Eq(ExpireDate{local_days{2024y / 9 / 1}})));
}

TEST_F(AcceptorFromFixNewOrderSingleMapping,
       MapsShortSaleExemptionReasonWhenSideIsSellShortExempt) {
  set_field(FIX::Side(FIX::Side_SELL_SHORT_EXEMPT));
  set_field(FIX::ShortSaleExemptionReason(0));

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.short_sale_exempt_reason,
              Optional(Eq(ShortSaleExemptionReason(0))));
}

struct AcceptorFromFixOrderCancelReplaceRequestMapping
    : public FromFixMapperFixture<FIX50SP2::OrderCancelReplaceRequest,
                                  protocol::OrderModificationRequest> {};

TEST_F(AcceptorFromFixOrderCancelReplaceRequestMapping, MapsClientOrderId) {
  set_field(FIX::ClOrdID{"client_order_id_"});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.client_order_id,
              Optional(Eq(ClientOrderId{"client_order_id_"})));
}

TEST_F(AcceptorFromFixOrderCancelReplaceRequestMapping, MapsOrigClientOrderId) {
  set_field(FIX::OrigClOrdID{"orig_client_order_id_"});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"orig_client_order_id_"})));
}

TEST_F(AcceptorFromFixOrderCancelReplaceRequestMapping, MapsVenueOrderId) {
  set_field(FIX::OrderID{"venue_order_id_"});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.venue_order_id,
              Optional(Eq(VenueOrderId{"venue_order_id_"})));
}

TEST_F(AcceptorFromFixOrderCancelReplaceRequestMapping, MapsOrderType) {
  set_field(FIX::OrdType{FIX::OrdType_LIMIT});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.order_type,
              Optional(Eq(OrderType::Option::Limit)));
}

TEST_F(AcceptorFromFixOrderCancelReplaceRequestMapping, MapsSide) {
  set_field(FIX::Side{FIX::Side_BUY});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(AcceptorFromFixOrderCancelReplaceRequestMapping, MapsTimeInForce) {
  set_field(FIX::TimeInForce{FIX::TimeInForce_DAY});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.time_in_force,
              Optional(Eq(TimeInForce::Option::Day)));
}

TEST_F(AcceptorFromFixOrderCancelReplaceRequestMapping, MapsOrderQuantity) {
  set_field(FIX::OrderQty{100});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.order_quantity,
              Optional(Eq(OrderQuantity{100})));
}

TEST_F(AcceptorFromFixOrderCancelReplaceRequestMapping, MapsOrderPrice) {
  set_field(FIX::Price{120.0});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.order_price, Optional(Eq(OrderPrice{120.0})));
}

TEST_F(AcceptorFromFixOrderCancelReplaceRequestMapping, MapsExpireTime) {
  // 2024-09-01 12:01:02.138567
  set_field(
      FIX::ExpireTime{FIX::UtcTimeStamp{12, 1, 2, 138567, 1, 9, 2024, 6}, 6});

  FromFixMapper::map(fix_message, internal_message);

  using sys_us = std::chrono::sys_time<std::chrono::microseconds>;
  ASSERT_THAT(
      internal_message.expire_time,
      Optional(Property(&ExpireTime::value, Eq(sys_us{1725192062138567us}))));
}

TEST_F(AcceptorFromFixOrderCancelReplaceRequestMapping, MapsExpireDate) {
  set_field(FIX::ExpireDate{"20240901"});

  FromFixMapper::map(fix_message, internal_message);

  using local_days = std::chrono::local_days;
  ASSERT_THAT(internal_message.expire_date,
              Optional(Eq(ExpireDate{local_days{2024y / 9 / 1}})));
}

TEST_F(AcceptorFromFixOrderCancelReplaceRequestMapping,
       MapsShortSaleExemptionReasonWhenSideIsSellShortExempt) {
  set_field(FIX::Side(FIX::Side_SELL_SHORT_EXEMPT));
  set_field(FIX::ShortSaleExemptionReason(0));

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.short_sale_exempt_reason,
              Optional(Eq(ShortSaleExemptionReason(0))));
}

struct AcceptorFromFixOrderCancelRequestMapping
    : public FromFixMapperFixture<FIX50SP2::OrderCancelRequest,
                                  protocol::OrderCancellationRequest> {};

TEST_F(AcceptorFromFixOrderCancelRequestMapping, MapsClientOrderId) {
  set_field(FIX::ClOrdID{"client_order_id_"});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.client_order_id,
              Optional(Eq(ClientOrderId{"client_order_id_"})));
}

TEST_F(AcceptorFromFixOrderCancelRequestMapping, MapsOrigClientOrderId) {
  set_field(FIX::OrigClOrdID{"orig_client_order_id_"});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"orig_client_order_id_"})));
}

TEST_F(AcceptorFromFixOrderCancelRequestMapping, MapsVenueOrderId) {
  set_field(FIX::OrderID{"venue_order_id_"});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.venue_order_id,
              Optional(Eq(VenueOrderId{"venue_order_id_"})));
}

TEST_F(AcceptorFromFixOrderCancelRequestMapping, MapsSide) {
  set_field(FIX::Side{FIX::Side_BUY});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.side, Optional(Eq(Side::Option::Buy)));
}

struct AcceptorFromFixMarketDataRequestMapping
    : public FromFixMapperFixture<FIX50SP2::MarketDataRequest,
                                  protocol::MarketDataRequest> {
 private:
  auto SetUp() -> void override {
    fix_message.set(FIX::NoRelatedSym(0));
    fix_message.set(FIX::NoMDEntryTypes(0));
  }
};

TEST_F(AcceptorFromFixMarketDataRequestMapping, MapsMdRequestId) {
  set_field(FIX::MDReqID{"request-id"});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.request_id,
              Optional(Eq(MdRequestId{"request-id"})));
}

TEST_F(AcceptorFromFixMarketDataRequestMapping, MapsSubscriptionRequestType) {
  set_field(FIX::SubscriptionRequestType{
      FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES});

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.request_type,
              Optional(Eq(MdSubscriptionRequestType::Option::Subscribe)));
}

TEST_F(AcceptorFromFixMarketDataRequestMapping, MapsMarketDataUpdateType) {
  set_field(FIX::MDUpdateType(FIX::MDUpdateType_INCREMENTAL_REFRESH));

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.update_type,
              Optional(Eq(MarketDataUpdateType::Option::Incremental)));
}

TEST_F(AcceptorFromFixMarketDataRequestMapping, MapsMarketDepth) {
  set_field(FIX::MarketDepth(1));

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.market_depth, Optional(Eq(MarketDepth{1})));
}

TEST_F(AcceptorFromFixMarketDataRequestMapping, MapsMarketDataEntriesTypes) {
  fix_message.addGroup([]() {
    FIX50SP2::MarketDataRequest::NoMDEntryTypes entry;
    set_field(FIX::MDEntryType(FIX::MDEntryType_BID), entry);
    return entry;
  }());

  fix_message.addGroup([]() {
    FIX50SP2::MarketDataRequest::NoMDEntryTypes entry;
    set_field(FIX::MDEntryType(FIX::MDEntryType_OFFER), entry);
    return entry;
  }());

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.market_data_types,
              ElementsAre(Eq(MdEntryType::Option::Bid),
                          Eq(MdEntryType::Option::Offer)));
}

TEST_F(AcceptorFromFixMarketDataRequestMapping, IgnoresUnknownMarketDataEntry) {
  fix_message.addGroup([]() {
    FIX50SP2::MarketDataRequest::NoMDEntryTypes entry;
    set_field(FIX::MDEntryType('e'), entry);
    return entry;
  }());

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.market_data_types, IsEmpty());
}

TEST_F(AcceptorFromFixMarketDataRequestMapping, MapsRequestedSymbols) {
  fix_message.addGroup([]() {
    FIX50SP2::MarketDataRequest::NoRelatedSym symbol;
    set_field(FIX::Symbol{"AAPL"}, symbol);
    return symbol;
  }());

  fix_message.addGroup([]() {
    FIX50SP2::MarketDataRequest::NoRelatedSym symbol;
    set_field(FIX::Symbol{"GOOGL"}, symbol);
    return symbol;
  }());

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(
      internal_message.instruments,
      ElementsAre(
          Field(&InstrumentDescriptor::symbol, Optional(Eq(Symbol{"AAPL"}))),
          Field(&InstrumentDescriptor::symbol, Optional(Eq(Symbol{"GOOGL"})))));
}

TEST_F(AcceptorFromFixMarketDataRequestMapping, MapsRequestParties) {
  fix_message.addGroup([] {
    FIX50SP2::NewOrderSingle::NoPartyIDs party;
    set_field(FIX::PartyID{"IP"}, party);
    set_field(FIX::PartyRole{FIX::PartyRole_CLIENT_ID}, party);
    set_field(FIX::PartyIDSource{FIX::PartyIDSource_PROPRIETARY_CUSTOM_CODE},
              party);
    return party;
  }());

  fix_message.addGroup([] {
    FIX50SP2::NewOrderSingle::NoPartyIDs party;
    set_field(FIX::PartyID{"P2"}, party);
    set_field(FIX::PartyRole{FIX::PartyRole_CLIENT_ID}, party);
    set_field(FIX::PartyIDSource{FIX::PartyIDSource_PROPRIETARY_CUSTOM_CODE},
              party);
    return party;
  }());

  FromFixMapper::map(fix_message, internal_message);

  ASSERT_THAT(internal_message.parties, SizeIs(2));
}

}  // namespace
}  // namespace simulator::fix::acceptor::test