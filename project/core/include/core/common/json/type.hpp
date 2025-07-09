#ifndef SIMULATOR_CORE_COMMON_JSON_TYPE_HPP_
#define SIMULATOR_CORE_COMMON_JSON_TYPE_HPP_

#include <fmt/format.h>
#include <rapidjson/document.h>

#include <optional>
#include <stdexcept>

#include "core/common/meta.hpp"

namespace simulator::core::json {

template <typename T>
struct Type {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value) -> T;

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const T& value) -> void;
};

template <typename T>
[[nodiscard]]
auto read_json_value(const rapidjson::Value& json_value, std::string_view key)
    -> T {
  const auto iter = json_value.FindMember(key.data());

  if constexpr (is_optional<T>::value) {
    if (iter != json_value.MemberEnd() && !iter->value.IsNull()) {
      try {
        return std::make_optional(
            Type<typename T::value_type>::read_json_value(iter->value));
      } catch (std::runtime_error& e) {
        throw std::runtime_error{
            fmt::format("failed to parse field `{}`: {}", key, e.what())};
      }
    }
    return std::nullopt;
  } else {
    if (iter != json_value.MemberEnd()) {
      try {
        return Type<T>::read_json_value(iter->value);
      } catch (std::runtime_error& e) {
        throw std::runtime_error{
            fmt::format("failed to parse field `{}`: {}", key, e.what())};
      }
    }
    throw std::runtime_error{
        fmt::format("missing field `{}` in JSON object", key)};
  }
}

template <typename T>
auto write_json_value(rapidjson::Value& json_value,
                      rapidjson::Document::AllocatorType& allocator,
                      const T& value,
                      std::string_view key) -> void {
  rapidjson::Value field;

  if constexpr (is_optional<T>::value) {
    if (value.has_value()) {
      Type<typename T::value_type>::write_json_value(
          field, allocator, value.value());
    }
  } else {
    Type<T>::write_json_value(field, allocator, value);
  }

  json_value.AddMember(
      rapidjson::Value{key.data(), static_cast<rapidjson::SizeType>(key.size())}
          .Move(),
      field.Move(),
      allocator);
}

}  // namespace simulator::core::json

template <>
struct fmt::formatter<rapidjson::Type> : fmt::formatter<std::string_view> {
  auto format(rapidjson::Type type, format_context& ctx) const
      -> format_context::iterator {
    std::string_view type_str;
    switch (type) {
      case rapidjson::Type::kNullType:
        type_str = "rapidjson::Type::kNullType";
        break;
      case rapidjson::Type::kFalseType:
        type_str = "rapidjson::Type::kFalseType";
        break;
      case rapidjson::Type::kTrueType:
        type_str = "rapidjson::Type::kTrueType";
        break;
      case rapidjson::Type::kObjectType:
        type_str = "rapidjson::Type::kObjectType";
        break;
      case rapidjson::Type::kArrayType:
        type_str = "rapidjson::Type::kArrayType";
        break;
      case rapidjson::Type::kStringType:
        type_str = "rapidjson::Type::kStringType";
        break;
      case rapidjson::Type::kNumberType:
        type_str = "rapidjson::Type::kNumberType";
        break;
    }
    return fmt::formatter<std::string_view>::format(type_str, ctx);
  }
};

#endif  // SIMULATOR_CORE_COMMON_JSON_TYPE_HPP_
