#include "ih/utils/executor.hpp"

#include <cassert>
#include <exception>
#include <memory>
#include <thread>
#include <utility>

#include "ih/context/component_context.hpp"
#include "ih/utils/executable.hpp"
#include "log/logging.hpp"

namespace simulator::generator {

Executor::Executor(std::unique_ptr<Executable> executable,
                   std::shared_ptr<ComponentContext> global_context) noexcept
    : global_context_{std::move(global_context)},
      executable_{std::move(executable)} {
  assert(global_context_);
  assert(executable_);
}

auto Executor::create(std::unique_ptr<Executable> executable,
                      std::shared_ptr<ComponentContext> global_context)
    -> std::unique_ptr<Executor> {
  return std::make_unique<Executor>(std::move(executable),
                                    std::move(global_context));
}

auto Executor::launch() -> void {
  // Consider adding some kind of id or name to identify executors in logs

  if (terminated_) {
    log::warn(
        "unable to launch a generation executor as it was terminated "
        "previously");
    return;
  }

  if (!global_context_->is_component_running()) {
    global_context_->call_on_launch([this] { launch(); });
    log::info("postponed launching of generation executor");
    return;
  }

  if (is_executing()) {
    log::warn(
        "unable to launch a generation executor as it is in executing state "
        "already");
    return;
  }

  start();
  log::info("generation executor was launched successfully");
}

auto Executor::terminate() noexcept -> void {
  terminated_ = true;
  if (is_executing()) {
    reset_executing_thread();
  }

  log::info("generation executor was terminated successfully");
}

auto Executor::is_executing() const noexcept -> bool {
  return executing_thread_ != nullptr;
}

auto Executor::start() -> void {
  reset_executing_thread();
  executing_thread_ = std::make_unique<std::thread>([this] { execute(); });
}

auto Executor::reset_executing_thread() noexcept -> void {
  if (is_executing()) {
    log::debug("joining generation executor's thread");

    assert(executing_thread_->joinable());
    try {
      executing_thread_->join();
    } catch (const std::exception& ex) {
      log::err("failed to join generation executor's thread: {}", ex.what());
    }
  }

  executing_thread_.reset();
}

auto Executor::execute() noexcept -> void {
  executable_->prepare();

  try {
    while (!terminated_ && global_context_->is_component_running()) {
      executable_->execute();
      if (executable_->finished()) {
        terminated_ = true;
      } else {
        std::this_thread::sleep_for(executable_->next_exec_timeout());
      }
    }
  } catch (const std::exception& ex) {
    log::err("error occurred in the generation executor's thread: {}",
             ex.what());

    // It may be useful to be able to restart a failed thread
    // Think about how to properly restart a failed thread
    // (we can't do it in the thread where an exception occurred as in
    // this case a thread will try to join itself -> ooops, deadlock =( )
    // Currently it is restarted by ComponentContext observer,
    // which requires a user direct action...
  }

  if (terminated_) {
    log::debug("generation executor's thread has been terminated");
    return;
  }

  try {
    global_context_->call_on_launch([this] { start(); });
    log::debug(
        "generation executor's thread has been suspended till "
        "next launch notification from the generation context");
  } catch (const std::exception& ex) {
    log::err(
        "an error occurred while postponing generator executor's "
        "thread launch on next generator launch event: {}",
        ex.what());
  }
}

}  // namespace simulator::generator
