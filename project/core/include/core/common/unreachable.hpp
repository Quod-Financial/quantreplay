#ifndef SIMULATOR_CORE_COMMON_UNREACHABLE_HPP_
#define SIMULATOR_CORE_COMMON_UNREACHABLE_HPP_

#if defined(SIMULATOR_OPTIMIZE_UNREACHABLE)

#include <utility>

#if __cpp_lib_unreachable >= 202202L

namespace simulator::core {

[[noreturn]] inline auto unreachable() -> void { std::unreachable(); }

}  // namespace simulator::core

#else                    // !(__cpp_lib_unreachable >= 202202L)

namespace simulator::core {

[[noreturn]] inline auto unreachable() -> void {
// From https://en.cppreference.com/w/cpp/utility/unreachable
// Uses compiler specific extensions if possible.
// Even if no extension is used, undefined behavior is still raised by
// an empty function body and the noreturn attribute.
#if defined(__GNUC__)    // GCC, Clang, ICC
  __builtin_unreachable();
#elif defined(_MSC_VER)  // MSVC
  __assume(false);
#endif
}

}  // namespace simulator::core

#endif  // __cpp_lib_unreachable >= 202202L

#else  // !defined(SIMULATOR_OPTIMIZE_UNREACHABLE)

#include <iostream>
#include <source_location>

namespace simulator::core {

[[noreturn]] inline void unreachable(
    std::source_location location = std::source_location::current()) {
  std::cerr << "trap: unreachable code executed: " << location.file_name()
            << '(' << location.line() << ':' << location.column() << ") `"
            << location.function_name() << "\n"
            << std::flush;
  std::abort();
}

}  // namespace simulator::core

#endif  // defined(SIMULATOR_OPTIMIZE_UNREACHABLE)

#endif  // SIMULATOR_CORE_COMMON_UNREACHABLE_HPP_
