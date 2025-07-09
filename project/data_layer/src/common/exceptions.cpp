#include "ih/common/exceptions.hpp"

#include <fmt/format.h>

#include <stdexcept>
#include <string>
#include <string_view>

namespace simulator::data_layer {

ConnectionPropertyMissing::ConnectionPropertyMissing(
    std::string_view property) noexcept
    : ConfigurationError(format_message(property)) {}

auto ConnectionPropertyMissing::format_message(
    std::string_view connection_property) noexcept -> std::string {
  return fmt::format("required database connection property `{}' is absent",
                     connection_property);
}

RequiredAttributeMissing::RequiredAttributeMissing(
    std::string_view model_name, std::string_view attribute_name) noexcept
    : MalformedPatch(format_message(model_name, attribute_name)) {}

auto RequiredAttributeMissing::format_message(
    std::string_view model_name, std::string_view attribute_name) noexcept
    -> std::string {
  return fmt::format(
      "can not create a {} model, "
      "required {} attribute is missing in the patch",
      model_name,
      attribute_name);
}

UnspecifiedAttributeAccess::UnspecifiedAttributeAccess(
    std::string_view model_name, std::string_view attribute_name) noexcept
    : std::logic_error(format_message(model_name, attribute_name)) {}

auto UnspecifiedAttributeAccess::format_message(
    std::string_view model, std::string_view attribute) noexcept
    -> std::string {
  return fmt::format(
      "illegal access attempt to an unspecified {}.{} attribute value",
      model,
      attribute);
}

DataEncodingError::DataEncodingError(std::string_view message) noexcept
    : InternalError(format_message(message)) {}

auto DataEncodingError::format_message(std::string_view message) noexcept
    -> std::string {
  return fmt::format("failed to encode data into DBMS format - {}", message);
}

EnumDecodingError::EnumDecodingError(std::string_view enum_name,
                                     std::string_view bad_value) noexcept
    : DataDecodingError(format_message(enum_name, bad_value)) {}

auto EnumDecodingError::format_message(std::string_view enum_name,
                                       std::string_view bad_value) noexcept
    -> std::string {
  return fmt::format(
      "can not convert `{}' to {} enumerable type", bad_value, enum_name);
}

ColumnNameDecodingError::ColumnNameDecodingError(
    std::string_view bad_database_column_name) noexcept
    : DataDecodingError(format_message(bad_database_column_name)) {}

auto ColumnNameDecodingError::format_message(
    std::string_view bad_database_column_name) noexcept -> std::string {
  return fmt::format("can not decode `{}' database column name",
                     bad_database_column_name);
}

}  // namespace simulator::data_layer