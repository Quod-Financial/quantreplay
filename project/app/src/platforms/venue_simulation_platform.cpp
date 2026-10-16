#include "ih/platforms/venue_simulation_platform.hpp"

#include <filesystem>
#include <memory>
#include <utility>
#include <vector>

#include "cfg/api/cfg.hpp"
#include "core/common/session_settings.hpp"
#include "data_layer/api/data_access_layer.hpp"
#include "fix/generator_initiator/lifetime.hpp"
#include "fix/session_settings.hpp"
#include "ih/components/fix_generator_initiator.hpp"
#include "ih/components/fix_trading_system_acceptor.hpp"
#include "ih/components/generator.hpp"
#include "ih/components/http_server.hpp"
#include "ih/components/trading_engine.hpp"
#include "ih/datasource_settings_files.hpp"
#include "ih/dispatchers/market_data_request_dispatcher.hpp"
#include "ih/dispatchers/venue_trading_reply_dispatcher.hpp"
#include "log/logging.hpp"
#include "middleware/channels/generator_admin_channel.hpp"
#include "middleware/channels/generator_initiator_event_channel.hpp"
#include "middleware/channels/market_data_reply_channel.hpp"
#include "middleware/channels/market_data_request_channel.hpp"
#include "middleware/channels/trading_admin_channel.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "middleware/channels/trading_request_channel.hpp"
#include "middleware/channels/trading_session_event_channel.hpp"

namespace simulator {
namespace {

auto get_fix_configuration_path() -> std::filesystem::path {
  return cfg::quickfix().session_settings;
}

auto convert_to_http_callbacks(const ControlCallbacks& callbacks)
    -> http::ControlCallbacks {
  http::ControlCallbacks http_callbacks;
  http_callbacks.reset_app_state = callbacks.reset_app_state;
  return http_callbacks;
}

[[nodiscard]]
auto create_fix_generator_initiator(
    const std::filesystem::path& fix_config_path)
    -> std::shared_ptr<FixGeneratorInitiator> {
  auto initiator = fix::create_generator_initiator(fix_config_path);
  if (!initiator.has_value()) {
    return nullptr;
  }
  return std::make_shared<FixGeneratorInitiator>(*std::move(initiator));
}

auto report_unusable_settings_file(const DatasourceSettingsFile& file)
    -> void {
  for (const auto& datasource : file.datasources) {
    log::err(
        "no FIX initiator was created from the settings file `{}', "
        "the `{}' datasource (id: {}) cannot reach its market data feed "
        "through it",
        file.path.string(),
        datasource.name,
        datasource.id);
  }
}

[[nodiscard]]
auto create_fix_generator_initiators(
    const std::filesystem::path& fix_config_path,
    const data_layer::database::Context& database)
    -> std::vector<std::shared_ptr<FixGeneratorInitiator>> {
  std::vector<std::shared_ptr<FixGeneratorInitiator>> initiators;
  if (auto initiator = create_fix_generator_initiator(fix_config_path)) {
    initiators.push_back(std::move(initiator));
  }
  const auto datasources =
      data_layer::select_enabled_venue_datasources(database, cfg::venue().name);
  for (const auto& file :
       datasource_settings_files(datasources, fix_config_path)) {
    if (auto initiator = create_fix_generator_initiator(file.path)) {
      initiators.push_back(std::move(initiator));
    } else {
      report_unusable_settings_file(file);
    }
  }
  return initiators;
}

[[nodiscard]]
auto market_data_request_routes(
    const std::vector<std::shared_ptr<FixGeneratorInitiator>>& initiators)
    -> std::vector<MarketDataRequestDispatcher::Route> {
  std::vector<MarketDataRequestDispatcher::Route> routes;
  for (const auto& initiator : initiators) {
    for (auto& session : initiator->sessions()) {
      routes.push_back({.session = std::move(session), .receiver = initiator});
    }
  }
  return routes;
}

}  // namespace

VenueSimulationPlatform::VenueSimulationPlatform(
    const data_layer::database::Context& database,
    const ControlCallbacks& callbacks) {
  log::debug("creating venue simulation platform");
  const auto fix_config_path = get_fix_configuration_path();
  const auto fix_sessions = fix::session_settings(fix_config_path);

  trading_engine_ = std::make_shared<TradingEngine>(database);
  fix_trading_system_acceptor_ =
      std::make_shared<FixTradingSystemAcceptor>(fix_config_path);
  // After the acceptor, so a file redeclaring its sessions is refused.
  fix_generator_initiators_ =
      create_fix_generator_initiators(fix_config_path, database);
  generator_ = std::make_shared<Generator>(database);
  http_server_ = std::make_shared<HttpServer>(
      database, convert_to_http_callbacks(callbacks), fix_sessions);

  middleware::bind_trading_admin_channel(trading_engine_);
  middleware::bind_trading_reply_channel(
      std::make_shared<VenueTradingReplyDispatcher>(
          generator_, fix_trading_system_acceptor_));
  middleware::bind_trading_request_channel(trading_engine_);
  middleware::bind_trading_session_connection_event_channel(http_server_);
  middleware::bind_trading_session_termination_event_channel(trading_engine_);
  middleware::bind_trading_session_termination_event_channel(http_server_);
  middleware::bind_generator_admin_channel(generator_);
  if (!fix_generator_initiators_.empty()) {
    middleware::bind_market_data_request_channel(
        std::make_shared<MarketDataRequestDispatcher>(
            market_data_request_routes(fix_generator_initiators_)));
    middleware::bind_market_data_reply_channel(generator_);
    middleware::bind_generator_initiator_connection_event_channel(generator_);
    middleware::bind_generator_initiator_termination_event_channel(generator_);
  }
  log::debug("venue simulation platform has been created");
}

auto VenueSimulationPlatform::launch() -> void {
  log::debug("launching venue simulation platform");
  trading_engine_->launch();
  fix_trading_system_acceptor_->launch();
  for (const auto& initiator : fix_generator_initiators_) {
    initiator->launch();
  }
  generator_->launch();
  http_server_->launch();
  log::info("venue simulation platform has been launched");
}

auto VenueSimulationPlatform::terminate() -> void {
  log::debug("terminating venue simulation platform");
  // The generator unsubscribe from the FIX and local market data
  // so it must terminate firstly.
  generator_->terminate();
  // To store the orders that will be canceled on FIX disconnect
  // (cancel-on-disconnect empties the order books), it must terminate before
  // the FIX acceptor.
  trading_engine_->terminate();
  fix_trading_system_acceptor_->terminate();
  for (const auto& initiator : fix_generator_initiators_) {
    initiator->terminate();
  }
  http_server_->terminate();

  middleware::release_trading_admin_channel();
  middleware::release_trading_reply_channel();
  middleware::release_trading_request_channel();
  middleware::release_trading_session_connection_event_channel();
  middleware::release_trading_session_termination_event_channel();
  middleware::release_generator_admin_channel();
  if (!fix_generator_initiators_.empty()) {
    middleware::release_market_data_request_channel();
    middleware::release_market_data_reply_channel();
    middleware::release_generator_initiator_connection_event_channel();
    middleware::release_generator_initiator_termination_event_channel();
  }

  trading_engine_.reset();
  fix_trading_system_acceptor_.reset();
  fix_generator_initiators_.clear();
  generator_.reset();
  http_server_.reset();
  log::info("venue simulation platform has been terminated");
}

}  // namespace simulator