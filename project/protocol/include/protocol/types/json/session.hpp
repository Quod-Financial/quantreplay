#ifndef SIMULATOR_PROTOCOL_TYPES_JSON_SESSION_HPP_
#define SIMULATOR_PROTOCOL_TYPES_JSON_SESSION_HPP_

#include "core/common/json/type.hpp"
#include "protocol/types/session.hpp"

namespace simulator::core::json {

template <>
struct Type<simulator::protocol::fix::Session> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value)
      -> simulator::protocol::fix::Session;

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const simulator::protocol::fix::Session& value)
      -> void;
};

}  // namespace simulator::core::json

#endif  // SIMULATOR_PROTOCOL_TYPES_JSON_SESSION_HPP_
