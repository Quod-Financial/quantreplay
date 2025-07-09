#ifndef SIMULATOR_RUNTIME_IH_THREAD_POOL_IMPL_HPP_
#define SIMULATOR_RUNTIME_IH_THREAD_POOL_IMPL_HPP_

#include <stdexcept>
#include <thread>

#include "runtime/thread_pool.hpp"

namespace simulator::trading_system::runtime {

class ThreadPool::Implementation {
 public:
  explicit Implementation(std::size_t threads_count)
      : threads_count_(normalize_threads_count(threads_count)) {}

  Implementation() = delete;
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) noexcept = delete;

  auto operator=(const Implementation&) -> Implementation& = delete;
  auto operator=(Implementation&&) noexcept -> Implementation& = delete;

  virtual ~Implementation() = default;

  auto concurrency() const noexcept -> std::size_t { return threads_count_; }

  virtual auto await() noexcept -> void = 0;

  virtual auto enqueue(std::function<void()> task) -> void = 0;

 private:
  static auto normalize_threads_count(std::size_t count) -> std::size_t {
    if (count == 0) {
      if (count = std::thread::hardware_concurrency(); count == 0) {
        throw std::runtime_error(
            "ThreadPool::Implementation: cannot determine the number of "
            "hardware threads");
      }
    }
    return count;
  }

  std::size_t threads_count_;
};

}  // namespace simulator::trading_system::runtime

#endif  // SIMULATOR_RUNTIME_IH_THREAD_POOL_IMPL_HPP_
