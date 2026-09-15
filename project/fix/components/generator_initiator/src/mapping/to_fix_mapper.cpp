#include "ih/mapping/to_fix_mapper.hpp"

#include <quickfix/fix50sp2/MarketDataRequest.h>

#include <memory>
#include <vector>

#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
#include "ih/mapping/to_fix_mapping.hpp"

namespace simulator::fix::generator_initiator {

namespace {

auto map_instrument(const InstrumentDescriptor& instrument,
                    FIX::FieldMap& destination) -> void {
  map_fix_field<FIX::SecurityID>(instrument.security_id, destination);
  map_fix_field<FIX::Symbol>(instrument.symbol, destination);
  map_fix_field<FIX::Currency>(instrument.currency, destination);
  map_fix_field<FIX::SecurityExchange>(instrument.security_exchange,
                                       destination);
  map_fix_field<FIX::SecurityType>(instrument.security_type, destination);
  map_fix_field<FIX::SecurityIDSource>(instrument.security_id_source,
                                       destination);
}

auto map_parties(const std::vector<Party>& parties, FIX::Message& destination)
    -> void {
  for (const Party& party : parties) {
    auto group = std::make_unique<FIX50SP2::MarketDataRequest::NoPartyIDs>();
    map_fix_field<FIX::PartyID>(party.party_id(), *group);
    map_fix_field<FIX::PartyIDSource>(party.source(), *group);
    map_fix_field<FIX::PartyRole>(party.role(), *group);
    destination.addGroupPtr(FIX::FIELD::NoPartyIDs, group.release());
  }
}

auto map_market_data_types(const std::vector<MdEntryType>& market_data_types,
                           FIX::Message& destination) -> void {
  if (market_data_types.empty()) {
    set_fix_field<FIX::NoMDEntryTypes>(0, destination);
  }
  for (const MdEntryType& entry_type : market_data_types) {
    auto group =
        std::make_unique<FIX50SP2::MarketDataRequest::NoMDEntryTypes>();
    map_fix_field<FIX::MDEntryType>(entry_type, *group);
    destination.addGroupPtr(FIX::FIELD::NoMDEntryTypes, group.release());
  }
}

auto map_instruments(const std::vector<InstrumentDescriptor>& instruments,
                     FIX::Message& destination) -> void {
  if (instruments.empty()) {
    set_fix_field<FIX::NoRelatedSym>(0, destination);
  }
  for (const InstrumentDescriptor& instrument : instruments) {
    auto group = std::make_unique<FIX50SP2::MarketDataRequest::NoRelatedSym>();
    map_instrument(instrument, *group);
    destination.addGroupPtr(FIX::FIELD::NoRelatedSym, group.release());
  }
}

}  // namespace

auto ToFixMapper::map(const protocol::MarketDataRequest& request,
                      FIX::Message& fix_message,
                      [[maybe_unused]] const MappingSettings::Setting& setting)
    -> void {
  map_fix_field<FIX::MDReqID>(request.request_id, fix_message);
  map_instruments(request.instruments, fix_message);
  map_market_data_types(request.market_data_types, fix_message);
  map_parties(request.parties, fix_message);
  map_fix_field<FIX::MarketDepth>(request.market_depth, fix_message);
  map_fix_field<FIX::SubscriptionRequestType>(request.request_type,
                                              fix_message);
  map_fix_field<FIX::MDUpdateType>(request.update_type, fix_message);
}

}  // namespace simulator::fix::generator_initiator
