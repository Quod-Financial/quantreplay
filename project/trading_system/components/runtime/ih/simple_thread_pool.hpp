#ifndef SIMULATOR_RUNTIME_IH_SIMPLE_THREAD_POOL_HPP_
#define SIMULATOR_RUNTIME_IH_SIMPLE_THREAD_POOL_HPP_

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "ih/thread_pool_impl.hpp"

namespace simulator::trading_system::runtime {

class SimpleThreadPool : public ThreadPool::Implementation {
 public:
  explicit SimpleThreadPool(std::size_t thread_count);

  SimpleThreadPool() = delete;
  SimpleThreadPool(const SimpleThreadPool&) = delete;
  SimpleThreadPool(SimpleThreadPool&&) noexcept = delete;

  ~SimpleThreadPool() noexcept override;

  auto operator=(const SimpleThreadPool&) -> SimpleThreadPool& = delete;
  auto operator=(SimpleThreadPool&&) noexcept -> SimpleThreadPool& = delete;

  auto await() noexcept -> void override;

  auto enqueue(std::function<void()> task) -> void override;

 private:
  auto init() -> void;

  auto run(const std::stop_token& stop_token) -> void;

  std::queue<std::function<void()>> tasks_;
  std::condition_variable condition_;
  std::mutex mutex_;
  std::vector<std::jthread> threads_;
};

}  // namespace simulator::trading_system::runtime

#endif  // SIMULATOR_RUNTIME_IH_SIMPLE_THREAD_POOL_HPP_
