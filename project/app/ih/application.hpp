#ifndef SIMULATOR_APP_IH_APPLICATION_HPP_
#define SIMULATOR_APP_IH_APPLICATION_HPP_

#include <memory>

#include "ih/control_callbacks.hpp"
#include "ih/platforms/platform.hpp"

namespace simulator {

class Application {
 public:
  Application(const Application&) = delete;

  Application(Application&&) noexcept = default;
  ~Application() noexcept;

  auto operator=(const Application&) -> Application& = delete;
  auto operator=(Application&&) noexcept -> Application& = default;

  static auto start(ControlCallbacks callbacks) -> Application;

  auto reset_state() -> void;

 private:
  explicit Application(ControlCallbacks callbacks);

  auto launch() -> void;

  auto terminate() noexcept -> void;

  auto create_venue_simulation_platform() -> std::unique_ptr<Platform>;

  std::unique_ptr<Platform> platform_;
  ControlCallbacks callbacks_;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_APPLICATION_HPP_