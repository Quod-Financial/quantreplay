#ifndef SIMULATOR_RUNTIME_IH_LOOP_IMPL_HPP_
#define SIMULATOR_RUNTIME_IH_LOOP_IMPL_HPP_

#include <chrono>
#include <functional>

#include "runtime/loop.hpp"

namespace simulator::trading_system::runtime {

class Loop::Implementation {
 public:
  Implementation() = default;
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) noexcept = delete;
  virtual ~Implementation() noexcept = default;

  auto operator=(const Implementation&) -> Implementation& = delete;
  auto operator=(Implementation&&) noexcept -> Implementation& = delete;

  virtual auto add(std::function<void()> task) -> void = 0;

  virtual auto run() -> void = 0;

  virtual auto terminate() -> void = 0;
};

}  // namespace simulator::trading_system::runtime

#endif  // SIMULATOR_RUNTIME_IH_LOOP_IMPL_HPP_
