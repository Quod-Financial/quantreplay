#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_SERVICE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_SERVICE_HPP_

#include <functional>
#include <memory>
#include <utility>

namespace simulator::trading_system::runtime {

class Service {
 public:
  Service() = default;
  Service(const Service&) = default;
  Service(Service&&) noexcept = default;
  virtual ~Service() noexcept = default;

  auto operator=(const Service&) -> Service& = default;
  auto operator=(Service&&) noexcept -> Service& = default;

  virtual auto execute(std::function<void()> task) -> void = 0;
};

template <typename F, typename... Args>
auto execute(Service& srv, F&& func, Args&&... args) {
  srv.execute(std::bind(std::forward<F>(func), std::forward<Args>(args)...));
}

template <typename F, typename... Args>
auto execute(Service* srv, F&& func, Args&&... args) {
  execute(*srv, std::forward<F>(func), std::forward<Args>(args)...);
}

template <typename F, typename... Args>
auto execute(const std::shared_ptr<Service>& srv, F&& func, Args&&... args) {
  execute(*srv, std::forward<F>(func), std::forward<Args>(args)...);
}

template <typename F, typename... Args>
auto execute(const std::unique_ptr<Service>& srv, F&& func, Args&&... args) {
  execute(*srv, std::forward<F>(func), std::forward<Args>(args)...);
}

}  // namespace simulator::trading_system::runtime

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_SERVICE_HPP_
