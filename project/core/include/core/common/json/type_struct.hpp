#ifndef SIMULATOR_CORE_COMMON_JSON_TYPE_STRUCT_HPP_
#define SIMULATOR_CORE_COMMON_JSON_TYPE_STRUCT_HPP_

#include <tuple>

#include "core/common/json/field.hpp"
#include "core/common/json/type.hpp"
#include "core/common/json/type_enum.hpp"

namespace simulator::core::json {

template <typename... Tp, typename Func>
void for_each(const std::tuple<Tp...>& tuple, Func&& func) {
  std::apply([&func](const auto&... elements) { (func(elements), ...); },
             tuple);
}

template <typename T>
  requires requires { Struct<T>::fields; }
struct Type<T> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value) -> T {
    T result;

    for_each(Struct<T>::fields, [&result, &json_value](auto field) {
      using field_type = typename decltype(field)::value_type;
      auto iter = json_value.FindMember(field.name.data());

      if constexpr (is_optional<field_type>::value) {
        if (iter != json_value.MemberEnd() && !iter->value.IsNull()) {
          try {
            result.*(field.ptr) =
                Type<typename field_type::value_type>::read_json_value(
                    iter->value);
          } catch (const std::runtime_error& e) {
            throw std::runtime_error{fmt::format(
                "failed to parse field `{}`: {}", field.name, e.what())};
          }
        }
      } else {
        if (iter != json_value.MemberEnd()) {
          try {
            result.*(field.ptr) =
                Type<field_type>::read_json_value(iter->value);
          } catch (const std::runtime_error& e) {
            throw std::runtime_error{fmt::format(
                "failed to parse field `{}`: {}", field.name, e.what())};
          }
        } else {
          throw std::runtime_error{
              fmt::format("missing field `{}` in JSON object", field.name)};
        }
      }
    });
    return result;
  }

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const T& value) -> void {
    json_value = rapidjson::Value(rapidjson::Type::kObjectType);
    json_value.MemberReserve(std::tuple_size_v<decltype(Struct<T>::fields)>,
                             allocator);

    for_each(Struct<T>::fields, [&json_value, &allocator, &value](auto field) {
      using field_type = typename decltype(field)::value_type;

      rapidjson::Value item;
      if constexpr (is_optional<field_type>::value) {
        if (const auto& opt_value = (value.*(field.ptr))) {
          Type<typename field_type::value_type>::write_json_value(
              item, allocator, opt_value.value());
        }
      } else {
        Type<field_type>::write_json_value(item, allocator, value.*(field.ptr));
      }

      json_value.AddMember(
          rapidjson::Value{field.name.data(),
                           static_cast<rapidjson::SizeType>(field.name.size())}
              .Move(),
          item,
          allocator);
    });
  }
};

}  // namespace simulator::core::json

#endif  // SIMULATOR_CORE_COMMON_JSON_TYPE_STRUCT_HPP_
