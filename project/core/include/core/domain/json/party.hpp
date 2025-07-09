#ifndef SIMULATOR_CORE_DOMAIN_JSON_PARTY_HPP_
#define SIMULATOR_CORE_DOMAIN_JSON_PARTY_HPP_

#include "core/common/json/type.hpp"
#include "core/domain/party.hpp"

namespace simulator::core::json {

template <>
struct Type<simulator::PartyIdentifier> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value)
      -> simulator::PartyIdentifier;

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const simulator::PartyIdentifier& value) -> void;
};

template <>
struct Type<simulator::Party> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value)
      -> simulator::Party;

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const simulator::Party& value) -> void;
};

}  // namespace simulator::core::json

#endif  // SIMULATOR_CORE_DOMAIN_JSON_PARTY_HPP_
