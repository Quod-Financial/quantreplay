#ifndef SIMULATOR_GENERATOR_IH_UTILS_EXECUTOR_HPP_
#define SIMULATOR_GENERATOR_IH_UTILS_EXECUTOR_HPP_

#include <atomic>
#include <memory>
#include <thread>

#include "ih/context/component_context.hpp"
#include "ih/utils/executable.hpp"

namespace simulator::generator {

class Executor {
 public:
  Executor() = delete;

  Executor(std::unique_ptr<Executable> executable,
           std::shared_ptr<ComponentContext> global_context) noexcept;

  static auto create(std::unique_ptr<Executable> executable,
                     std::shared_ptr<ComponentContext> global_context)
      -> std::unique_ptr<Executor>;

  auto launch() -> void;

  auto terminate() noexcept -> void;

 private:
  [[nodiscard]]
  auto is_executing() const noexcept -> bool;

  auto start() -> void;

  auto reset_executing_thread() noexcept -> void;

  auto execute() noexcept -> void;

  std::shared_ptr<ComponentContext> global_context_;

  std::unique_ptr<Executable> executable_;
  std::unique_ptr<std::thread> executing_thread_;

  std::atomic<bool> terminated_{false};
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_UTILS_EXECUTOR_HPP_
