#ifndef SIMULATOR_GENERATOR_IH_UTILS_PARSERS_HPP_
#define SIMULATOR_GENERATOR_IH_UTILS_PARSERS_HPP_

#include <chrono>
#include <sstream>
#include <string_view>

namespace simulator::generator::parsing {

// YYYY-mm-dd HH:MM:SS.MS
constexpr std::string_view DefaultTimestampFormat{"%F %T"};

auto parse(std::stringstream& buffer,
           std::chrono::system_clock::time_point& dest,
           std::string_view format = DefaultTimestampFormat) -> bool;

auto parse(std::string_view timestamp,
           std::chrono::system_clock::time_point& dest,
           std::stringstream& buffer,
           std::string_view format = DefaultTimestampFormat) -> bool;

auto parse(std::string_view timestamp,
           std::chrono::system_clock::time_point& dest,
           std::string_view format = DefaultTimestampFormat) -> bool;

}  // namespace simulator::generator::parsing

#endif  // SIMULATOR_GENERATOR_IH_UTILS_PARSERS_HPP_
