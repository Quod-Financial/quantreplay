#include "ih/simple_thread_pool.hpp"

#include <functional>
#include <mutex>
#include <utility>

#include "log/logging.hpp"

namespace simulator::trading_system::runtime {

SimpleThreadPool::SimpleThreadPool(std::size_t thread_count)
    : ThreadPool::Implementation(thread_count) {
  init();
}

SimpleThreadPool::~SimpleThreadPool() noexcept { await(); }

auto SimpleThreadPool::await() noexcept -> void {
  log::trace("awaiting threadpool to finish tasks");

  for (auto& thread : threads_) {
    thread.request_stop();
  }

  condition_.notify_all();

  log::debug("awaiting {} threads to finish tasks", threads_.size());
  for (auto& thread : threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  log::trace("all threadpools threads were joined");
}

auto SimpleThreadPool::enqueue(std::function<void()> task) -> void {
  log::trace("enqueueing a task to the threadpool");
  {
    std::lock_guard lock(mutex_);
    tasks_.emplace(std::move(task));
  }
  condition_.notify_one();
  log::trace("enqueued a task to the threadpool");
}

auto SimpleThreadPool::init() -> void {
  const auto threads_count = concurrency();
  threads_.reserve(threads_count);
  for (std::size_t trd = 0; trd < threads_count; ++trd) {
    threads_.emplace_back([this](const auto& stop) { run(stop); });
  }
  log::debug("threadpool with {} threads created", threads_count);
}

auto SimpleThreadPool::run(const std::stop_token& stop_token) -> void {
  log::trace("threadpool thread started execution");
  while (true) {
    std::function<void()> task;

    {
      std::unique_lock lock(mutex_);
      condition_.wait(
          lock, [&] { return !tasks_.empty() || stop_token.stop_requested(); });

      if (!tasks_.empty()) {
        std::swap(task, tasks_.front());
        tasks_.pop();
      } else if (stop_token.stop_requested()) {
        break;
      }
    }

    if (task) {
      log::trace("threadpool thread starting executing a task");
      task();
      log::trace("threadpool thread finished a task");
    }
  }
  log::trace("threadpool thread finished execution");
}

}  // namespace simulator::trading_system::runtime