#ifndef SIMULATOR_APP_IH_APPLICATION_HPP_
#define SIMULATOR_APP_IH_APPLICATION_HPP_

#include <memory>

#include "ih/platforms/platform.hpp"

namespace simulator {

class Application {
 public:
  Application(const Application&) = delete;

  Application(Application&&) noexcept = default;
  ~Application() noexcept;

  auto operator=(const Application&) -> Application& = delete;
  auto operator=(Application&&) noexcept -> Application& = default;

  static auto start() -> Application;

 private:
  Application();

  auto launch() -> void;

  auto terminate() noexcept -> void;

  static auto create_venue_simulation_platform() -> std::unique_ptr<Platform>;

  std::unique_ptr<Platform> platform_;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_APPLICATION_HPP_