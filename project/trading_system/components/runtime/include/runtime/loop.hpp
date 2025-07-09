#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_LOOP_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_LOOP_HPP_

#include <chrono>
#include <functional>
#include <memory>

namespace simulator::trading_system::runtime {

class Loop {
 public:
  class Implementation;

  [[nodiscard]]
  static auto create_one_second_rate_loop() -> Loop;

  explicit Loop(std::unique_ptr<Implementation> impl);

  Loop() = delete;
  Loop(const Loop&) = delete;
  Loop(Loop&&) noexcept;
  ~Loop() noexcept;

  auto operator=(const Loop&) -> Loop& = delete;
  auto operator=(Loop&&) noexcept -> Loop&;

  auto add(std::function<void()> task) -> void;

  auto run() -> void;
  auto terminate() -> void;

 private:
  std::unique_ptr<Implementation> impl_;
};

}  // namespace simulator::trading_system::runtime

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_LOOP_HPP_
