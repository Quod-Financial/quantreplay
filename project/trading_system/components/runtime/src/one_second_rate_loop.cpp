#include "ih/one_second_rate_loop.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <utility>

#include "log/logging.hpp"

namespace simulator::trading_system::runtime {

auto OneSecondRateLoop::add(std::function<void()> task) -> void {
  log::trace("adding task to the loop");
  if (!thread_) [[likely]] {
    tasks_.emplace_back(std::move(task));
    log::debug("added repeating task to the loop");
  } else {
    throw std::logic_error(
        "OneSecondRateLoop::add: loop is already running, cannot add a task");
  }
}

auto OneSecondRateLoop::run() -> void {
  log::trace("starting the thread loop");

  if (thread_) [[unlikely]] {
    log::warn("cannot start the loop, it is already running");
    return;
  }

  if (tasks_.empty()) [[unlikely]] {
    log::warn("loop was not started, no tasks to run");
    return;
  }

  thread_ = std::make_unique<std::jthread>(
      [this](std::stop_token stop) { loop_main(std::move(stop)); });

  log::debug("loop started with {} repeating tasks", tasks_.size());
}

auto OneSecondRateLoop::terminate() -> void {
  log::trace("terminating the loop");

  if (thread_) {
    thread_.reset();
    log::debug("loop was terminated");
  } else {
    log::warn("cannot terminate the loop, it is not running");
  }
}

auto OneSecondRateLoop::loop_main(std::stop_token token) const -> void {
  log::debug("loop thread starting execution");

  while (!token.stop_requested()) {
    log::trace("executing tasks in the loop");
    for (const auto& task : tasks_) {
      task();
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  log::debug("loop thread stopped execution");
}

}  // namespace simulator::trading_system::runtime