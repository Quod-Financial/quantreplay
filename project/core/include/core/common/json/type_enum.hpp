#ifndef SIMULATOR_CORE_COMMON_JSON_TYPE_ENUM_HPP_
#define SIMULATOR_CORE_COMMON_JSON_TYPE_ENUM_HPP_

#include <fmt/format.h>
#include <rapidjson/document.h>

#include "core/common/enum_converter.hpp"
#include "core/common/json/type.hpp"
#include "core/common/meta.hpp"

namespace simulator::core::json {

template <Enumerable E>
struct Type<E> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value) -> E {
    return EnumConverter<E>::value(
        Type<std::string_view>::read_json_value(json_value));
  }

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const E& value) -> void {
    Type<std::string_view>::write_json_value(
        json_value, allocator, EnumConverter<E>::str(value));
  }
};

}  // namespace simulator::core::json

#endif  // SIMULATOR_CORE_COMMON_JSON_TYPE_ENUM_HPP_
