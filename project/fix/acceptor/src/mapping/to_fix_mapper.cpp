#include "ih/mapping/to_fix_mapper.hpp"

#include <quickfix/fix50sp2/ExecutionReport.h>
#include <quickfix/fix50sp2/MarketDataIncrementalRefresh.h>
#include <quickfix/fix50sp2/MarketDataSnapshotFullRefresh.h>
#include <quickfix/fix50sp2/SecurityStatus.h>

#include <memory>

#include "common/custom_fields.hpp"
#include "common/mapping/to_fix_mapping.hpp"
#include "core/tools/time.hpp"

namespace simulator::fix::acceptor {

namespace {

auto map_market_entry(const MarketDataEntry& source,
                      FIX::FieldMap& destination,
                      const MappingSettings::Setting& setting) -> void {
  map_fix_field<FIX::MDEntryID>(source.id, destination);
  map_fix_field<FIX::MDEntryType>(source.type, destination);
  map_fix_field<FIX::MDEntryPx>(source.price, destination);
  map_fix_field<FIX::MDEntrySize>(source.quantity, destination);
  map_fix_field<FIX::MDEntryBuyer>(source.buyer_id, destination);
  map_fix_field<FIX::MDEntrySeller>(source.seller_id, destination);
  map_fix_field<FIX::AggressorSide>(source.aggressor_side, destination);
  map_fix_field<FIX::MDEntryDate>(source.time, destination);
  map_fix_field<FIX::MDEntryTime>(
      source.time, destination, setting.timestamp_precision);

  if (const auto& phase = source.phase) {
    set_fix_field<FIX::TradingSessionID>(FIX::TradingSessionID_DAY,
                                         destination);
    map_fix_field<FIX::TradingSessionSubID>(phase->trading_phase(),
                                            destination);
    map_fix_field<FIX::SecurityTradingStatus>(phase->trading_status(),
                                              destination);
  }
}

}  // namespace

auto ToFixMapper::map(const protocol::BusinessMessageReject& reply,
                      FIX::Message& fix_message,
                      [[maybe_unused]] const MappingSettings::Setting& setting)
    -> void {
  map_fix_field<FIX::RefMsgType>(reply.ref_message_type, fix_message);
  map_fix_field<FIX::RefSeqNum>(reply.ref_seq_num, fix_message);
  map_fix_field<FIX::BusinessRejectRefID>(reply.ref_id, fix_message);
  map_fix_field<FIX::Text>(reply.text, fix_message);
  map_fix_field<FIX::BusinessRejectReason>(reply.business_reject_reason,
                                           fix_message);
}

auto ToFixMapper::map(const protocol::ExecutionReport& reply,
                      FIX::Message& fix_message,
                      const MappingSettings::Setting& setting) -> void {
  map_fix_execution_report_common_fields(reply, fix_message, setting);

  map_fix_field<FIX::ExecType>(reply.execution_type, fix_message);
  map_fix_field<FIX::OrdStatus>(reply.order_status, fix_message);
  map_fix_field<FIX::LastPx>(reply.execution_price, fix_message);
  map_fix_field<FIX::LeavesQty>(reply.leaves_quantity, fix_message);
  map_fix_field<FIX::CumQty>(reply.cum_executed_quantity, fix_message);
  map_fix_field<FIX::LastQty>(reply.executed_quantity, fix_message);
}

auto ToFixMapper::map(const protocol::OrderPlacementConfirmation& reply,
                      FIX::Message& fix_message,
                      const MappingSettings::Setting& setting) -> void {
  map_fix_execution_report_common_fields(reply, fix_message, setting);

  set_fix_field<FIX::ExecType>(FIX::ExecType_NEW, fix_message);
  set_fix_field<FIX::OrdStatus>(FIX::OrdStatus_NEW, fix_message);
  set_fix_field<FIX::CumQty>(0.0, fix_message);
  map_fix_field<FIX::LeavesQty>(reply.order_quantity, fix_message);
}

auto ToFixMapper::map(const protocol::OrderPlacementReject& reply,
                      FIX::Message& fix_message,
                      const MappingSettings::Setting& setting) -> void {
  map_fix_execution_report_common_fields(reply, fix_message, setting);

  set_fix_field<FIX::ExecType>(FIX::ExecType_REJECTED, fix_message);
  set_fix_field<FIX::OrdStatus>(FIX::OrdStatus_REJECTED, fix_message);
  set_fix_field<FIX::CumQty>(0.0, fix_message);
  map_fix_field<FIX::LeavesQty>(reply.order_quantity, fix_message);
  map_fix_field<FIX::Text>(reply.reject_text, fix_message);
}

auto ToFixMapper::map(const protocol::OrderModificationConfirmation& reply,
                      FIX::Message& fix_message,
                      const MappingSettings::Setting& setting) -> void {
  map_fix_execution_report_common_fields(reply, fix_message, setting);

  set_fix_field<FIX::ExecType>(FIX::ExecType_REPLACED, fix_message);

  map_fix_field<FIX::OrigClOrdID>(reply.orig_client_order_id, fix_message);
  map_fix_field<FIX::LeavesQty>(reply.leaving_quantity, fix_message);
  map_fix_field<FIX::CumQty>(reply.cum_executed_quantity, fix_message);
  map_fix_field<FIX::OrdStatus>(reply.order_status, fix_message);
}

auto ToFixMapper::map(const protocol::OrderModificationReject& reply,
                      FIX::Message& fix_message,
                      [[maybe_unused]] const MappingSettings::Setting& setting)
    -> void {
  map_fix_order_cancel_reject_common_fields(reply, fix_message);

  set_fix_field<FIX::CxlRejResponseTo>(
      FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST, fix_message);
}

auto ToFixMapper::map(const protocol::OrderCancellationConfirmation& reply,
                      FIX::Message& fix_message,
                      const MappingSettings::Setting& setting) -> void {
  map_fix_execution_report_common_fields(reply, fix_message, setting);

  set_fix_field<FIX::ExecType>(FIX::ExecType_CANCELED, fix_message);

  map_fix_field<FIX::OrigClOrdID>(reply.orig_client_order_id, fix_message);
  map_fix_field<FIX::LeavesQty>(reply.leaving_quantity, fix_message);
  map_fix_field<FIX::CumQty>(reply.cum_executed_quantity, fix_message);
  map_fix_field<FIX::OrdStatus>(reply.order_status, fix_message);
}

auto ToFixMapper::map(const protocol::OrderCancellationReject& reply,
                      FIX::Message& fix_message,
                      [[maybe_unused]] const MappingSettings::Setting& setting)
    -> void {
  map_fix_order_cancel_reject_common_fields(reply, fix_message);

  set_fix_field<FIX::CxlRejResponseTo>(
      FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST, fix_message);
}

auto ToFixMapper::map(const protocol::MarketDataReject& reply,
                      FIX::Message& fix_message,
                      [[maybe_unused]] const MappingSettings::Setting& setting)
    -> void {
  map_fix_field<FIX::MDReqID>(reply.request_id, fix_message);
  map_fix_field<FIX::MDReqRejReason>(reply.reject_reason, fix_message);
  map_fix_field<FIX::Text>(reply.reject_text, fix_message);
}

auto ToFixMapper::map(const protocol::MarketDataSnapshot& reply,
                      FIX::Message& fix_message,
                      const MappingSettings::Setting& setting) -> void {
  map_instrument(reply.instrument, fix_message);

  map_fix_field<FIX::MDReqID>(reply.request_id, fix_message);
  set_fix_field<FIX::LastUpdateTime>(core::get_current_system_time(),
                                     fix_message,
                                     setting.timestamp_precision);

  if (reply.market_data_entries.empty()) {
    set_fix_field<FIX::NoMDEntries>(0, fix_message);
  }
  for (const MarketDataEntry& entry : reply.market_data_entries) {
    auto group = std::make_unique<
        FIX50SP2::MarketDataSnapshotFullRefresh::NoMDEntries>();
    map_market_entry(entry, *group, setting);
    fix_message.addGroupPtr(FIX::FIELD::NoMDEntries, group.release());
  }
}

auto ToFixMapper::map(const protocol::MarketDataUpdate& reply,
                      FIX::Message& fix_message,
                      const MappingSettings::Setting& setting) -> void {
  map_fix_field<FIX::MDReqID>(reply.request_id, fix_message);

  if (reply.market_data_entries.empty()) {
    set_fix_field<FIX::NoMDEntries>(0, fix_message);
  }
  for (const MarketDataEntry& entry : reply.market_data_entries) {
    auto group =
        std::make_unique<FIX50SP2::MarketDataIncrementalRefresh::NoMDEntries>();
    map_market_entry(entry, *group, setting);
    map_fix_field<FIX::MDUpdateAction>(entry.action, *group);
    fix_message.addGroupPtr(FIX::FIELD::NoMDEntries, group.release());
  }
}

auto ToFixMapper::map(const protocol::SecurityStatus& reply,
                      FIX::Message& fix_message,
                      [[maybe_unused]] const MappingSettings::Setting& setting)
    -> void {
  map_instrument(reply.instrument, fix_message);
  for (const Party& party : reply.instrument.parties) {
    auto group =
        std::make_unique<FIX50SP2::SecurityStatus::NoInstrumentParties>();
    map_fix_field<FIX::InstrumentPartyID>(party.party_id(), *group);
    map_fix_field<FIX::InstrumentPartyIDSource>(party.source(), *group);
    map_fix_field<FIX::InstrumentPartyRole>(party.role(), *group);
    fix_message.addGroupPtr(FIX::FIELD::NoInstrumentParties, group.release());
  }

  map_fix_field<FIX::SecurityStatusReqID>(reply.request_id, fix_message);
  map_fix_field<FIX::TradingSessionSubID>(reply.trading_phase, fix_message);
  map_fix_field<FIX::SecurityTradingStatus>(reply.trading_status, fix_message);
  set_fix_field<FIX::TradingSessionID>(FIX::TradingSessionID_DAY, fix_message);
}

auto ToFixMapper::map_instrument(const InstrumentDescriptor& instrument,
                                 FIX::FieldMap& destination) -> void {
  map_fix_field<FIX::Symbol>(instrument.symbol, destination);
  map_fix_field<FIX::SecurityType>(instrument.security_type, destination);
  map_fix_field<FIX::Currency>(instrument.currency, destination);
  map_fix_field<FIX::SecurityExchange>(instrument.security_exchange,
                                       destination);
  map_fix_field<FIX::SecurityID>(instrument.security_id, destination);
  map_fix_field<FIX::SecurityIDSource>(instrument.security_id_source,
                                       destination);
}

auto ToFixMapper::map_parties(const std::vector<Party>& parties,
                              FIX::FieldMap& destination) -> void {
  for (const Party& party : parties) {
    auto group = std::make_unique<FIX50SP2::ExecutionReport::NoPartyIDs>();
    map_fix_field<FIX::PartyID>(party.party_id(), *group);
    map_fix_field<FIX::PartyIDSource>(party.source(), *group);
    map_fix_field<FIX::PartyRole>(party.role(), *group);
    destination.addGroupPtr(FIX::FIELD::NoPartyIDs, group.release());
  }
}

template <typename InternalMessage>
auto ToFixMapper::map_fix_execution_report_common_fields(
    const InternalMessage& reply,
    FIX::Message& fix_message,
    const MappingSettings::Setting& setting) -> void {
  set_fix_field<FIX::MsgType>(FIX::MsgType_ExecutionReport,
                              fix_message.getHeader());

  map_instrument(reply.instrument, fix_message);
  map_parties(reply.parties, fix_message);

  map_fix_field<FIX::ClOrdID>(reply.client_order_id, fix_message);
  map_fix_field<FIX::OrderID>(reply.venue_order_id, fix_message);
  map_fix_field<FIX::ExecID>(reply.execution_id, fix_message);
  map_fix_field<FIX::Price>(reply.order_price, fix_message);
  map_fix_field<FIX::Side>(reply.side, fix_message);
  map_fix_field<FIX::OrdType>(reply.order_type, fix_message);
  map_fix_field<FIX::TimeInForce>(reply.time_in_force, fix_message);
  map_fix_field<FIX::ExpireTime>(
      reply.expire_time, fix_message, setting.timestamp_precision);
  map_fix_field<FIX::ExpireDate>(reply.expire_date, fix_message);
  map_fix_field<FIX::ShortSaleExemptionReason>(reply.short_sale_exempt_reason,
                                               fix_message);
}

template <typename InternalMessage>
auto ToFixMapper::map_fix_order_cancel_reject_common_fields(
    const InternalMessage& reply, FIX::Message& fix_message) -> void {
  set_fix_field<FIX::MsgType>(FIX::MsgType_OrderCancelReject,
                              fix_message.getHeader());

  map_fix_field<FIX::ClOrdID>(reply.client_order_id, fix_message);
  map_fix_field<FIX::OrigClOrdID>(reply.orig_client_order_id, fix_message);
  map_fix_field<FIX::OrderID>(reply.venue_order_id, fix_message);
  map_fix_field<FIX::Text>(reply.reject_text, fix_message);
  map_fix_field<FIX::OrdStatus>(reply.order_status, fix_message);
}

}  // namespace simulator::fix::acceptor