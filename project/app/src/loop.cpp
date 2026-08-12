#include "ih/loop.hpp"

#include <condition_variable>
#include <mutex>

#include "log/logging.hpp"

namespace simulator {

static std::mutex loop_mutex;            // NOLINT - global application mutex
static std::condition_variable loop_cv;  // NOLINT - global main stop condition
static Loop::State state{};

auto Loop::suspend_main_thread() -> State {
  log::info("blocking main thread");
  std::unique_lock app_main_lock{loop_mutex};
  loop_cv.wait(app_main_lock, [] { return state != State{}; });

  return std::exchange(state, State{});
}

auto Loop::termination_requested() -> bool {
  const std::lock_guard lock{loop_mutex};
  return state == State::Terminate;
}

auto Loop::terminate() -> void {
  {
    std::lock_guard lock{loop_mutex};
    state = State::Terminate;
    loop_cv.notify_one();
  }

  log::info(
      "main thread was requested to be released to terminate application");
}

auto Loop::reset_app_state() -> void {
  {
    std::lock_guard lock{loop_mutex};
    state = State::Reset;
    loop_cv.notify_one();
  }

  log::info(
      "main thread was requested to be released to reset application state");
}

}  // namespace simulator