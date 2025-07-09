#include "ih/application.hpp"

#include "cfg/api/cfg.hpp"
#include "core/version.hpp"
#include "data_layer/api/data_access_layer.hpp"
#include "ih/platforms/venue_simulation_platform.hpp"
#include "log/logging.hpp"

namespace simulator {

namespace database = data_layer::database;
namespace {

auto setup_database_connection() -> database::Context {
  return database::setup(cfg::db());
}

}  // namespace

auto Application::start() -> Application {
  log::info("starting Market Simulator, version: {}", core::version());
  Application application{};
  application.launch();
  return application;
}

Application::Application() {
  log::debug("creating simulator application");
  platform_ = create_venue_simulation_platform();
  log::info("simulator application created");
}

Application::~Application() noexcept { terminate(); }

auto Application::launch() -> void {
  log::debug("launching simulator application");
  platform_->launch();
  log::info("simulator application has been launched");
}

auto Application::terminate() noexcept -> void try {
  platform_->terminate();
  platform_.reset();
} catch (const std::exception& exception) {
  log::err("an error occurred while terminating simulator application: {}",
           exception.what());
} catch (...) {
  log::err("unknown error occurred while terminating simulator application");
}

auto Application::create_venue_simulation_platform()
    -> std::unique_ptr<Platform> {
  return std::make_unique<VenueSimulationPlatform>(setup_database_connection());
}

}  // namespace simulator