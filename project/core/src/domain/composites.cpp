#include <fmt/format.h>

#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

#include "core/common/name.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_data_entry.hpp"
#include "core/domain/market_phase.hpp"
#include "core/domain/party.hpp"
#include "core/tools/format.hpp"

namespace simulator {

PartyIdentifier::PartyIdentifier(PartyId identifier, PartyIdSource source)
    : party_id_(std::move(identifier)), source_(source) {}

Party::Party(PartyIdentifier identifier, PartyRole role)
    : identifier_(std::move(identifier)), role_(role) {}

Party::Party(PartyId party_id, PartyIdSource id_source, PartyRole role)
    : Party(PartyIdentifier{std::move(party_id), id_source}, role) {}

auto make_counterparty(const Party& party) -> Party {
  if (party.role() == PartyRole::Option::ExecutingFirm) [[likely]] {
    return Party{party.identifier(), PartyRole::Option::ContraFirm};
  }

  throw std::invalid_argument(fmt::format(
      "can not determine counterparty role for the given party role: {}",
      party.role()));
}

auto make_counterparty(const std::optional<Party>& party)
    -> std::optional<Party> {
  return party.has_value() ? std::make_optional(make_counterparty(*party))
                           : std::nullopt;
}

auto operator<<(std::ostream& stream, const InstrumentDescriptor& descriptor)
    -> std::ostream& {
  return stream << fmt::to_string(descriptor);
}

auto operator<<(std::ostream& ostream, const MarketDataEntry& entry)
    -> std::ostream& {
  return ostream << fmt::to_string(entry);
}

auto operator<<(std::ostream& ostream, const MarketPhase& phase)
    -> std::ostream& {
  return ostream << fmt::to_string(phase);
}

auto operator<<(std::ostream& ostream, const PartyIdentifier& identifier)
    -> std::ostream& {
  return ostream << fmt::to_string(identifier);
}

auto operator<<(std::ostream& ostream, const Party& party) -> std::ostream& {
  return ostream << fmt::to_string(party);
}

}  // namespace simulator

auto fmt::formatter<simulator::InstrumentDescriptor>::format(
    const formattable& descriptor, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(
      context.out(),
      "{{ {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {:p}={} }}",
      name_of(descriptor.symbol),
      descriptor.symbol,
      name_of(descriptor.security_id),
      descriptor.security_id,
      name_of(descriptor.security_id_source),
      descriptor.security_id_source,
      name_of(descriptor.security_type),
      descriptor.security_type,
      name_of(descriptor.security_exchange),
      descriptor.security_exchange,
      name_of(descriptor.currency),
      descriptor.currency,
      name_of(descriptor.requester_instrument_id),
      descriptor.requester_instrument_id,
      name_of(descriptor.parties),
      format_collection(descriptor.parties));
}

auto fmt::formatter<simulator::MarketDataEntry>::format(
    const formattable& entry, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "{{ {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, {}={}, "
                   "{}={}, {}={}, {}={} }}",
                   name_of(entry.id),
                   entry.id,
                   name_of(entry.type),
                   entry.type,
                   name_of(entry.action),
                   entry.action,
                   name_of(entry.time),
                   entry.time,
                   name_of(entry.price),
                   entry.price,
                   name_of(entry.quantity),
                   entry.quantity,
                   name_of(entry.buyer_id),
                   entry.buyer_id,
                   name_of(entry.seller_id),
                   entry.seller_id,
                   name_of(entry.aggressor_side),
                   entry.aggressor_side,
                   name_of(entry.phase),
                   entry.phase);
}

auto fmt::formatter<simulator::MarketPhase>::format(
    const formattable& phase, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "{{ {}={}, {}={} }}",
                   name_of(phase.trading_phase()),
                   phase.trading_phase(),
                   name_of(phase.trading_status()),
                   phase.trading_status());
}

auto fmt::formatter<simulator::PartyIdentifier>::format(
    const formattable& identifier, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "{{ {}={}, {}={} }}",
                   name_of(identifier.party_id()),
                   identifier.party_id(),
                   name_of(identifier.source()),
                   identifier.source());
}

auto fmt::formatter<simulator::Party>::format(const formattable& party,
                                              format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "{{ {}={}, {}={} }}",
                   name_of(party.identifier()),
                   party.identifier(),
                   name_of(party.role()),
                   party.role());
}
