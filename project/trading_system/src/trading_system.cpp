#include "ih/trading_system.hpp"

#include <cstdlib>

#include "data_layer/api/data_access_layer.hpp"
#include "ih/config/config.hpp"
#include "ih/tools/loaders.hpp"
#include "instruments/cache.hpp"
#include "log/logging.hpp"
#include "trading_system/trading_system.hpp"

namespace simulator::trading_system {

namespace database = data_layer::database;

namespace {

[[nodiscard]]
auto read_system_configuration(const database::Context& database) -> Config {
  log::trace("reading system configuration");

  Config config;
  create_database_config_loader(database)->load_config(config);

  log::info("trading system configuration has been pulled from the database");

  return config;
}

[[nodiscard]]
auto create_instruments_cache(const database::Context& database)
    -> instrument::Cache {
  log::trace("creating trading instruments cache");

  auto cache = instrument::Cache::create();
  create_database_instruments_loader(database)->load_instruments(cache);

  log::info("trading instruments cache has been created");

  return cache;
}

[[nodiscard]]
auto create_trading_system_implementation(const database::Context& database)
    -> std::unique_ptr<System::Implementation> {
  try {
    return std::make_unique<System::Implementation>(
        read_system_configuration(database),
        create_instruments_cache(database));
  } catch (const std::exception& exception) {
    log::err(
        "failed to create a trading system implementation, an error occurred: "
        "{}",
        exception.what());
  } catch (...) {
    log::err(
        "failed to create a trading system implementation, unknown error "
        "occurred");
  }
  throw std::runtime_error("failed to create a trading engine implementation");
}

auto launch_trading_system(System::Implementation& system) -> void {
  try {
    system.launch();
    return;
  } catch (const std::exception& exception) {
    log::err("failed to launch trading system, an error occurred: {}",
             exception.what());

  } catch (...) {
    log::err("failed to launch trading system, unknown error occurred");
  }
  throw std::runtime_error("failed to launch trading engine");
}

auto terminate_trading_system(System::Implementation& system) noexcept
    -> void try {
  system.terminate();
} catch (const std::exception& exception) {
  log::err("failed to terminate trading system, an error occurred: {}",
           exception.what());
} catch (...) {
  log::err("failed to terminate trading system, unknown error occurred");
}

}  // namespace

System::System(std::unique_ptr<Implementation> impl) noexcept
    : impl_(std::move(impl)) {}

System::System(System&&) noexcept = default;

System::~System() noexcept = default;

auto System::operator=(System&&) noexcept -> System& = default;

auto System::implementation() const noexcept -> Implementation& {
  if (impl_) [[likely]] {
    return *impl_;
  }

  log::err(
      "trading system implementation has not been "
      "allocated/initialized, this may indicate a critical bug "
      "in the component, can not continue execution, aborting...");

  std::abort();
}

auto create_trading_system(const database::Context& database) -> System {
  log::debug("creating trading system instance");
  System engine{create_trading_system_implementation(database)};
  log::info("created trading system instance");

  return engine;
}

auto launch_trading_system(System& trading_system) -> void {
  log::debug("launching trading system");
  launch_trading_system(trading_system.implementation());
  log::info("trading system has been launched");
}

auto terminate_trading_system(System& trading_system) noexcept -> void {
  log::debug("terminating trading system");
  terminate_trading_system(trading_system.implementation());
  log::info("trading system has been terminated");
}

auto process(protocol::OrderPlacementRequest request, System& trading_system)
    -> void {
  log::debug("called the procedure to process OrderPlacementRequest");
  trading_system.implementation().execute(std::move(request));
}

auto process(protocol::OrderModificationRequest request, System& trading_system)
    -> void {
  log::debug("called the procedure to process OrderModificationRequest");
  trading_system.implementation().execute(std::move(request));
}

auto process(protocol::OrderCancellationRequest request, System& trading_system)
    -> void {
  log::debug("called the procedure to process OrderCancellationRequest");
  trading_system.implementation().execute(std::move(request));
}

auto process(protocol::MarketDataRequest request, System& trading_system)
    -> void {
  log::debug("called the procedure to process MarketDataRequest");
  trading_system.implementation().execute(std::move(request));
}

auto process(protocol::SecurityStatusRequest request, System& trading_system)
    -> void {
  log::debug("called the procedure to process SecurityStatusRequest");
  trading_system.implementation().execute(std::move(request));
}

auto process(const protocol::InstrumentStateRequest& request,
             protocol::InstrumentState& reply,
             System& trading_system) -> void {
  log::debug("called the procedure to process InstrumentStateRequest");
  trading_system.implementation().execute(request, reply);
}

auto process(const protocol::HaltPhaseRequest& request,
             protocol::HaltPhaseReply& reply,
             System& trading_system) -> void {
  log::debug("called the procedure to process HaltPhaseRequest");
  trading_system.implementation().execute(request, reply);
}

auto process(const protocol::ResumePhaseRequest& request,
             protocol::ResumePhaseReply& reply,
             System& trading_system) -> void {
  log::debug("called the procedure to process ResumePhaseRequest");
  trading_system.implementation().execute(request, reply);
}

auto process(const protocol::StoreMarketStateRequest& request,
             protocol::StoreMarketStateReply& reply,
             System& trading_system) -> void {
  log::debug("called the procedure to process StoreMarketStateRequest");
  trading_system.implementation().execute(request, reply);
}

auto process(const protocol::RecoverMarketStateRequest& request,
             protocol::RecoverMarketStateReply& reply,
             System& trading_system) -> void {
  log::debug("called the procedure to process RecoverMarketStateRequest");
  trading_system.implementation().execute(request, reply);
}

auto react_on(const protocol::SessionTerminatedEvent& event,
              System& trading_system) -> void {
  log::debug("called procedure to react on SessionTerminatedEvent");
  trading_system.implementation().react_on(event);
}

}  // namespace simulator::trading_system