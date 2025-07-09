#include "core/common/json/type.hpp"

#include <date/date.h>
#include <fmt/chrono.h>
#include <fmt/format.h>

#include "core/common/unreachable.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"

namespace simulator::core::json {

namespace {

auto report_invalid_type(rapidjson::Type actual, std::string_view expected)
    -> void {
  throw std::runtime_error{fmt::format(
      "unexpected data Type `{}`, `{}` is expected", actual, expected)};
}

}  // namespace

template <>
auto Type<bool>::read_json_value(const rapidjson::Value& json_value) -> bool {
  if (json_value.IsBool()) {
    return json_value.GetBool();
  }
  report_invalid_type(json_value.GetType(), "bool");
  unreachable();
}

template <>
auto Type<bool>::write_json_value(
    rapidjson::Value& json_value,
    [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
    const bool& value) -> void {
  json_value.SetBool(value);
}

template <>
auto Type<unsigned int>::read_json_value(const rapidjson::Value& json_value)
    -> unsigned int {
  if (json_value.IsUint()) {
    return json_value.GetUint();
  }
  report_invalid_type(json_value.GetType(), "unsigned int");
  unreachable();
}

template <>
auto Type<unsigned int>::write_json_value(
    rapidjson::Value& json_value,
    [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
    const unsigned int& value) -> void {
  json_value.SetUint(value);
}

template <>
auto Type<int>::read_json_value(const rapidjson::Value& json_value) -> int {
  if (json_value.IsInt()) {
    return json_value.GetInt();
  }
  report_invalid_type(json_value.GetType(), "int");
  unreachable();
}

template <>
auto Type<int>::write_json_value(
    rapidjson::Value& json_value,
    [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
    const int& value) -> void {
  json_value.SetInt(value);
}

template <>
auto Type<std::uint64_t>::read_json_value(const rapidjson::Value& json_value)
    -> std::uint64_t {
  if (json_value.IsUint64()) {
    return json_value.GetUint64();
  }
  report_invalid_type(json_value.GetType(), "uint64");
  unreachable();
}

template <>
auto Type<std::uint64_t>::write_json_value(
    rapidjson::Value& json_value,
    [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
    const std::uint64_t& value) -> void {
  json_value.SetUint64(value);
}

template <>
auto Type<std::int64_t>::read_json_value(const rapidjson::Value& json_value)
    -> std::int64_t {
  if (json_value.IsInt64()) {
    return json_value.GetInt64();
  }
  report_invalid_type(json_value.GetType(), "int64");
  unreachable();
}

template <>
auto Type<std::int64_t>::write_json_value(
    rapidjson::Value& json_value,
    [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
    const std::int64_t& value) -> void {
  json_value.SetInt64(value);
}

template <>
auto Type<double>::read_json_value(const rapidjson::Value& json_value)
    -> double {
  if (json_value.IsDouble()) {
    return json_value.GetDouble();
  }
  report_invalid_type(json_value.GetType(), "double");
  unreachable();
}

template <>
auto Type<double>::write_json_value(
    rapidjson::Value& json_value,
    [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
    const double& value) -> void {
  json_value.SetDouble(value);
}

template <>
auto Type<std::string>::read_json_value(const rapidjson::Value& json_value)
    -> std::string {
  if (json_value.IsString()) {
    return {json_value.GetString(), json_value.GetStringLength()};
  }
  report_invalid_type(json_value.GetType(), "string");
  unreachable();
}

template <>
auto Type<std::string>::write_json_value(
    rapidjson::Value& json_value,
    rapidjson::Document::AllocatorType& allocator,
    const std::string& value) -> void {
  json_value.SetString(
      value.data(), static_cast<rapidjson::SizeType>(value.size()), allocator);
}

template <>
auto Type<std::string_view>::read_json_value(const rapidjson::Value& json_value)
    -> std::string_view {
  if (json_value.IsString()) {
    return {json_value.GetString(), json_value.GetStringLength()};
  }
  report_invalid_type(json_value.GetType(), "string_view");
  unreachable();
}

template <>
auto Type<std::string_view>::write_json_value(
    rapidjson::Value& json_value,
    [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
    const std::string_view& value) -> void {
  json_value.SetString(value.data(),
                       static_cast<rapidjson::SizeType>(value.size()));
}

template <>
auto Type<core::sys_us>::read_json_value(const rapidjson::Value& json_value)
    -> core::sys_us {
  const auto str = Type<std::string_view>::read_json_value(json_value);

  std::stringstream stream;
  stream << str;
  core::sys_us time;
  stream >> date::parse("%F %T", time);

  if (stream.fail() || stream.bad()) {
    throw std::runtime_error{fmt::format("failed to parse sys_us: {}", str)};
  }

  return time;
}

template <>
auto Type<core::sys_us>::write_json_value(
    rapidjson::Value& json_value,
    rapidjson::Document::AllocatorType& allocator,
    const core::sys_us& value) -> void {
  Type<std::string>::write_json_value(
      json_value, allocator, fmt::format("{:%F %T}", value));
}

template <>
auto Type<core::local_days>::read_json_value(const rapidjson::Value& json_value)
    -> core::local_days {
  const auto str = Type<std::string_view>::read_json_value(json_value);

  std::stringstream stream;
  stream << str;
  date::local_days days;
  stream >> date::parse("%F", days);

  if (stream.fail() || stream.bad()) {
    throw std::runtime_error{fmt::format("failed to parse sys_days: {}", str)};
  }

  return core::local_days{days.time_since_epoch()};
}

template <>
auto Type<core::local_days>::write_json_value(
    rapidjson::Value& json_value,
    rapidjson::Document::AllocatorType& allocator,
    const core::local_days& value) -> void {
  Type<std::string>::write_json_value(
      json_value, allocator, fmt::format("{:%F}", value));
}

}  // namespace simulator::core::json
