#include "ih/loop.hpp"

#include <condition_variable>
#include <mutex>

#include "log/logging.hpp"

namespace simulator {

static std::mutex loop_mutex;            // NOLINT - global application mutex
static std::condition_variable loop_cv;  // NOLINT - global main stop condition

auto Loop::suspend_main_thread() -> void {
  log::info("blocking main thread");
  std::unique_lock app_main_lock{loop_mutex};
  loop_cv.wait(app_main_lock);
}

auto Loop::release_main_thread() -> void {
  loop_cv.notify_one();
  log::info("main thread was requested to be released");
}

}  // namespace simulator