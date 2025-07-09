#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_THREAD_POOL_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_THREAD_POOL_HPP_

#include <functional>
#include <memory>

#include "runtime/service.hpp"

namespace simulator::trading_system::runtime {

class ThreadPool : public Service {
 public:
  class Implementation;

  [[nodiscard]]
  static auto create_simple_thread_pool(std::size_t threads = 0) -> ThreadPool;

  explicit ThreadPool(std::unique_ptr<Implementation> impl);

  ThreadPool() = delete;
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) noexcept;
  ~ThreadPool() noexcept override;

  auto operator=(const ThreadPool&) -> ThreadPool& = delete;
  auto operator=(ThreadPool&&) noexcept -> ThreadPool&;

  auto await() noexcept -> void;

  auto execute(std::function<void()> task) -> void override;

 private:
  std::unique_ptr<Implementation> impl_;
};

}  // namespace simulator::trading_system::runtime

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_THREAD_POOL_HPP_
