#ifndef SIMULATOR_CORE_VERSION_HPP_
#define SIMULATOR_CORE_VERSION_HPP_

#include <string_view>

namespace simulator::core {

[[nodiscard]]
auto version() noexcept -> std::string_view;

[[nodiscard]]
auto major_version() noexcept -> std::string_view;

}  // namespace simulator::core

#endif  // SIMULATOR_CORE_VERSION_HPP_