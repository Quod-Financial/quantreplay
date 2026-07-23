#include "ih/application.hpp"

#include <algorithm>
#include <chrono>
#include <random>
#include <thread>
#include <type_traits>

#include "cfg/api/cfg.hpp"
#include "core/version.hpp"
#include "data_layer/api/data_access_layer.hpp"
#include "data_layer/api/exceptions/exceptions.hpp"
#include "ih/loop.hpp"
#include "ih/platforms/venue_simulation_platform.hpp"
#include "log/logging.hpp"

namespace simulator {

namespace database = data_layer::database;
namespace {

auto setup_database_connection() -> database::Context {
  return database::setup(cfg::db());
}

[[nodiscard]]
auto next_retry_backoff(int attempt) -> std::chrono::milliseconds {
  constexpr auto BaseBackoff = std::chrono::milliseconds{2000};
  constexpr auto MaxBackoff = std::chrono::milliseconds{8000};
  const auto ceiling =
      std::min(BaseBackoff * (1 << std::min(attempt - 1, 4)), MaxBackoff);
  static std::mt19937 generator{std::random_device{}()};
  std::uniform_real_distribution<double> jitter{0.5, 1.0};
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      ceiling * jitter(generator));
}

template <typename Build>
[[nodiscard]]
auto build_with_db_retry(Build build) -> std::invoke_result_t<Build> {
  constexpr int MaxAttempts = 5;
  for (int attempt = 1;; ++attempt) {
    try {
      return build();
    } catch (const data_layer::ConnectionFailure& failure) {
      if (attempt >= MaxAttempts) {
        log::err("database unavailable after {} attempts, giving up: {}",
                 attempt,
                 failure.what());
        throw;
      }
      if (Loop::termination_requested()) {
        log::warn("termination requested, abandoning database retry: {}",
                  failure.what());
        throw;
      }
      const auto backoff = next_retry_backoff(attempt);
      log::warn("database unavailable (attempt {}/{}): {}, retrying in {}ms",
                attempt,
                MaxAttempts,
                failure.what(),
                backoff.count());
      std::this_thread::sleep_for(backoff);
    }
  }
}

}  // namespace

auto Application::start(ControlCallbacks callbacks) -> Application {
  log::info("starting Market Simulator, version: {}", core::version());
  Application application{std::move(callbacks)};
  application.launch();
  return application;
}

auto Application::reset_state() -> void {
  log::info("resetting the simulator application state");
  try {
    platform_->terminate();
    platform_.reset();
  } catch (const std::exception& exception) {
    log::err("an error occurred while terminating simulator application: {}",
             exception.what());
    throw;
  } catch (...) {
    log::err("unknown error occurred while terminating simulator application");
    throw;
  }

  platform_ = build_with_db_retry(
      [this] { return create_venue_simulation_platform(); });
  platform_->launch();
  log::info("simulator application state has been reset");
}

Application::Application(ControlCallbacks callbacks)
    : callbacks_{std::move(callbacks)} {
  log::debug("creating simulator application");
  platform_ = build_with_db_retry(
      [this] { return create_venue_simulation_platform(); });
  log::info("simulator application created");
}

Application::~Application() noexcept { terminate(); }

auto Application::launch() -> void {
  log::debug("launching simulator application");
  platform_->launch();
  log::info("simulator application has been launched");
}

auto Application::terminate() noexcept -> void try {
  if (platform_) {
    platform_->terminate();
  }
  platform_.reset();
} catch (const std::exception& exception) {
  log::err("an error occurred while terminating simulator application: {}",
           exception.what());
} catch (...) {
  log::err("unknown error occurred while terminating simulator application");
}

auto Application::create_venue_simulation_platform()
    -> std::unique_ptr<Platform> {
  return std::make_unique<VenueSimulationPlatform>(setup_database_connection(),
                                                   callbacks_);
}

}  // namespace simulator