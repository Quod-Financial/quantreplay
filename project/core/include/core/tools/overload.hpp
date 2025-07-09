#ifndef SIMULATOR_CORE_TOOLS_OVERLOAD_HPP_
#define SIMULATOR_CORE_TOOLS_OVERLOAD_HPP_

#include <utility>

namespace simulator::core {

template <typename... Overloads>
[[nodiscard]]
constexpr auto overload(Overloads&&... overloads) {
  struct Overload : Overloads... {
    using Overloads::operator()...;
  };
  return Overload{std::forward<Overloads>(overloads)...};
}

}  // namespace simulator::core

#endif  // SIMULATOR_CORE_TOOLS_OVERLOAD_HPP_
