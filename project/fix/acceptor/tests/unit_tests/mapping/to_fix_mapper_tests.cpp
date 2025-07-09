#include <gmock/gmock.h>
#include <quickfix/Fields.h>
#include <quickfix/FixValues.h>
#include <quickfix/Message.h>

#include <chrono>
#include <optional>

#include "common/custom_fields.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"
#include "ih/mapping/to_fix_mapper.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix::acceptor::test {
namespace {

using namespace std::chrono_literals;
using namespace testing;  // NOLINT

constexpr auto UtcTimestampMicrosecondsRegex =
    R"([0-9]{8}-[0-9]{2}:[0-9]{2}:[0-9]{2}\.[0-9]{3})";
constexpr auto UtcTimeOnlyMicrosecondsRegex =
    R"([0-9]{2}:[0-9]{2}:[0-9]{2}\.[0-9]{3})";

struct AcceptorToFixMapping : public Test {
  template <typename InternalMessageType>
  static auto make_internal_message() -> InternalMessageType {
    return InternalMessageType{protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{"Sender"},
                               protocol::fix::TargetCompId{"Target"}}}};
  }

  template <typename FixFieldType>
  static auto get_fix_field(const FIX::FieldMap& source)
      -> std::optional<FixFieldType> {
    FixFieldType fix_field;
    if (source.getFieldIfSet(fix_field)) {
      return fix_field;
    }
    return std::nullopt;
  }

  template <typename FixFieldType>
  static auto get_fix_field_string(const FIX::FieldMap& source)
      -> std::optional<std::string> {
    FixFieldType fix_field;
    if (source.getFieldIfSet(fix_field)) {
      return fix_field.getString();
    }
    return std::nullopt;
  }

  FIX::Message fix_message;
};

// NOLINTBEGIN(*-magic-numbers,*-array-to-pointer-decay)

// region InstrumentDescriptor mapping

struct AcceptorToFixInstrumentMapping : public AcceptorToFixMapping {};

TEST_F(AcceptorToFixInstrumentMapping, MapsSecurityIdentifier) {
  InstrumentDescriptor instrument;
  instrument.security_id = SecurityId{"SecurityID"};
  instrument.security_id_source = SecurityIdSource::Option::Isin;

  ToFixMapper::map_instrument(instrument, fix_message);

  EXPECT_THAT(get_fix_field<FIX::SecurityID>(fix_message),
              Optional(Eq("SecurityID")));
  EXPECT_THAT(get_fix_field<FIX::SecurityIDSource>(fix_message),
              Optional(Eq(FIX::SecurityIDSource_ISIN_NUMBER)));
}

TEST_F(AcceptorToFixInstrumentMapping, MapsSymbol) {
  InstrumentDescriptor instrument;
  instrument.symbol = Symbol{"Symbol"};

  ToFixMapper::map_instrument(instrument, fix_message);

  ASSERT_THAT(get_fix_field<FIX::Symbol>(fix_message), Optional(Eq("Symbol")));
}

TEST_F(AcceptorToFixInstrumentMapping, MapsSecurityType) {
  InstrumentDescriptor instrument;
  instrument.security_type = SecurityType::Option::CommonStock;

  ToFixMapper::map_instrument(instrument, fix_message);

  ASSERT_THAT(get_fix_field<FIX::SecurityType>(fix_message),
              Optional(Eq(FIX::SecurityType_COMMON_STOCK)));
}

TEST_F(AcceptorToFixInstrumentMapping, MapsSecurityExchange) {
  InstrumentDescriptor instrument;
  instrument.security_exchange = SecurityExchange{"SecurityExchange"};

  ToFixMapper::map_instrument(instrument, fix_message);

  ASSERT_THAT(get_fix_field<FIX::SecurityExchange>(fix_message),
              Optional(Eq("SecurityExchange")));
}

TEST_F(AcceptorToFixInstrumentMapping, MapsCurrency) {
  InstrumentDescriptor instrument;
  instrument.currency = Currency{"Currency"};

  ToFixMapper::map_instrument(instrument, fix_message);

  ASSERT_THAT(get_fix_field<FIX::Currency>(fix_message),
              Optional(Eq("Currency")));
}

// endregion InstrumentDescriptor mapping

// region Parties mapping

struct AcceptorToFixPartiesMapping : public AcceptorToFixMapping {};

TEST_F(AcceptorToFixPartiesMapping, IgnoresEmptyParties) {
  const std::vector<Party> parties;

  ToFixMapper::map_parties(parties, fix_message);

  ASSERT_THAT(fix_message.groupCount(FIX::FIELD::NoPartyIDs), Eq(0));
}

TEST_F(AcceptorToFixPartiesMapping, MapsSingleParty) {
  const std::vector<Party> parties{Party{PartyId{"PartyID"},
                                         PartyIdSource::Option::Proprietary,
                                         PartyRole::Option::ExecutingFirm}};

  ToFixMapper::map_parties(parties, fix_message);

  ASSERT_THAT(fix_message.groupCount(FIX::FIELD::NoPartyIDs), Eq(1));

  auto& party_group = fix_message.getGroupRef(1, FIX::FIELD::NoPartyIDs);
  EXPECT_THAT(get_fix_field<FIX::PartyID>(party_group),
              Optional(Eq("PartyID")));
  EXPECT_THAT(get_fix_field<FIX::PartyIDSource>(party_group),
              Optional(Eq(FIX::PartyIDSource_PROPRIETARY)));
  EXPECT_THAT(get_fix_field<FIX::PartyRole>(party_group),
              Optional(Eq(FIX::PartyRole_EXECUTING_FIRM)));
}

TEST_F(AcceptorToFixPartiesMapping, MapsMultipleParties) {
  const std::vector<Party> parties{Party{PartyId{"PartyID1"},
                                         PartyIdSource::Option::Proprietary,
                                         PartyRole::Option::ExecutingFirm},
                                   Party{PartyId{"PartyID2"},
                                         PartyIdSource::Option::Proprietary,
                                         PartyRole::Option::ClientID}};

  ToFixMapper::map_parties(parties, fix_message);

  ASSERT_THAT(fix_message.groupCount(FIX::FIELD::NoPartyIDs), Eq(2));
}

// endregion Parties mapping

// region ExecutionReport mapping

struct AcceptorToFixExecutionReportMapping : public AcceptorToFixMapping {
  protocol::ExecutionReport reply =
      make_internal_message<protocol::ExecutionReport>();
};

TEST_F(AcceptorToFixExecutionReportMapping, SetsExecutionReportMessageType) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::MsgType>(fix_message.getHeader()),
              Optional(Eq(FIX::MsgType_ExecutionReport)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsExecutionType) {
  reply.execution_type = ExecutionType::Option::OrderTraded;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExecType>(fix_message),
              Optional(Eq(FIX::ExecType_TRADE)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsOrderStatus) {
  reply.order_status = OrderStatus::Option::PartiallyFilled;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdStatus>(fix_message),
              Optional(Eq(FIX::OrdStatus_PARTIALLY_FILLED)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsClientOrderId) {
  reply.client_order_id = ClientOrderId{"ClientOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ClOrdID>(fix_message),
              Optional(Eq("ClientOrderId")));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsVenueOrderId) {
  reply.venue_order_id = VenueOrderId{"VenueOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrderID>(fix_message),
              Optional(Eq("VenueOrderId")));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsExecutionId) {
  reply.execution_id = ExecutionId{"ExecutionId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExecID>(fix_message),
              Optional(Eq("ExecutionId")));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsOrderPrice) {
  reply.order_price = OrderPrice{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Price>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsExecutionPrice) {
  reply.execution_price = ExecutionPrice{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::LastPx>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsLeavesQuantity) {
  reply.leaves_quantity = LeavesQuantity{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::LeavesQty>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsCumExecutedQuantity) {
  reply.cum_executed_quantity = CumExecutedQuantity{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::CumQty>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsExecutedQuantity) {
  reply.executed_quantity = ExecutedQuantity{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::LastQty>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsOrderType) {
  reply.order_type = OrderType::Option::Limit;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdType>(fix_message),
              Optional(Eq(FIX::OrdType_LIMIT)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsSide) {
  reply.side = Side::Option::Buy;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Side>(fix_message),
              Optional(Eq(FIX::Side_BUY)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsTimeInForce) {
  reply.time_in_force = TimeInForce::Option::ImmediateOrCancel;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::TimeInForce>(fix_message),
              Optional(Eq(FIX::TimeInForce_IMMEDIATE_OR_CANCEL)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsShortSaleExemptionReason) {
  reply.short_sale_exempt_reason = ShortSaleExemptionReason(0);

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ShortSaleExemptionReason>(fix_message),
              Optional(Eq(0)));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsExpireDate) {
  reply.expire_date = ExpireDate(2020y / 1 / 1);

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExpireDate>(fix_message), Ne(std::nullopt));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsExpireTimeSeconds) {
  reply.expire_time = ExpireTime(
      std::chrono::sys_time<std::chrono::microseconds>(1724139893999999us));

  ToFixMapper::map(
      reply, fix_message, {.timestamp_precision = TimestampPrecision::Seconds});

  ASSERT_THAT(
      get_fix_field<FIX::ExpireTime>(fix_message),
      Optional(FIX::ExpireTime{{7, 44, 53, 999999, 20, 8, 2024, 6}, 0}));
}

TEST_F(AcceptorToFixExecutionReportMapping, MapsExpireTimeMilliseconds) {
  reply.expire_time = ExpireTime(
      std::chrono::sys_time<std::chrono::microseconds>(1724139893123456us));

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(
      get_fix_field<FIX::ExpireTime>(fix_message),
      Optional(FIX::ExpireTime{{7, 44, 53, 123456, 20, 8, 2024, 6}, 3}));
}

// endregion ExecutionReport mapping

// region OrderPlacementConfirmation mapping

struct AcceptorToFixOrderPlacementConfirmationMapping
    : public AcceptorToFixMapping {
  protocol::OrderPlacementConfirmation reply =
      make_internal_message<protocol::OrderPlacementConfirmation>();
};

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping,
       SetsExecutionReportMessageType) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::MsgType>(fix_message.getHeader()),
              Optional(Eq(FIX::MsgType_ExecutionReport)));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping, SetsExecTypeNew) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExecType>(fix_message),
              Optional(Eq(FIX::ExecType_NEW)));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping, MapsClientOrderId) {
  reply.client_order_id = ClientOrderId{"ClientOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ClOrdID>(fix_message),
              Optional(Eq("ClientOrderId")));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping, MapsVenueOrderId) {
  reply.venue_order_id = VenueOrderId{"VenueOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrderID>(fix_message),
              Optional(Eq("VenueOrderId")));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping, MapsExecutionId) {
  reply.execution_id = ExecutionId{"ExecutionId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExecID>(fix_message),
              Optional(Eq("ExecutionId")));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping,
       MapsLeavesQuantityAsFullOrderQuantity) {
  reply.order_quantity = OrderQuantity{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::LeavesQty>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping,
       SetsZeroCumExecutedQuantity) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::CumQty>(fix_message), Optional(Eq(0)));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping, SetsNewOrderStatus) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdStatus>(fix_message),
              Optional(Eq(FIX::OrdStatus_NEW)));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping, MapsOrderPrice) {
  reply.order_price = OrderPrice{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Price>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping, MapsOrderType) {
  reply.order_type = OrderType::Option::Limit;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdType>(fix_message),
              Optional(Eq(FIX::OrdType_LIMIT)));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping, MapsSide) {
  reply.side = Side::Option::Buy;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Side>(fix_message),
              Optional(Eq(FIX::Side_BUY)));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping, MapsTimeInForce) {
  reply.time_in_force = TimeInForce::Option::ImmediateOrCancel;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::TimeInForce>(fix_message),
              Optional(Eq(FIX::TimeInForce_IMMEDIATE_OR_CANCEL)));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping,
       MapsShortSaleExemptionReason) {
  reply.short_sale_exempt_reason = ShortSaleExemptionReason(0);

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ShortSaleExemptionReason>(fix_message),
              Optional(Eq(0)));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping, MapsExpireDate) {
  reply.expire_date = ExpireDate(2020y / 1 / 1);

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExpireDate>(fix_message), Ne(std::nullopt));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping, MapsExpireTimeSeconds) {
  reply.expire_time = ExpireTime(
      std::chrono::sys_time<std::chrono::microseconds>(1724139893999999us));

  ToFixMapper::map(
      reply, fix_message, {.timestamp_precision = TimestampPrecision::Seconds});

  ASSERT_THAT(
      get_fix_field<FIX::ExpireTime>(fix_message),
      Optional(FIX::ExpireTime{{7, 44, 53, 999999, 20, 8, 2024, 6}, 0}));
}

TEST_F(AcceptorToFixOrderPlacementConfirmationMapping,
       MapsExpireTimeMilliseconds) {
  reply.expire_time = ExpireTime(
      std::chrono::sys_time<std::chrono::microseconds>(1724139893123456us));

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(
      get_fix_field<FIX::ExpireTime>(fix_message),
      Optional(FIX::ExpireTime{{7, 44, 53, 123456, 20, 8, 2024, 6}, 3}));
}

// endregion OrderPlacementConfirmation mapping

// region OrderPlacementReject mapping

struct AcceptorToFixOrderPlacementRejectMapping : public AcceptorToFixMapping {
  protocol::OrderPlacementReject reply =
      make_internal_message<protocol::OrderPlacementReject>();
};

TEST_F(AcceptorToFixOrderPlacementRejectMapping,
       SetsExecutionReportMessageType) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::MsgType>(fix_message.getHeader()),
              Optional(Eq(FIX::MsgType_ExecutionReport)));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, SetsExecTypeRejected) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExecType>(fix_message),
              Optional(Eq(FIX::ExecType_REJECTED)));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, SetsOrderStatusToRejected) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdStatus>(fix_message),
              Optional(Eq(FIX::OrdStatus_REJECTED)));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsClientOrderId) {
  reply.client_order_id = ClientOrderId{"ClientOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ClOrdID>(fix_message),
              Optional(Eq("ClientOrderId")));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsVenueOrderId) {
  reply.venue_order_id = VenueOrderId{"VenueOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrderID>(fix_message),
              Optional(Eq("VenueOrderId")));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsExecutionId) {
  reply.execution_id = ExecutionId{"ExecutionId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExecID>(fix_message),
              Optional(Eq("ExecutionId")));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsOrderQuantityToLeavesQty) {
  reply.order_quantity = OrderQuantity{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::LeavesQty>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, SetsCumQtyToZero) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::CumQty>(fix_message), Optional(Eq(0)));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsOrderPrice) {
  reply.order_price = OrderPrice{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Price>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsOrderType) {
  reply.order_type = OrderType::Option::Limit;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdType>(fix_message),
              Optional(Eq(FIX::OrdType_LIMIT)));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsSide) {
  reply.side = Side::Option::Buy;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Side>(fix_message),
              Optional(Eq(FIX::Side_BUY)));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsTimeInForce) {
  reply.time_in_force = TimeInForce::Option::ImmediateOrCancel;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::TimeInForce>(fix_message),
              Optional(Eq(FIX::TimeInForce_IMMEDIATE_OR_CANCEL)));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsRejectText) {
  reply.reject_text = RejectText{"RejectText"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Text>(fix_message),
              Optional(Eq("RejectText")));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsShortSaleExemptionReason) {
  reply.short_sale_exempt_reason = ShortSaleExemptionReason(0);

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ShortSaleExemptionReason>(fix_message),
              Optional(Eq(0)));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsExpireDate) {
  reply.expire_date = ExpireDate(2020y / 1 / 1);

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExpireDate>(fix_message), Ne(std::nullopt));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsExpireTimeSeconds) {
  reply.expire_time = ExpireTime(
      std::chrono::sys_time<std::chrono::microseconds>(1724139893999999us));

  ToFixMapper::map(
      reply, fix_message, {.timestamp_precision = TimestampPrecision::Seconds});

  ASSERT_THAT(
      get_fix_field<FIX::ExpireTime>(fix_message),
      Optional(FIX::ExpireTime{{7, 44, 53, 999999, 20, 8, 2024, 6}, 0}));
}

TEST_F(AcceptorToFixOrderPlacementRejectMapping, MapsExpireTimeMilliseconds) {
  reply.expire_time = ExpireTime(
      std::chrono::sys_time<std::chrono::microseconds>(1724139893123456us));

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(
      get_fix_field<FIX::ExpireTime>(fix_message),
      Optional(FIX::ExpireTime{{7, 44, 53, 123456, 20, 8, 2024, 6}, 3}));
}

// endregion OrderPlacementReject mapping

// region OrderModificationConfirmation mapping

struct AcceptorToFixOrderModificationConfirmationMapping
    : public AcceptorToFixMapping {
  protocol::OrderModificationConfirmation reply =
      make_internal_message<protocol::OrderModificationConfirmation>();
};

TEST_F(AcceptorToFixOrderModificationConfirmationMapping,
       SetsExecutionReportMessageType) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::MsgType>(fix_message.getHeader()),
              Optional(Eq(FIX::MsgType_ExecutionReport)));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping,
       SetsExecTypeReplaced) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExecType>(fix_message),
              Optional(Eq(FIX::ExecType_REPLACED)));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping, MapsClientOrderId) {
  reply.client_order_id = ClientOrderId{"ClientOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ClOrdID>(fix_message),
              Optional(Eq("ClientOrderId")));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping,
       MapsOrigClientOrderId) {
  reply.orig_client_order_id = OrigClientOrderId{"OrigClientOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrigClOrdID>(fix_message),
              Optional(Eq("OrigClientOrderId")));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping, MapsVenueOrderId) {
  reply.venue_order_id = VenueOrderId{"VenueOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrderID>(fix_message),
              Optional(Eq("VenueOrderId")));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping, MapsExecutionId) {
  reply.execution_id = ExecutionId{"ExecutionId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExecID>(fix_message),
              Optional(Eq("ExecutionId")));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping, MapsOrderPrice) {
  reply.order_price = OrderPrice{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Price>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping, MapsLeavesQuantity) {
  reply.leaving_quantity = LeavesQuantity{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::LeavesQty>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping,
       MapsCumExecutedQuantity) {
  reply.cum_executed_quantity = CumExecutedQuantity{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::CumQty>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping, MapsOrderStatus) {
  reply.order_status = OrderStatus::Option::Modified;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdStatus>(fix_message),
              Optional(Eq(FIX::OrdStatus_REPLACED)));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping, MapsOrderType) {
  reply.order_type = OrderType::Option::Limit;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdType>(fix_message),
              Optional(Eq(FIX::OrdType_LIMIT)));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping, MapsSide) {
  reply.side = Side::Option::Buy;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Side>(fix_message),
              Optional(Eq(FIX::Side_BUY)));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping, MapsTimeInForce) {
  reply.time_in_force = TimeInForce::Option::ImmediateOrCancel;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::TimeInForce>(fix_message),
              Optional(Eq(FIX::TimeInForce_IMMEDIATE_OR_CANCEL)));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping,
       MapsShortSaleExemptionReason) {
  reply.short_sale_exempt_reason = ShortSaleExemptionReason(0);

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ShortSaleExemptionReason>(fix_message),
              Optional(Eq(0)));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping, MapsExpireDate) {
  reply.expire_date = ExpireDate(2020y / 1 / 1);

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExpireDate>(fix_message), Ne(std::nullopt));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping,
       MapsExpireTimeSeconds) {
  reply.expire_time = ExpireTime(
      std::chrono::sys_time<std::chrono::microseconds>(1724139893999999us));

  ToFixMapper::map(
      reply, fix_message, {.timestamp_precision = TimestampPrecision::Seconds});

  ASSERT_THAT(
      get_fix_field<FIX::ExpireTime>(fix_message),
      Optional(FIX::ExpireTime{{7, 44, 53, 999999, 20, 8, 2024, 6}, 0}));
}

TEST_F(AcceptorToFixOrderModificationConfirmationMapping,
       MapsExpireTimeMilliseconds) {
  reply.expire_time = ExpireTime(
      std::chrono::sys_time<std::chrono::microseconds>(1724139893123456us));

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(
      get_fix_field<FIX::ExpireTime>(fix_message),
      Optional(FIX::ExpireTime{{7, 44, 53, 123456, 20, 8, 2024, 6}, 3}));
}

// endregion OrderModificationConfirmation mapping

// region OrderModificationReject mapping

struct AcceptorToFixOrderModificationRejectMapping
    : public AcceptorToFixMapping {
  protocol::OrderModificationReject reply =
      make_internal_message<protocol::OrderModificationReject>();
};

TEST_F(AcceptorToFixOrderModificationRejectMapping,
       SetsOrderCancelRejectMessageType) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::MsgType>(fix_message.getHeader()),
              Optional(Eq(FIX::MsgType_OrderCancelReject)));
}

TEST_F(AcceptorToFixOrderModificationRejectMapping,
       SetsCxlRejResponseToOrderCancelReplaceRequest) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::CxlRejResponseTo>(fix_message),
              Optional(Eq(FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST)));
}

TEST_F(AcceptorToFixOrderModificationRejectMapping, MapsClientOrderId) {
  reply.client_order_id = ClientOrderId{"ClientOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ClOrdID>(fix_message),
              Optional(Eq("ClientOrderId")));
}

TEST_F(AcceptorToFixOrderModificationRejectMapping, MapsOrigClientOrderId) {
  reply.orig_client_order_id = OrigClientOrderId{"OrigClientOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrigClOrdID>(fix_message),
              Optional(Eq("OrigClientOrderId")));
}

TEST_F(AcceptorToFixOrderModificationRejectMapping, MapsVenueOrderId) {
  reply.venue_order_id = VenueOrderId{"VenueOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrderID>(fix_message),
              Optional(Eq("VenueOrderId")));
}

TEST_F(AcceptorToFixOrderModificationRejectMapping, MapsRejectText) {
  reply.reject_text = RejectText{"RejectText"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Text>(fix_message),
              Optional(Eq("RejectText")));
}

TEST_F(AcceptorToFixOrderModificationRejectMapping, MapsOrderStatus) {
  reply.order_status = OrderStatus::Option::PartiallyFilled;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdStatus>(fix_message),
              Optional(Eq(FIX::OrdStatus_PARTIALLY_FILLED)));
}

// endregion OrderModificationReject mapping

// region OrderCancellationConfirmation mapping

struct AcceptorToFixOrderCancellationConfirmationMapping
    : public AcceptorToFixMapping {
  protocol::OrderCancellationConfirmation reply =
      make_internal_message<protocol::OrderCancellationConfirmation>();
};

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping,
       SetsExecutionReportMessageType) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::MsgType>(fix_message.getHeader()),
              Optional(Eq(FIX::MsgType_ExecutionReport)));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping,
       SetsExecTypeCanceled) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExecType>(fix_message),
              Optional(Eq(FIX::ExecType_CANCELED)));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping, MapsClientOrderId) {
  reply.client_order_id = ClientOrderId{"ClientOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ClOrdID>(fix_message),
              Optional(Eq("ClientOrderId")));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping,
       MapsOrigClientOrderId) {
  reply.orig_client_order_id = OrigClientOrderId{"OrigClientOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrigClOrdID>(fix_message),
              Optional(Eq("OrigClientOrderId")));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping, MapsVenueOrderId) {
  reply.venue_order_id = VenueOrderId{"VenueOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrderID>(fix_message),
              Optional(Eq("VenueOrderId")));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping, MapsExecutionId) {
  reply.execution_id = ExecutionId{"ExecutionId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExecID>(fix_message),
              Optional(Eq("ExecutionId")));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping, MapsLeavesQuantity) {
  reply.leaving_quantity = LeavesQuantity{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::LeavesQty>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping,
       MapsCumExecutedQuantity) {
  reply.cum_executed_quantity = CumExecutedQuantity{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::CumQty>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping, MapsOrderPrice) {
  reply.order_price = OrderPrice{42};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Price>(fix_message), Optional(Eq(42.0)));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping, MapsOrderStatus) {
  reply.order_status = OrderStatus::Option::Cancelled;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdStatus>(fix_message),
              Optional(Eq(FIX::OrdStatus_CANCELED)));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping, MapsOrderType) {
  reply.order_type = OrderType::Option::Limit;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdType>(fix_message),
              Optional(Eq(FIX::OrdType_LIMIT)));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping, MapsSide) {
  reply.side = Side::Option::Buy;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Side>(fix_message),
              Optional(Eq(FIX::Side_BUY)));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping, MapsTimeInForce) {
  reply.time_in_force = TimeInForce::Option::ImmediateOrCancel;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::TimeInForce>(fix_message),
              Optional(Eq(FIX::TimeInForce_IMMEDIATE_OR_CANCEL)));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping,
       MapsShortSaleExemptionReason) {
  reply.short_sale_exempt_reason = ShortSaleExemptionReason(0);

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ShortSaleExemptionReason>(fix_message),
              Optional(Eq(0)));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping, MapsExpireDate) {
  reply.expire_date = ExpireDate(2020y / 1 / 1);

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ExpireDate>(fix_message), Ne(std::nullopt));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping,
       MapsExpireTimeSeconds) {
  reply.expire_time = ExpireTime(
      std::chrono::sys_time<std::chrono::microseconds>(1724139893999999us));

  ToFixMapper::map(
      reply, fix_message, {.timestamp_precision = TimestampPrecision::Seconds});

  ASSERT_THAT(
      get_fix_field<FIX::ExpireTime>(fix_message),
      Optional(FIX::ExpireTime{{7, 44, 53, 999999, 20, 8, 2024, 6}, 0}));
}

TEST_F(AcceptorToFixOrderCancellationConfirmationMapping,
       MapsExpireTimeMilliseconds) {
  reply.expire_time = ExpireTime(
      std::chrono::sys_time<std::chrono::microseconds>(1724139893123456us));

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(
      get_fix_field<FIX::ExpireTime>(fix_message),
      Optional(FIX::ExpireTime{{7, 44, 53, 123456, 20, 8, 2024, 6}, 3}));
}

// endregion OrderCancellationConfirmation mapping

// region OrderCancellationReject mapping

struct AcceptorToFixOrderCancellationRejectMapping
    : public AcceptorToFixMapping {
  protocol::OrderCancellationReject reply =
      make_internal_message<protocol::OrderCancellationReject>();
};

TEST_F(AcceptorToFixOrderCancellationRejectMapping,
       SetsOrderCancelRejectMessageType) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::MsgType>(fix_message.getHeader()),
              Optional(Eq(FIX::MsgType_OrderCancelReject)));
}

TEST_F(AcceptorToFixOrderCancellationRejectMapping,
       SetsCxlRejResponseToOrderCancelRequest) {
  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::CxlRejResponseTo>(fix_message),
              Optional(Eq(FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST)));
}

TEST_F(AcceptorToFixOrderCancellationRejectMapping, MapsClientOrderId) {
  reply.client_order_id = ClientOrderId{"ClientOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::ClOrdID>(fix_message),
              Optional(Eq("ClientOrderId")));
}

TEST_F(AcceptorToFixOrderCancellationRejectMapping, MapsOrigClientOrderId) {
  reply.orig_client_order_id = OrigClientOrderId{"OrigClientOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrigClOrdID>(fix_message),
              Optional(Eq("OrigClientOrderId")));
}

TEST_F(AcceptorToFixOrderCancellationRejectMapping, MapsVenueOrderId) {
  reply.venue_order_id = VenueOrderId{"VenueOrderId"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrderID>(fix_message),
              Optional(Eq("VenueOrderId")));
}

TEST_F(AcceptorToFixOrderCancellationRejectMapping, MapsRejectText) {
  reply.reject_text = RejectText{"RejectText"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Text>(fix_message),
              Optional(Eq("RejectText")));
}

TEST_F(AcceptorToFixOrderCancellationRejectMapping, MapsOrderStatus) {
  reply.order_status = OrderStatus::Option::Rejected;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::OrdStatus>(fix_message),
              Optional(Eq(FIX::OrdStatus_REJECTED)));
}

// endregion OrderCancellationReject mapping

// region MarketDataReject mapping

struct AcceptorToFixMarketDataRejectMapping : public AcceptorToFixMapping {
  protocol::MarketDataReject reply =
      make_internal_message<protocol::MarketDataReject>();
};

TEST_F(AcceptorToFixMarketDataRejectMapping, MapsMdRequestId) {
  reply.request_id = MdRequestId{"request-id"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::MDReqID>(fix_message),
              Optional(Eq("request-id")));
}

TEST_F(AcceptorToFixMarketDataRejectMapping, MapsMdRejectReason) {
  reply.reject_reason = MdRejectReason::Option::UnknownSymbol;

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::MDReqRejReason>(fix_message),
              Optional(Eq(FIX::MDReqRejReason_UNKNOWN_SYMBOL)));
}

TEST_F(AcceptorToFixMarketDataRejectMapping, MapsRejectText) {
  reply.reject_text = RejectText{"test error"};

  ToFixMapper::map(reply, fix_message, {});

  ASSERT_THAT(get_fix_field<FIX::Text>(fix_message),
              Optional(Eq("test error")));
}

// endregion MarketDataReject mapping

// region MarketDataSnapshot mapping

struct AcceptorToFixMarketDataSnapshotMapping : public AcceptorToFixMapping {
  protocol::MarketDataSnapshot reply =
      make_internal_message<protocol::MarketDataSnapshot>();
};

TEST_F(AcceptorToFixMarketDataSnapshotMapping, MapsCommonFields) {
  reply.request_id = MdRequestId{"request-id"};
  reply.instrument.symbol = Symbol{"AAPL"};

  ToFixMapper::map(reply, fix_message, {});

  EXPECT_THAT(get_fix_field<FIX::Symbol>(fix_message), Optional(Eq("AAPL")));
  EXPECT_THAT(get_fix_field<FIX::NoMDEntries>(fix_message), Optional(Eq(0)));
  EXPECT_THAT(get_fix_field_string<FIX::LastUpdateTime>(fix_message),
              Optional(MatchesRegex(UtcTimestampMicrosecondsRegex)));
  EXPECT_THAT(get_fix_field<FIX::MDReqID>(fix_message),
              Optional(Eq("request-id")));
}

TEST_F(AcceptorToFixMarketDataSnapshotMapping,
       MapsCommonMarketDataEntryAttributes) {
  reply.market_data_entries.emplace_back([] {
    MarketDataEntry entry;
    entry.id = MarketEntryId("entry-id");
    entry.type = MdEntryType::Option::Bid;
    entry.price = Price(42);
    entry.quantity = Quantity(100);
    return entry;
  }());

  ToFixMapper::map(reply, fix_message, {});
  ASSERT_THAT(get_fix_field<FIX::NoMDEntries>(fix_message), Optional(Eq(1)));

  const auto& entry = fix_message.getGroupRef(1, FIX::FIELD::NoMDEntries);
  EXPECT_THAT(get_fix_field<FIX::MDEntryPx>(entry), Optional(Eq(42.0)));
  EXPECT_THAT(get_fix_field<FIX::MDEntrySize>(entry), Optional(Eq(100.0)));
  EXPECT_THAT(get_fix_field<FIX::MDEntryID>(entry), Eq("entry-id"));
  EXPECT_THAT(get_fix_field<FIX::MDEntryType>(entry),
              Optional(Eq(FIX::MDEntryType_BID)));
}

TEST_F(AcceptorToFixMarketDataSnapshotMapping,
       MapsTradeSpecificMarketDataEntryAttributes) {
  reply.market_data_entries.emplace_back([] {
    MarketDataEntry entry;
    entry.buyer_id = BuyerId("buyer-id");
    entry.seller_id = SellerId("seller-id");
    entry.aggressor_side = AggressorSide::Option::Sell;
    entry.time = MarketEntryTime(core::get_current_system_time());
    return entry;
  }());

  ToFixMapper::map(reply, fix_message, {});
  ASSERT_THAT(get_fix_field<FIX::NoMDEntries>(fix_message), Optional(Eq(1)));

  auto& entry = fix_message.getGroupRef(1, FIX::FIELD::NoMDEntries);
  EXPECT_THAT(get_fix_field_string<FIX::MDEntryTime>(entry),
              Optional(MatchesRegex(UtcTimeOnlyMicrosecondsRegex)));
  EXPECT_THAT(entry.isSetField(FIX::FIELD::MDEntryDate), IsTrue());
  EXPECT_THAT(get_fix_field<FIX::MDEntryBuyer>(entry),
              Optional(Eq("buyer-id")));
  EXPECT_THAT(get_fix_field<FIX::MDEntrySeller>(entry),
              Optional(Eq("seller-id")));
  EXPECT_THAT(get_fix_field<FIX::AggressorSide>(entry),
              Optional(Eq(FIX::Side_SELL)));
}

TEST_F(AcceptorToFixMarketDataSnapshotMapping,
       MapsMarketPhaseFromMarketDataEntry) {
  reply.market_data_entries.emplace_back([] {
    MarketDataEntry entry;
    entry.phase = {TradingPhase::Option::Open, TradingStatus::Option::Halt};
    return entry;
  }());

  ToFixMapper::map(reply, fix_message, {});
  ASSERT_THAT(get_fix_field<FIX::NoMDEntries>(fix_message), Optional(Eq(1)));

  auto& entry = fix_message.getGroupRef(1, FIX::FIELD::NoMDEntries);
  EXPECT_THAT(get_fix_field<FIX::TradingSessionSubID>(entry),
              Optional(Eq(FIX::TradingSessionSubID_3)));
  EXPECT_THAT(get_fix_field<FIX::SecurityTradingStatus>(entry),
              Optional(Eq(FIX::SecurityTradingStatus_TRADING_HALT)));
}

TEST_F(AcceptorToFixMarketDataSnapshotMapping,
       HardcodesTradingSessionIdWhenMarketPhasePresentInMarketEntry) {
  reply.market_data_entries.emplace_back([] {
    MarketDataEntry entry;
    entry.phase = {TradingPhase::Option::Open, TradingStatus::Option::Halt};
    return entry;
  }());

  ToFixMapper::map(reply, fix_message, {});
  ASSERT_THAT(get_fix_field<FIX::NoMDEntries>(fix_message), Optional(Eq(1)));

  auto& entry = fix_message.getGroupRef(1, FIX::FIELD::NoMDEntries);
  EXPECT_THAT(get_fix_field<FIX::TradingSessionID>(entry),
              Optional(Eq(FIX::TradingSessionID_DAY)));
}

// endregion MarketDataSnapshot mapping

// region MarketDataUpdate mapping

struct AcceptorToFixMarketDataUpdateMapping : public AcceptorToFixMapping {
  protocol::MarketDataUpdate reply =
      make_internal_message<protocol::MarketDataUpdate>();
};

TEST_F(AcceptorToFixMarketDataUpdateMapping, MapsCommonFields) {
  reply.request_id = MdRequestId{"request-id"};

  ToFixMapper::map(reply, fix_message, {});

  EXPECT_THAT(get_fix_field<FIX::NoMDEntries>(fix_message), Optional(Eq(0)));
  EXPECT_THAT(get_fix_field<FIX::MDReqID>(fix_message),
              Optional(Eq("request-id")));
}

TEST_F(AcceptorToFixMarketDataUpdateMapping,
       MapsCommonMarketDataEntryAttributes) {
  reply.market_data_entries.emplace_back([] {
    MarketDataEntry entry;
    entry.id = MarketEntryId("entry-id");
    entry.action = MarketEntryAction::Option::New;
    entry.type = MdEntryType::Option::Bid;
    entry.price = Price(42);
    entry.quantity = Quantity(100);
    return entry;
  }());

  ToFixMapper::map(reply, fix_message, {});
  ASSERT_THAT(get_fix_field<FIX::NoMDEntries>(fix_message), Optional(Eq(1)));

  const auto& entry = fix_message.getGroupRef(1, FIX::FIELD::NoMDEntries);
  EXPECT_THAT(get_fix_field<FIX::MDEntryPx>(entry), Optional(Eq(42.0)));
  EXPECT_THAT(get_fix_field<FIX::MDEntrySize>(entry), Optional(Eq(100.0)));
  EXPECT_THAT(get_fix_field<FIX::MDEntryID>(entry), Eq("entry-id"));
  EXPECT_THAT(get_fix_field<FIX::MDEntryType>(entry),
              Optional(Eq(FIX::MDEntryType_BID)));
  EXPECT_THAT(get_fix_field<FIX::MDUpdateAction>(entry),
              Optional(Eq(FIX::MDUpdateAction_NEW)));
}

TEST_F(AcceptorToFixMarketDataUpdateMapping,
       MapsTradeSpecificMarketDataEntryAttributes) {
  reply.market_data_entries.emplace_back([] {
    MarketDataEntry entry;
    entry.buyer_id = BuyerId("buyer-id");
    entry.seller_id = SellerId("seller-id");
    entry.aggressor_side = AggressorSide::Option::Sell;
    entry.time = MarketEntryTime(core::get_current_system_time());
    return entry;
  }());

  ToFixMapper::map(reply, fix_message, {});
  ASSERT_THAT(get_fix_field<FIX::NoMDEntries>(fix_message), Optional(Eq(1)));

  auto& entry = fix_message.getGroupRef(1, FIX::FIELD::NoMDEntries);
  EXPECT_THAT(get_fix_field_string<FIX::MDEntryTime>(entry),
              Optional(MatchesRegex(UtcTimeOnlyMicrosecondsRegex)));
  EXPECT_THAT(entry.isSetField(FIX::FIELD::MDEntryDate), IsTrue());
  EXPECT_THAT(get_fix_field<FIX::MDEntryBuyer>(entry),
              Optional(Eq("buyer-id")));
  EXPECT_THAT(get_fix_field<FIX::MDEntrySeller>(entry),
              Optional(Eq("seller-id")));
  EXPECT_THAT(get_fix_field<FIX::AggressorSide>(entry),
              Optional(Eq(FIX::Side_SELL)));
}

TEST_F(AcceptorToFixMarketDataUpdateMapping,
       MapsMarketPhaseFromMarketDataEntry) {
  reply.market_data_entries.emplace_back([] {
    MarketDataEntry entry;
    entry.phase = {TradingPhase::Option::Open, TradingStatus::Option::Halt};
    return entry;
  }());

  ToFixMapper::map(reply, fix_message, {});
  ASSERT_THAT(get_fix_field<FIX::NoMDEntries>(fix_message), Optional(Eq(1)));

  auto& entry = fix_message.getGroupRef(1, FIX::FIELD::NoMDEntries);
  EXPECT_THAT(get_fix_field<FIX::TradingSessionSubID>(entry),
              Optional(Eq(FIX::TradingSessionSubID_3)));
  EXPECT_THAT(get_fix_field<FIX::SecurityTradingStatus>(entry),
              Optional(Eq(FIX::SecurityTradingStatus_TRADING_HALT)));
}

TEST_F(AcceptorToFixMarketDataUpdateMapping,
       HardcodesTradingSessionIdWhenMarketPhasePresentInMarketEntry) {
  reply.market_data_entries.emplace_back([] {
    MarketDataEntry entry;
    entry.phase = {TradingPhase::Option::Open, TradingStatus::Option::Halt};
    return entry;
  }());

  ToFixMapper::map(reply, fix_message, {});
  ASSERT_THAT(get_fix_field<FIX::NoMDEntries>(fix_message), Optional(Eq(1)));

  auto& entry = fix_message.getGroupRef(1, FIX::FIELD::NoMDEntries);
  EXPECT_THAT(get_fix_field<FIX::TradingSessionID>(entry),
              Optional(Eq(FIX::TradingSessionID_DAY)));
}

// endregion MarketDataUpdate mapping

// NOLINTEND(*-magic-numbers,*-array-to-pointer-decay)

}  // namespace
}  // namespace simulator::fix::acceptor::test