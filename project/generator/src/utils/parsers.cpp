#include "ih/utils/parsers.hpp"

#include <date/date.h>

#include <string>

namespace simulator::generator::parsing {

auto parse(std::stringstream& buffer,
           std::chrono::system_clock::time_point& dest,
           std::string_view format) -> bool {
  buffer >> date::parse(std::string{format}, dest);
  return !buffer.fail() && !buffer.bad();
}

auto parse(std::string_view timestamp,
           std::chrono::system_clock::time_point& dest,
           std::stringstream& buffer,
           std::string_view format) -> bool {
  buffer.clear();
  buffer << timestamp;

  const bool parsed = parse(buffer, dest, format);

  buffer.clear();
  return parsed;
}

auto parse(std::string_view timestamp,
           std::chrono::system_clock::time_point& dest,
           std::string_view format) -> bool {
  std::stringstream buffer{};
  return parse(timestamp, dest, buffer, format);
}

}  // namespace simulator::generator::parsing
