#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_TIME_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_TIME_HPP_

#include <date/date.h>

#include <chrono>
#include <sstream>
#include <string>
#include <string_view>

namespace simulator::generator::historical {

using Timepoint = std::chrono::system_clock::time_point;
using Duration = std::chrono::milliseconds;

namespace time {

constexpr std::string_view DefaultFormat{"%F %T"};

constexpr inline auto make_offset(historical::Timepoint historical_time,
                                  historical::Timepoint current_time)
    -> historical::Duration {
  return std::chrono::duration_cast<historical::Duration>(current_time -
                                                          historical_time);
}

inline auto make_offset(historical::Timepoint historical_time) noexcept
    -> historical::Duration {
  const auto now = std::chrono::system_clock::now();
  return make_offset(historical_time, now);
}

inline auto from(std::stringstream& stream, historical::Timepoint& dest)
    -> bool {
  if (stream.bad() || stream.fail()) {
    return false;
  }

  stream >> date::parse(time::DefaultFormat.data(), dest);

  const bool converted = !stream.fail();
  if (converted) {
    dest = std::chrono::time_point_cast<historical::Duration>(dest);
  }

  return converted;
}

inline auto from(std::string_view string_view, historical::Timepoint& dest)
    -> bool {
  std::stringstream stream{};
  stream << string_view;
  return from(stream, dest);
}

inline auto from(const std::string& string, historical::Timepoint& dest)
    -> bool {
  return from(std::string_view{string}, dest);
}

inline auto to_string(historical::Timepoint timepoint, std::string& dest)
    -> bool {
  std::stringstream stream{};
  try {
    stream << date::format(time::DefaultFormat.data(), timepoint);
  } catch (...) {
    return false;
  }

  dest = stream.str();
  return true;
}

}  // namespace time

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_TIME_HPP_
