#ifndef SIMULATOR_GENERATOR_IH_UTILS_EXECUTABLE_HPP_
#define SIMULATOR_GENERATOR_IH_UTILS_EXECUTABLE_HPP_

#include <chrono>

namespace simulator::generator {

class Executable {
 public:
  virtual ~Executable() = default;

  virtual void prepare() noexcept = 0;

  virtual void execute() = 0;

  [[nodiscard]]
  virtual auto finished() const noexcept -> bool = 0;

  [[nodiscard]]
  virtual auto next_exec_timeout() const -> std::chrono::microseconds = 0;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_UTILS_EXECUTABLE_HPP_
