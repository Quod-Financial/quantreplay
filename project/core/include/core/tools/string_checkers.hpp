#ifndef SIMULATOR_CORE_TOOLS_STRING_CHECKERS_HPP_
#define SIMULATOR_CORE_TOOLS_STRING_CHECKERS_HPP_

#include <string_view>

namespace simulator::core {

[[nodiscard]] auto represents_base10_unsigned_int(
    std::string_view string) noexcept -> bool;

}  // namespace simulator::core

#endif  // SIMULATOR_CORE_TOOLS_STRING_CHECKERS_HPP_
