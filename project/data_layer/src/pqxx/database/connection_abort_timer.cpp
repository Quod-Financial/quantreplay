#include "ih/pqxx/database/connection_abort_timer.hpp"

#include <sys/socket.h>

#include <utility>

#include "log/logging.hpp"

namespace simulator::data_layer::internal_pqxx {

ConnectionAbortTimer::ConnectionAbortTimer(std::chrono::milliseconds timeout,
                                           AbortAction on_timeout)
    : timeout_{timeout},
      on_timeout_{std::move(on_timeout)},
      worker_{[this] { watch(); }} {}

ConnectionAbortTimer::~ConnectionAbortTimer() {
  {
    const std::lock_guard lock{mutex_};
    disarmed_ = true;
  }
  state_changed_.notify_all();
  // Wake watch() before worker_'s implicit join, so it exits without firing.
}

auto ConnectionAbortTimer::fired() const noexcept -> bool {
  return fired_.load(std::memory_order_relaxed);
}

auto ConnectionAbortTimer::watch() -> void {
  std::unique_lock lock{mutex_};
  const bool disarmed =
      state_changed_.wait_for(lock, timeout_, [this] { return disarmed_; });
  if (disarmed) {
    return;
  }

  fired_.store(true, std::memory_order_relaxed);
  lock.unlock();
  if (on_timeout_) {
    on_timeout_();
  }
}

auto make_socket_abort_action(int socket_fd)
    -> ConnectionAbortTimer::AbortAction {
  return [socket_fd] {
    if (socket_fd < 0) {
      return;
    }
    log::warn(
        "database operation exceeded its deadline; shutting down the "
        "connection socket to abort the wedged libpq call");
    ::shutdown(socket_fd, SHUT_RDWR);
  };
}

}  // namespace simulator::data_layer::internal_pqxx
