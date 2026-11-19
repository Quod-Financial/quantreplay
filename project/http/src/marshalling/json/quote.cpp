#include "ih/marshalling/json/quote.hpp"

#include <fmt/format.h>
#include <rapidjson/document.h>

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
#include "ih/marshalling/json/detail/enumeration_resolver.hpp"
#include "ih/marshalling/json/detail/keys.hpp"
#include "ih/marshalling/json/detail/unmarshaller.hpp"
#include "ih/marshalling/json/detail/utils.hpp"

namespace simulator::http::json {
namespace {

struct QuoteIdentification {
  std::optional<std::string> symbol;
  std::optional<std::string> security_type;
  std::optional<std::string> security_id;
  std::optional<std::string> security_id_source;
  std::optional<std::string> price_currency;
  std::optional<std::string> security_exchange;
  std::optional<std::string> party_role;
  std::optional<std::string> party_id;
};

template <typename Attribute, typename Value>
[[nodiscard]]
auto make_attribute(const std::optional<Value>& value)
    -> std::optional<Attribute> {
  return value.has_value() ? std::make_optional<Attribute>(*value)
                           : std::nullopt;
}

template <typename Attribute>
[[nodiscard]]
auto resolve_enumerable(const std::string& value) -> Attribute {
  typename Attribute::Option option{};
  EnumerationResolver::resolve(value, option);
  return Attribute{option};
}

template <typename Attribute>
[[nodiscard]]
auto make_enumerable(const std::optional<std::string>& value)
    -> std::optional<Attribute> {
  return value.has_value() ? std::make_optional<Attribute>(
                                 resolve_enumerable<Attribute>(*value))
                           : std::nullopt;
}

[[nodiscard]]
auto make_parties(const QuoteIdentification& identification)
    -> std::vector<Party> {
  const auto& party_id = identification.party_id;
  const auto& party_role = identification.party_role;
  if (!party_id.has_value() && !party_role.has_value()) {
    return {};
  }
  if (!party_id.has_value() || !party_role.has_value()) {
    throw std::invalid_argument{
        "`partyId' and `partyRole' must be given together"};
  }

  std::vector<Party> parties;
  // The listing lookup does not compare party id sources.
  parties.emplace_back(PartyId{*party_id},
                       PartyIdSource{PartyIdSource::Option::Proprietary},
                       resolve_enumerable<PartyRole>(*party_role));
  return parties;
}

[[nodiscard]]
auto make_instrument(const QuoteIdentification& identification)
    -> InstrumentDescriptor {
  if (identification.security_id.has_value() &&
      !identification.security_id_source.has_value()) {
    throw std::invalid_argument{
        "`securityIdSource' is required when `securityId' is given"};
  }

  return InstrumentDescriptor{
      .security_id = make_attribute<SecurityId>(identification.security_id),
      .symbol = make_attribute<Symbol>(identification.symbol),
      .currency = make_attribute<Currency>(identification.price_currency),
      .security_exchange =
          make_attribute<SecurityExchange>(identification.security_exchange),
      .parties = make_parties(identification),
      .requester_instrument_id = std::nullopt,
      .security_type =
          make_enumerable<SecurityType>(identification.security_type),
      .security_id_source =
          make_enumerable<SecurityIdSource>(identification.security_id_source)};
}

[[nodiscard]]
auto require_field(const std::optional<double>& value, std::string_view key)
    -> double {
  if (!value.has_value()) {
    throw std::invalid_argument{
        fmt::format("missing required field `{}'", key)};
  }
  return *value;
}

[[nodiscard]]
auto unmarshall_venue_id(Unmarshaller& unmarshaller) -> std::string {
  std::optional<std::string> venue_id;
  unmarshaller(quote_key::VenueId, venue_id);
  if (!venue_id.has_value() || venue_id->empty()) {
    throw std::invalid_argument{"missing required field `venueId'"};
  }
  return std::move(*venue_id);
}

[[nodiscard]]
auto unmarshall_identification(Unmarshaller& unmarshaller)
    -> QuoteIdentification {
  QuoteIdentification identification;
  unmarshaller(quote_key::Symbol, identification.symbol);
  unmarshaller(quote_key::SecurityType, identification.security_type);
  unmarshaller(quote_key::SecurityId, identification.security_id);
  unmarshaller(quote_key::SecurityIdSource, identification.security_id_source);
  unmarshaller(quote_key::PriceCurrency, identification.price_currency);
  unmarshaller(quote_key::SecurityExchange, identification.security_exchange);
  unmarshaller(quote_key::PartyRole, identification.party_role);
  unmarshaller(quote_key::PartyId, identification.party_id);
  return identification;
}

[[nodiscard]]
auto unmarshall_entry(const rapidjson::Value& value, std::string_view side_key)
    -> protocol::QuoteEntry {
  if (!value.IsObject()) {
    throw std::runtime_error{fmt::format(
        "can not parse a JSON object in `{}' JSON array", side_key)};
  }

  Unmarshaller unmarshaller{value};
  std::optional<double> price;
  std::optional<double> quantity;
  unmarshaller(quote_key::Price, price);
  unmarshaller(quote_key::Quantity, quantity);
  return protocol::QuoteEntry{
      .price = Price{require_field(price, quote_key::Price)},
      .quantity = Quantity{require_field(quantity, quote_key::Quantity)}};
}

[[nodiscard]]
auto unmarshall_entries(const rapidjson::Document& document,
                        std::string_view side_key)
    -> std::vector<protocol::QuoteEntry> {
  std::vector<protocol::QuoteEntry> entries;
  const rapidjson::Value name{make_key(side_key)};
  const auto member = document.FindMember(name);
  if (member == document.MemberEnd()) {
    return entries;
  }
  if (!member->value.IsArray()) {
    throw std::runtime_error{fmt::format(
        "can not parse `{}' key in Quote JSON, which is not a JSON array",
        side_key)};
  }

  for (const auto& entry : member->value.GetArray()) {
    entries.push_back(unmarshall_entry(entry, side_key));
  }
  return entries;
}

}  // namespace

auto QuoteUnmarshaller::unmarshall(std::string_view json,
                                   QuotePublication& dest) -> void {
  rapidjson::Document document;
  document.Parse(json.data(), json.size());
  if (document.HasParseError() || !document.IsObject()) {
    throw std::runtime_error{"failed to parse Quote JSON object"};
  }

  Unmarshaller unmarshaller{document};
  dest.venue_id = unmarshall_venue_id(unmarshaller);
  dest.request.instrument =
      make_instrument(unmarshall_identification(unmarshaller));
  dest.request.bids = unmarshall_entries(document, quote_key::Bids);
  dest.request.asks = unmarshall_entries(document, quote_key::Asks);
}

}  // namespace simulator::http::json
