#include "api/exceptions/exceptions.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <string>
#include <string_view>

namespace simulator::data_layer {

ConfigurationError::ConfigurationError(const std::string& message) noexcept
    : std::runtime_error{message} {}

ConnectionFailure::ConnectionFailure(std::string_view driver_message) noexcept
    : std::runtime_error{format_message(driver_message)} {}

auto ConnectionFailure::format_message(std::string_view error) noexcept
    -> std::string {
  std::string escaped{error};
  constexpr char space = ' ';
  std::ranges::replace_if(
      escaped, [](auto ch) { return ch == '\n' || ch == '\r'; }, space);
  return fmt::format("database connection error occurred: {}", escaped);
}

DataDecodingError::DataDecodingError(const std::string& message) noexcept
    : std::runtime_error(message) {}

MalformedPatch::MalformedPatch(const std::string& message) noexcept
    : std::invalid_argument(message) {}

CardinalityViolationError::CardinalityViolationError() noexcept
    : std::out_of_range(
          "database operation affected an unexpected number of records") {}

DataIntegrityError::DataIntegrityError(std::string_view constraint) noexcept
    : std::runtime_error(format_message(constraint)) {}

auto DataIntegrityError::format_message(std::string_view constraint) noexcept
    -> std::string {
  return fmt::format("requested operation violates {} constraint", constraint);
}

InternalError::InternalError(std::string_view message) noexcept
    : std::logic_error(format_message(message)) {}

auto InternalError::format_message(std::string_view message) noexcept
    -> std::string {
  return fmt::format("data access level internal error occurred: {}", message);
}

}  // namespace simulator::data_layer