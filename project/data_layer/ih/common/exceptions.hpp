#ifndef SIMULATOR_PROJECT_DATA_LAYER_IH_COMMON_EXCEPTIONS_HPP_
#define SIMULATOR_PROJECT_DATA_LAYER_IH_COMMON_EXCEPTIONS_HPP_

#include <fmt/format.h>

#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

#include "api/exceptions/exceptions.hpp"

// Internal exception that are handled inside the component and are not visible
// to client code
namespace simulator::data_layer {

class ConnectionPropertyMissing : public ConfigurationError {
 public:
  explicit ConnectionPropertyMissing(std::string_view property) noexcept;

 private:
  static auto format_message(std::string_view connection_property) noexcept
      -> std::string;
};

// Thrown when client code tries to create a model from a patch with required
// attribute missing
class RequiredAttributeMissing : public MalformedPatch {
 public:
  explicit RequiredAttributeMissing(std::string_view model_name,
                                    std::string_view attribute_name) noexcept;

 private:
  static auto format_message(std::string_view model_name,
                             std::string_view attribute_name) noexcept
      -> std::string;
};

// Thrown when client code tries to access a not-initialized model attribute
class UnspecifiedAttributeAccess : public std::logic_error {
 public:
  explicit UnspecifiedAttributeAccess(std::string_view model_name,
                                      std::string_view attribute_name) noexcept;

 private:
  static auto format_message(std::string_view model,
                             std::string_view attribute) noexcept
      -> std::string;
};

class EnumDecodingError : public DataDecodingError {
 public:
  EnumDecodingError(std::string_view enum_name,
                    std::string_view bad_value) noexcept;

 private:
  static auto format_message(std::string_view enum_name,
                             std::string_view bad_value) noexcept
      -> std::string;
};

class ColumnNameDecodingError : public DataDecodingError {
 public:
  ColumnNameDecodingError(std::string_view column_name) noexcept;

 private:
  static auto format_message(std::string_view bad_column_name) noexcept
      -> std::string;
};

// General data conversion error occurred during data serialization into DBMS
// format
class DataEncodingError : public InternalError {
 public:
  explicit DataEncodingError(std::string_view message) noexcept;

 private:
  static auto format_message(std::string_view message) noexcept -> std::string;
};

class EnumEncodingError : public DataEncodingError {
 public:
  template <typename T, std::enable_if_t<std::is_enum_v<T>>* = nullptr>
  EnumEncodingError(std::string_view enum_name, T bad_value)
      : DataEncodingError(format_message(enum_name, bad_value)) {}

 private:
  template <typename T, std::enable_if_t<std::is_enum_v<T>>* = nullptr>
  static auto format_message(std::string_view enum_name, T bad_value)
      -> std::enable_if_t<std::is_enum_v<T>, std::string> {
    using UnderlyingType = std::underlying_type_t<T>;
    return fmt::format("can not encode an unknown {} enumeration value ({:#x})",
                       enum_name,
                       static_cast<UnderlyingType>(bad_value));
  }
};

class ColumnNameEncodingError : public DataEncodingError {
 public:
  template <typename T, std::enable_if_t<std::is_enum_v<T>>* = nullptr>
  ColumnNameEncodingError(std::string_view table_name, T bad_value)
      : DataEncodingError(format_message(table_name, bad_value)) {}

 private:
  template <typename T, std::enable_if_t<std::is_enum_v<T>>* = nullptr>
  static auto format_message(std::string_view table_name, T bad_attribute)
      -> std::enable_if_t<std::is_enum_v<T>, std::string> {
    using UnderlyingType = std::underlying_type_t<T>;
    return fmt::format(
        "can not encode {:#x} value into a column name for {} table",
        static_cast<UnderlyingType>(bad_attribute),
        table_name);
  }
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_PROJECT_DATA_LAYER_IH_COMMON_EXCEPTIONS_HPP_
