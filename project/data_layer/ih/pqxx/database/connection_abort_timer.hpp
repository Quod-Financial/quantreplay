#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_CONNECTION_ABORT_TIMER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_CONNECTION_ABORT_TIMER_HPP_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace simulator::data_layer::internal_pqxx {

// Backstop for a DB op wedged on a live-but-silent peer, which the
// connection-string TCP timeouts cannot detect.
inline constexpr std::chrono::milliseconds DefaultOperationTimeout{
    std::chrono::seconds{30}};

// Runs an abort action once from a background thread unless disarmed first.
class ConnectionAbortTimer {
 public:
  using AbortAction = std::function<void()>;

  ConnectionAbortTimer(std::chrono::milliseconds timeout,
                       AbortAction on_timeout);

  ConnectionAbortTimer(const ConnectionAbortTimer&) = delete;
  ConnectionAbortTimer(ConnectionAbortTimer&&) = delete;
  auto operator=(const ConnectionAbortTimer&) -> ConnectionAbortTimer& = delete;
  auto operator=(ConnectionAbortTimer&&) -> ConnectionAbortTimer& = delete;

  // Disarms; waits for any in-flight action to finish.
  ~ConnectionAbortTimer();

  [[nodiscard]]
  auto fired() const noexcept -> bool;

 private:
  auto watch() -> void;

  std::chrono::milliseconds timeout_;
  AbortAction on_timeout_;
  std::mutex mutex_;
  std::condition_variable state_changed_;
  bool disarmed_{false};
  std::atomic<bool> fired_{false};
  std::jthread worker_;
};

// Shuts the socket down to unblock a libpq call wedged on it.
auto make_socket_abort_action(int socket_fd)
    -> ConnectionAbortTimer::AbortAction;

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_CONNECTION_ABORT_TIMER_HPP_
