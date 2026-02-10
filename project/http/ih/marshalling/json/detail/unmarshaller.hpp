#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_UNMARSHALLER_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_UNMARSHALLER_HPP_

#include <fmt/format.h>
#include <rapidjson/document.h>

#include <concepts>
#include <cstdint>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "core/common/meta.hpp"
#include "ih/marshalling/json/detail/enumeration_resolver.hpp"
#include "ih/marshalling/json/detail/key_resolver.hpp"
#include "ih/marshalling/json/detail/traits.hpp"

namespace simulator::http::json {

class Unmarshaller {
 public:
  explicit Unmarshaller(const rapidjson::Value& value);

  auto operator()(std::string_view key, bool& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> &&
             (std::same_as<std::remove_cvref_t<T>, bool> ||
              std::same_as<std::remove_cvref_t<T>, std::optional<bool>>)
  auto operator()(Attribute attribute, T& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> &&
             (std::same_as<std::remove_cvref_t<T>, char> ||
              std::same_as<std::remove_cvref_t<T>, std::optional<char>>)
  auto operator()(Attribute attribute, T& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> &&
             (std::same_as<std::remove_cvref_t<T>, double> ||
              std::same_as<std::remove_cvref_t<T>, std::optional<double>>)
  auto operator()(Attribute attribute, T& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> &&
             (std::same_as<std::remove_cvref_t<T>, std::string> ||
              std::same_as<std::remove_cvref_t<T>, std::optional<std::string>>)
  auto operator()(Attribute attribute, T& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> && is_marshallable_int_v<T>
  auto operator()(Attribute attribute, T& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> && is_marshallable_int_v<T>
  auto operator()(Attribute attribute, std::optional<T>& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> && is_marshallable_uint_v<T>
  auto operator()(Attribute attribute, T& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> && is_marshallable_uint_v<T>
  auto operator()(Attribute attribute, std::optional<T>& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> && std::is_enum_v<T>
  auto operator()(Attribute attribute, T& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> && std::is_enum_v<T>
  auto operator()(Attribute attribute, std::optional<T>& value) -> bool;

 private:
  auto try_get_member(std::string_view key) -> const rapidjson::Value*;

  template <typename T, typename Checker, typename Getter>
  auto get_value(std::string_view key,
                 Checker check,
                 Getter get,
                 std::string_view type_name) -> std::optional<T>;

  template <typename T, typename Checker, typename Getter>
  auto get_optional_value(std::string_view key,
                          Checker check,
                          Getter get,
                          std::string_view type_name)
      -> std::optional<std::optional<T>>;

  auto get_boolean(std::string_view key) -> std::optional<bool>;

  auto get_optional_boolean(std::string_view key)
      -> std::optional<std::optional<bool>>;

  auto get_integer(std::string_view key) -> std::optional<std::int64_t>;

  auto get_optional_integer(std::string_view key)
      -> std::optional<std::optional<std::int64_t>>;

  auto get_uinteger(std::string_view key) -> std::optional<std::uint64_t>;

  auto get_optional_uinteger(std::string_view key)
      -> std::optional<std::optional<std::uint64_t>>;

  auto get_double(std::string_view key) -> std::optional<double>;

  auto get_optional_double(std::string_view key)
      -> std::optional<std::optional<double>>;

  auto get_string(std::string_view key) -> std::optional<std::string>;

  auto get_optional_string(std::string_view key)
      -> std::optional<std::optional<std::string>>;

  auto get_character(std::string_view key) -> std::optional<char>;

  auto get_optional_character(std::string_view key)
      -> std::optional<std::optional<char>>;

  auto json_object() -> const rapidjson::Value&;

  template <typename Target, typename Source>
  static auto cast_integer(std::string_view key, Source integer)
      -> std::remove_cvref_t<Target>;

  template <typename Attribute>
  static auto resolve_key(Attribute attribute) -> std::string_view;

  [[noreturn]]
  static auto report_invalid_type(std::string_view key,
                                  std::string_view expected_type) -> void;

  std::reference_wrapper<const rapidjson::Value> json_object_;
};

inline Unmarshaller::Unmarshaller(const rapidjson::Value& value)
    : json_object_(value) {
  if (!value.IsObject()) {
    throw std::logic_error{
        "trying to initialize an unmarshaller with a JSON value which is "
        "not an object"};
  }
}

inline auto Unmarshaller::operator()(std::string_view key, bool& value)
    -> bool {
  if (const auto boolean = get_boolean(key)) {
    value = *boolean;
    return true;
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> &&
           (std::same_as<std::remove_cvref_t<T>, bool> ||
            std::same_as<std::remove_cvref_t<T>, std::optional<bool>>)
auto Unmarshaller::operator()(Attribute attribute, T& value) -> bool {
  const auto key = resolve_key(attribute);
  if constexpr (core::is_optional_v<std::remove_cvref_t<T>>) {
    if (auto result = get_optional_boolean(key)) {
      value = std::move(*result);
      return true;
    }
  } else {
    if (auto result = get_boolean(key)) {
      value = *result;
      return true;
    }
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> &&
           (std::same_as<std::remove_cvref_t<T>, char> ||
            std::same_as<std::remove_cvref_t<T>, std::optional<char>>)
auto Unmarshaller::operator()(Attribute attribute, T& value) -> bool {
  const auto key = resolve_key(attribute);
  if constexpr (core::is_optional_v<std::remove_cvref_t<T>>) {
    if (auto result = get_optional_character(key)) {
      value = std::move(*result);
      return true;
    }
  } else {
    if (auto result = get_character(key)) {
      value = *result;
      return true;
    }
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> &&
           (std::same_as<std::remove_cvref_t<T>, double> ||
            std::same_as<std::remove_cvref_t<T>, std::optional<double>>)
auto Unmarshaller::operator()(Attribute attribute, T& value) -> bool {
  const auto key = resolve_key(attribute);
  if constexpr (core::is_optional_v<std::remove_cvref_t<T>>) {
    if (auto result = get_optional_double(key)) {
      value = std::move(*result);
      return true;
    }
  } else {
    if (auto result = get_double(key)) {
      value = *result;
      return true;
    }
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> &&
           (std::same_as<std::remove_cvref_t<T>, std::string> ||
            std::same_as<std::remove_cvref_t<T>, std::optional<std::string>>)
auto Unmarshaller::operator()(Attribute attribute, T& value) -> bool {
  const auto key = resolve_key(attribute);
  if constexpr (core::is_optional_v<std::remove_cvref_t<T>>) {
    if (auto result = get_optional_string(key)) {
      value = std::move(*result);
      return true;
    }
  } else {
    if (auto result = get_string(key)) {
      value = std::move(*result);
      return true;
    }
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> && is_marshallable_int_v<T>
auto Unmarshaller::operator()(Attribute attribute, T& value) -> bool {
  const auto key = resolve_key(attribute);
  if (auto integer = get_integer(key)) {
    value = cast_integer<T>(key, *integer);
    return true;
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> && is_marshallable_int_v<T>
auto Unmarshaller::operator()(Attribute attribute, std::optional<T>& value)
    -> bool {
  const auto key = resolve_key(attribute);
  if (auto integer = get_optional_integer(key)) {
    if (integer->has_value()) {
      value = cast_integer<T>(key, integer->value());
    } else {
      value.reset();
    }
    return true;
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> && is_marshallable_uint_v<T>
auto Unmarshaller::operator()(Attribute attribute, T& value) -> bool {
  const auto key = resolve_key(attribute);
  if (auto unsigned_integer = get_uinteger(key)) {
    value = cast_integer<T>(key, *unsigned_integer);
    return true;
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> && is_marshallable_uint_v<T>
auto Unmarshaller::operator()(Attribute attribute, std::optional<T>& value)
    -> bool {
  const auto key = resolve_key(attribute);
  if (auto unsigned_integer = get_optional_uinteger(key)) {
    if (unsigned_integer->has_value()) {
      value = cast_integer<T>(key, unsigned_integer->value());
    } else {
      value.reset();
    }
    return true;
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> && std::is_enum_v<T>
auto Unmarshaller::operator()(Attribute attribute, T& value) -> bool {
  std::string decoded_string;
  if ((*this)(attribute, decoded_string)) {
    EnumerationResolver::resolve(decoded_string, value);
    return true;
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> && std::is_enum_v<T>
auto Unmarshaller::operator()(Attribute attribute, std::optional<T>& value)
    -> bool {
  std::optional<std::string> decoded_string;
  if ((*this)(attribute, decoded_string)) {
    if (decoded_string.has_value()) {
      T enum_value;
      EnumerationResolver::resolve(*decoded_string, enum_value);
      value = enum_value;
    } else {
      value.reset();
    }
    return true;
  }
  return false;
}

inline auto Unmarshaller::try_get_member(std::string_view key)
    -> const rapidjson::Value* {
  if (!json_object().HasMember(key.data())) {
    return nullptr;
  }
  return &json_object()[key.data()];
}

template <typename T, typename Checker, typename Getter>
auto Unmarshaller::get_value(std::string_view key,
                             Checker check,
                             Getter get,
                             std::string_view type_name) -> std::optional<T> {
  const auto* member = try_get_member(key);
  if (!member) {
    return std::nullopt;
  }

  if (check(*member)) {
    return std::optional<T>{get(*member)};
  }

  report_invalid_type(key, type_name);
}

template <typename T, typename Checker, typename Getter>
auto Unmarshaller::get_optional_value(std::string_view key,
                                      Checker check,
                                      Getter get,
                                      std::string_view type_name)
    -> std::optional<std::optional<T>> {
  const auto* member = try_get_member(key);
  if (!member) {
    return std::nullopt;
  }

  if (member->IsNull()) {
    return std::optional<T>{std::nullopt};
  }

  if (check(*member)) {
    return std::optional<T>{get(*member)};
  }

  report_invalid_type(key, fmt::format("{} or null", type_name));
}

inline auto Unmarshaller::get_boolean(std::string_view key)
    -> std::optional<bool> {
  return get_value<bool>(
      key,
      [](const auto& v) { return v.IsBool(); },
      [](const auto& v) { return v.GetBool(); },
      "boolean");
}

inline auto Unmarshaller::get_optional_boolean(std::string_view key)
    -> std::optional<std::optional<bool>> {
  return get_optional_value<bool>(
      key,
      [](const auto& v) { return v.IsBool(); },
      [](const auto& v) { return v.GetBool(); },
      "boolean");
}

inline auto Unmarshaller::get_integer(std::string_view key)
    -> std::optional<std::int64_t> {
  return get_value<std::int64_t>(
      key,
      [](const auto& v) { return v.IsInt64(); },
      [](const auto& v) { return v.GetInt64(); },
      "integer");
}

inline auto Unmarshaller::get_optional_integer(std::string_view key)
    -> std::optional<std::optional<std::int64_t>> {
  return get_optional_value<std::int64_t>(
      key,
      [](const auto& v) { return v.IsInt64(); },
      [](const auto& v) { return v.GetInt64(); },
      "integer");
}

inline auto Unmarshaller::get_uinteger(std::string_view key)
    -> std::optional<std::uint64_t> {
  return get_value<std::uint64_t>(
      key,
      [](const auto& v) { return v.IsUint64(); },
      [](const auto& v) { return v.GetUint64(); },
      "unsigned integer");
}

inline auto Unmarshaller::get_optional_uinteger(std::string_view key)
    -> std::optional<std::optional<std::uint64_t>> {
  return get_optional_value<std::uint64_t>(
      key,
      [](const auto& v) { return v.IsUint64(); },
      [](const auto& v) { return v.GetUint64(); },
      "unsigned integer");
}

inline auto Unmarshaller::get_double(std::string_view key)
    -> std::optional<double> {
  const auto* member = try_get_member(key);
  if (!member) {
    return std::nullopt;
  }

  if (member->IsDouble()) {
    return member->GetDouble();
  }
  if (member->IsInt64()) {
    return static_cast<double>(member->GetInt64());
  }
  if (member->IsUint64()) {
    return static_cast<double>(member->GetUint64());
  }

  report_invalid_type(key, "double-precision floating point");
}

inline auto Unmarshaller::get_optional_double(std::string_view key)
    -> std::optional<std::optional<double>> {
  const auto* member = try_get_member(key);
  if (!member) {
    return std::nullopt;
  }

  if (member->IsNull()) {
    return std::optional<double>{std::nullopt};
  }
  if (member->IsDouble()) {
    return std::make_optional<double>(member->GetDouble());
  }
  if (member->IsInt64()) {
    return std::make_optional<double>(member->GetInt64());
  }
  if (member->IsUint64()) {
    return std::make_optional<double>(member->GetUint64());
  }

  report_invalid_type(key, "double-precision floating point or null");
}

inline auto Unmarshaller::get_string(std::string_view key)
    -> std::optional<std::string> {
  return get_value<std::string>(
      key,
      [](const auto& v) { return v.IsString(); },
      [](const auto& v) {
        return std::string{v.GetString(), v.GetStringLength()};
      },
      "string");
}

inline auto Unmarshaller::get_optional_string(std::string_view key)
    -> std::optional<std::optional<std::string>> {
  return get_optional_value<std::string>(
      key,
      [](const auto& v) { return v.IsString(); },
      [](const auto& v) {
        return std::string{v.GetString(), v.GetStringLength()};
      },
      "string");
}

inline auto Unmarshaller::get_character(std::string_view key)
    -> std::optional<char> {
  auto str = get_string(key);
  if (!str) {
    return std::nullopt;
  }

  if (str->size() == 1) {
    return str->front();
  }

  throw std::runtime_error{fmt::format(
      "invalid string received for `{}', a string with exactly 1 character "
      "is expected",
      key)};
}

inline auto Unmarshaller::get_optional_character(std::string_view key)
    -> std::optional<std::optional<char>> {
  auto str = get_optional_string(key);
  if (!str) {
    return std::nullopt;
  }

  if (!str->has_value()) {
    return std::optional<char>{std::nullopt};
  }

  if ((*str)->size() == 1) {
    return std::optional<char>{(*str)->front()};
  }

  throw std::runtime_error{fmt::format(
      "invalid string received for `{}', a string with exactly 1 character "
      "is expected or null",
      key)};
}

inline auto Unmarshaller::json_object() -> const rapidjson::Value& {
  return json_object_.get();
}

template <typename Target, typename Source>
auto Unmarshaller::cast_integer(std::string_view key, Source integer)
    -> std::remove_cvref_t<Target> {
  using TargetType = std::remove_cvref_t<Target>;
  constexpr auto min = std::numeric_limits<TargetType>::min();
  constexpr auto max = std::numeric_limits<TargetType>::max();

  if (min <= integer && integer <= max) {
    return static_cast<TargetType>(integer);
  }

  if (integer < min) {
    throw std::runtime_error{
        fmt::format("invalid integer received for `{}', value is less than "
                    "supported minimal value {}",
                    key,
                    min)};
  }

  throw std::runtime_error{
      fmt::format("invalid integer received for `{}', value is greater than "
                  "supported maximal value {}",
                  key,
                  max)};
}

template <typename Attribute>
inline auto Unmarshaller::resolve_key(Attribute attribute) -> std::string_view {
  static_assert(
      std::is_enum_v<Attribute>,
      "Given Attribute type is not an enumeration type, thus could not "
      "be resolved as JSON document key");

  return KeyResolver::resolve_key(attribute);
}

inline auto Unmarshaller::report_invalid_type(std::string_view key,
                                              std::string_view expected_type)
    -> void {
  throw std::runtime_error{
      fmt::format("unexpected data type received for `{}', {} is expected",
                  key,
                  expected_type)};
}

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_UNMARSHALLER_HPP_
