#ifndef SIMULATOR_CORE_TOOLS_FORMAT_HPP_
#define SIMULATOR_CORE_TOOLS_FORMAT_HPP_

#include <fmt/ranges.h>

#include <string>
#include <vector>

namespace simulator::core {

template <typename T>
auto format_collection(const std::vector<T>& vector) -> std::string {
  return vector.empty() ? "[]" : fmt::format("[ {} ]", fmt::join(vector, ", "));
}

}  // namespace simulator::core

#endif  // SIMULATOR_CORE_TOOLS_FORMAT_HPP_
