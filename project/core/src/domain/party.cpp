#include "core/domain/json/party.hpp"

namespace simulator::core::json {

auto Type<simulator::PartyIdentifier>::read_json_value(
    const rapidjson::Value& json_value) -> simulator::PartyIdentifier {
  auto party_id = json::read_json_value<PartyId>(json_value, "party_id");
  auto source = json::read_json_value<PartyIdSource>(json_value, "source");
  return simulator::PartyIdentifier{std::move(party_id), std::move(source)};
}

auto Type<simulator::PartyIdentifier>::write_json_value(
    rapidjson::Value& json_value,
    rapidjson::Document::AllocatorType& allocator,
    const simulator::PartyIdentifier& value) -> void {
  json_value = rapidjson::Value(rapidjson::Type::kObjectType);
  json_value.MemberReserve(2, allocator);
  json::write_json_value(json_value, allocator, value.party_id(), "party_id");
  json::write_json_value(json_value, allocator, value.source(), "source");
}

auto Type<simulator::Party>::read_json_value(const rapidjson::Value& json_value)
    -> simulator::Party {
  auto identifier = json::read_json_value<simulator::PartyIdentifier>(
      json_value, "identifier");
  auto role = json::read_json_value<simulator::PartyRole>(json_value, "role");
  return simulator::Party{std::move(identifier), std::move(role)};
}

auto Type<simulator::Party>::write_json_value(
    rapidjson::Value& json_value,
    rapidjson::Document::AllocatorType& allocator,
    const simulator::Party& value) -> void {
  json_value = rapidjson::Value(rapidjson::Type::kObjectType);
  json_value.MemberReserve(2, allocator);
  json::write_json_value(
      json_value, allocator, value.identifier(), "identifier");
  json::write_json_value(json_value, allocator, value.role(), "role");
}

}  // namespace simulator::core::json
