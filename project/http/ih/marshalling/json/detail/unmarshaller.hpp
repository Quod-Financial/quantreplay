#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_UNMARSHALLER_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_UNMARSHALLER_HPP_

#include <fmt/format.h>
#include <rapidjson/document.h>

#include <cstdint>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "ih/marshalling/json/detail/enumeration_resolver.hpp"
#include "ih/marshalling/json/detail/key_resolver.hpp"
#include "ih/marshalling/json/detail/traits.hpp"

namespace simulator::http::json {

class Unmarshaller {
 public:
  explicit Unmarshaller(const rapidjson::Value& value);

  auto operator()(std::string_view key, bool& value) -> bool;

  template <typename Attribute>
    requires std::is_enum_v<Attribute>
  auto operator()(Attribute attribute, bool& value) -> bool;

  template <typename Attribute>
    requires std::is_enum_v<Attribute>
  auto operator()(Attribute attribute, char& value) -> bool;

  template <typename Attribute>
    requires std::is_enum_v<Attribute>
  auto operator()(Attribute attribute, double& value) -> bool;

  template <typename Attribute>
    requires std::is_enum_v<Attribute>
  auto operator()(Attribute attribute, std::string& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> && is_marshallable_int_v<T>
  auto operator()(Attribute attribute, T& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> && is_marshallable_uint_v<T>
  auto operator()(Attribute attribute, T& value) -> bool;

  template <typename Attribute, typename T>
    requires std::is_enum_v<Attribute> && std::is_enum_v<T>
  auto operator()(Attribute attribute, T& value) -> bool;

 private:
  auto get_boolean(std::string_view key) -> std::optional<bool>;

  auto get_integer(std::string_view key) -> std::optional<std::int64_t>;

  auto get_uinteger(std::string_view key) -> std::optional<std::uint64_t>;

  auto get_double(std::string_view key) -> std::optional<double>;

  auto get_string(std::string_view key) -> std::optional<std::string>;

  auto get_character(std::string_view key) -> std::optional<char>;

  auto json_object() -> const rapidjson::Value&;

  template <typename Target, typename Source>
  static auto cast_integer(std::string_view key, Source integer)
      -> std::remove_cv_t<std::remove_reference_t<Target>>;

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

template <typename Attribute>
  requires std::is_enum_v<Attribute>
inline auto Unmarshaller::operator()(Attribute attribute, bool& value) -> bool {
  const std::string_view key = resolve_key(attribute);
  return operator()(key, value);
}

template <typename Attribute>
  requires std::is_enum_v<Attribute>
inline auto Unmarshaller::operator()(Attribute attribute, char& value) -> bool {
  const std::string_view key = resolve_key(attribute);
  if (const auto character = get_character(key)) {
    value = *character;
    return true;
  }
  return false;
}

template <typename Attribute>
  requires std::is_enum_v<Attribute>
inline auto Unmarshaller::operator()(Attribute attribute, double& value)
    -> bool {
  const std::string_view key = resolve_key(attribute);
  if (const auto number = get_double(key)) {
    value = *number;
    return true;
  }
  return false;
}

template <typename Attribute>
  requires std::is_enum_v<Attribute>
inline auto Unmarshaller::operator()(Attribute attribute, std::string& value)
    -> bool {
  const std::string_view key = resolve_key(attribute);
  if (auto str = get_string(key)) {
    value = std::move(*str);
    return true;
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> && is_marshallable_int_v<T>
inline auto Unmarshaller::operator()(Attribute attribute, T& value) -> bool {
  const std::string_view key = resolve_key(attribute);
  if (auto integer = get_integer(key)) {
    value = cast_integer<T>(key, *integer);
    return true;
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> && is_marshallable_uint_v<T>
inline auto Unmarshaller::operator()(Attribute attribute, T& value) -> bool {
  const std::string_view key = resolve_key(attribute);
  if (auto unsinged_integer = get_uinteger(key)) {
    value = cast_integer<T>(key, *unsinged_integer);
    return true;
  }
  return false;
}

template <typename Attribute, typename T>
  requires std::is_enum_v<Attribute> && std::is_enum_v<T>
inline auto Unmarshaller::operator()(Attribute attribute, T& value) -> bool {
  std::string decoded_string;
  if ((*this)(attribute, decoded_string)) {
    EnumerationResolver::resolve(decoded_string, value);
    return true;
  }
  return false;
}

inline auto Unmarshaller::get_boolean(std::string_view key)
    -> std::optional<bool> {
  if (!json_object().HasMember(key.data())) {
    return std::nullopt;
  }

  const auto& value = json_object()[key.data()];
  if (value.IsBool()) {
    return std::make_optional<bool>(value.GetBool());
  }

  report_invalid_type(key, "boolean");
}

inline auto Unmarshaller::get_integer(std::string_view key)
    -> std::optional<std::int64_t> {
  if (!json_object().HasMember(key.data())) {
    return std::nullopt;
  }

  const auto& value = json_object()[key.data()];
  if (value.IsInt64()) {
    return std::make_optional<std::int64_t>(value.GetInt64());
  }

  report_invalid_type(key, "integer");
}

inline auto Unmarshaller::get_uinteger(std::string_view key)
    -> std::optional<std::uint64_t> {
  if (!json_object().HasMember(key.data())) {
    return std::nullopt;
  }

  const auto& value = json_object()[key.data()];
  if (value.IsUint64()) {
    return std::make_optional<std::uint64_t>(value.GetUint64());
  }

  report_invalid_type(key, "unsigned integer");
}

inline auto Unmarshaller::get_double(std::string_view key)
    -> std::optional<double> {
  if (!json_object().HasMember(key.data())) {
    return std::nullopt;
  }

  const auto& value = json_object()[key.data()];
  std::optional<double> decoded_value;
  if (value.IsDouble()) {
    decoded_value = std::make_optional<double>(value.GetDouble());
  } else if (value.IsInt64()) {
    decoded_value = std::make_optional<double>(value.GetInt64());
  } else if (value.IsUint64()) {
    decoded_value = std::make_optional<double>(value.GetUint64());
  }

  if (decoded_value.has_value()) {
    return decoded_value;
  }
  report_invalid_type(key, "double-precision floating point");
}

inline auto Unmarshaller::get_string(std::string_view key)
    -> std::optional<std::string> {
  if (!json_object().HasMember(key.data())) {
    return std::nullopt;
  }

  const auto& value = json_object()[key.data()];
  if (value.IsString()) {
    return std::make_optional<std::string>(value.GetString(),
                                           value.GetStringLength());
  }

  report_invalid_type(key, "string");
}

inline auto Unmarshaller::get_character(std::string_view key)
    -> std::optional<char> {
  const auto decoded_as_string = get_string(key);
  if (!decoded_as_string.has_value()) {
    return std::nullopt;
  }

  if (decoded_as_string->size() == 1) {
    return std::make_optional<char>(decoded_as_string->at(0));
  }

  throw std::runtime_error{fmt::format(
      "invalid string received for `{}', a string with exactly 1 character "
      "is expected",
      key)};
}

inline auto Unmarshaller::json_object() -> const rapidjson::Value& {
  return json_object_.get();
}

template <typename Target, typename Source>
inline auto Unmarshaller::cast_integer(std::string_view key, Source integer)
    -> std::remove_cv_t<std::remove_reference_t<Target>> {
  using TargetType = std::remove_cv_t<std::remove_reference_t<Target>>;
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
      fmt::format("invalid integer received for `{}', value is greater that "
                  "supported maximal value {}",
                  key,
                  min)};
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
