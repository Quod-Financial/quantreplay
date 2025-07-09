#include "ih/mapping/from_fix_mapper.hpp"

#include <quickfix/Exceptions.h>

#include "common/custom_fields.hpp"
#include "common/mapping/checks.hpp"
#include "common/mapping/from_fix_mapping.hpp"
#include "log/logging.hpp"

namespace simulator::fix::acceptor {

namespace {

auto decode_instrument(const FIX::FieldMap& source, std::vector<Party> parties)
    -> InstrumentDescriptor {
  InstrumentDescriptor instrument;

  map_fix_field<FIX::Symbol>(source, instrument.symbol);
  map_fix_field<FIX::SecurityType>(source, instrument.security_type);
  map_fix_field<FIX::SecurityExchange>(source, instrument.security_exchange);
  map_fix_field<FIX::Currency>(source, instrument.currency);
  map_fix_field<FIX::SecurityID>(source, instrument.security_id);
  map_fix_field<FIX::SecurityIDSource>(source, instrument.security_id_source);
  instrument.parties = std::move(parties);

  return instrument;
}

auto decode_party(const FIX::FieldMap& source) -> Party {
  return Party{
      convert_from_fix_field<PartyId>(get_fix_field<FIX::PartyID>(source)),
      convert_from_fix_field<PartyIdSource>(
          get_fix_field<FIX::PartyIDSource>(source)),
      convert_from_fix_field<PartyRole>(get_fix_field<FIX::PartyRole>(source))};
}

auto map_instrument(const FIX::FieldMap& source,
                    std::vector<Party> parties,
                    InstrumentDescriptor& destination) -> void {
  destination = decode_instrument(source, std::move(parties));
}

auto map_parties(const FIX::FieldMap& source, std::vector<Party>& destination)
    -> void {
  constexpr int GroupTag = FIX::FIELD::NoPartyIDs;
  if (!contains<GroupTag>(source)) {
    return;
  }

  const auto parties_num = get_fix_field<FIX::NoPartyIDs>(source);
  destination.reserve(static_cast<std::size_t>(parties_num));
  for (int party = 1; party <= parties_num; ++party) {
    destination.emplace_back(decode_party(source.getGroupRef(party, GroupTag)));
  }
}

auto map_market_entry_type(const FIX::FieldMap& source,
                           std::vector<MdEntryType>& destination) -> void {
  try {
    destination.emplace_back(convert_from_fix_field<MdEntryType>(
        get_fix_field<FIX::MDEntryType>(source)));
  } catch (const FIX::IncorrectTagValue& exception) {
    log::warn("ignoring an unsupported market data entry type: {}",
              exception.detail);
  }
}

}  // namespace

auto FromFixMapper::map(const FIX::Message& fix_message,
                        protocol::OrderPlacementRequest& request) -> void {
  map_parties(fix_message, request.parties);
  map_instrument(fix_message, request.parties, request.instrument);

  map_fix_field<FIX::ClOrdID>(fix_message, request.client_order_id);
  map_fix_field<FIX::OrdType>(fix_message, request.order_type);
  map_fix_field<FIX::Side>(fix_message, request.side);
  map_fix_field<FIX::TimeInForce>(fix_message, request.time_in_force);
  map_fix_field<FIX::OrderQty>(fix_message, request.order_quantity);
  map_fix_field<FIX::Price>(fix_message, request.order_price);
  map_fix_field<FIX::ExpireTime>(fix_message, request.expire_time);
  map_fix_field<FIX::ExpireDate>(fix_message, request.expire_date);

  if (request.side == Side::Option::SellShortExempt) {
    map_fix_field<FIX::ShortSaleExemptionReason>(
        fix_message, request.short_sale_exempt_reason);
  }
}

auto FromFixMapper::map(const FIX::Message& fix_message,
                        protocol::OrderModificationRequest& request) -> void {
  map_parties(fix_message, request.parties);
  map_instrument(fix_message, request.parties, request.instrument);

  map_fix_field<FIX::OrigClOrdID>(fix_message, request.orig_client_order_id);
  map_fix_field<FIX::OrderID>(fix_message, request.venue_order_id);
  map_fix_field<FIX::ClOrdID>(fix_message, request.client_order_id);
  map_fix_field<FIX::OrdType>(fix_message, request.order_type);
  map_fix_field<FIX::Side>(fix_message, request.side);
  map_fix_field<FIX::TimeInForce>(fix_message, request.time_in_force);
  map_fix_field<FIX::OrderQty>(fix_message, request.order_quantity);
  map_fix_field<FIX::Price>(fix_message, request.order_price);
  map_fix_field<FIX::ExpireTime>(fix_message, request.expire_time);
  map_fix_field<FIX::ExpireDate>(fix_message, request.expire_date);

  if (request.side == Side::Option::SellShortExempt) {
    map_fix_field<FIX::ShortSaleExemptionReason>(
        fix_message, request.short_sale_exempt_reason);
  }
}

auto FromFixMapper::map(const FIX::Message& fix_message,
                        protocol::OrderCancellationRequest& request) -> void {
  map_parties(fix_message, request.parties);
  map_instrument(fix_message, request.parties, request.instrument);

  map_fix_field<FIX::OrigClOrdID>(fix_message, request.orig_client_order_id);
  map_fix_field<FIX::OrderID>(fix_message, request.venue_order_id);
  map_fix_field<FIX::ClOrdID>(fix_message, request.client_order_id);
  map_fix_field<FIX::Side>(fix_message, request.side);
}

auto FromFixMapper::map(const FIX::Message& fix_message,
                        protocol::MarketDataRequest& request) -> void {
  map_parties(fix_message, request.parties);
  map_fix_field<FIX::MDReqID>(fix_message, request.request_id);
  map_fix_field<FIX::MDUpdateType>(fix_message, request.update_type);
  map_fix_field<FIX::MarketDepth>(fix_message, request.market_depth);
  map_fix_field<FIX::SubscriptionRequestType>(fix_message,
                                              request.request_type);

  const auto entries_num = get_fix_field<FIX::NoMDEntryTypes>(fix_message);
  request.market_data_types.reserve(static_cast<std::size_t>(entries_num));
  for (int entry = 1; entry <= entries_num; ++entry) {
    map_market_entry_type(
        fix_message.getGroupRef(entry, FIX::FIELD::NoMDEntryTypes),
        request.market_data_types);
  }

  const auto symbols_num = get_fix_field<FIX::NoRelatedSym>(fix_message);
  request.instruments.reserve(static_cast<std::size_t>(symbols_num));
  for (int symbol = 1; symbol <= symbols_num; ++symbol) {
    request.instruments.emplace_back(decode_instrument(
        fix_message.getGroupRef(symbol, FIX::FIELD::NoRelatedSym),
        request.parties));
  }
}

auto FromFixMapper::map(const FIX::Message& message,
                        protocol::SecurityStatusRequest& request) -> void {
  map_fix_field<FIX::MsgSeqNum>(message.getHeader(), request.seq_num);

  map_fix_field<FIX::Symbol>(message, request.instrument.symbol);
  map_fix_field<FIX::SecurityType>(message, request.instrument.security_type);
  map_fix_field<FIX::Currency>(message, request.instrument.currency);
  map_fix_field<FIX::SecurityID>(message, request.instrument.security_id);
  map_fix_field<FIX::SecurityIDSource>(message,
                                       request.instrument.security_id_source);
  map_fix_field<FIX::SecurityExchange>(message,
                                       request.instrument.security_exchange);

  if (contains<FIX::FIELD::NoInstrumentParties>(message)) {
    const auto parties_num = get_fix_field<FIX::NoInstrumentParties>(message);
    request.instrument.parties.reserve(static_cast<std::size_t>(parties_num));
    for (int party = 1; party <= parties_num; ++party) {
      const auto& grp =
          message.getGroupRef(party, FIX::FIELD::NoInstrumentParties);
      request.instrument.parties.emplace_back(
          Party(convert_from_fix_field<PartyId>(
                    get_fix_field<FIX::InstrumentPartyID>(grp)),
                convert_from_fix_field<PartyIdSource>(
                    get_fix_field<FIX::InstrumentPartyIDSource>(grp)),
                convert_from_fix_field<PartyRole>(
                    get_fix_field<FIX::InstrumentPartyRole>(grp))));
    }
  }

  map_fix_field<FIX::SecurityStatusReqID>(message, request.request_id);
  map_fix_field<FIX::SubscriptionRequestType>(message, request.request_type);
}

}  // namespace simulator::fix::acceptor