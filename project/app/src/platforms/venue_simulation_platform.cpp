#include "ih/platforms/venue_simulation_platform.hpp"

#include <filesystem>
#include <memory>
#include <utility>
#include <vector>

#include "cfg/api/cfg.hpp"
#include "core/common/session_settings.hpp"
#include "fix/generator_initiator/lifetime.hpp"
#include "fix/session_settings.hpp"
#include "ih/components/fix_generator_initiator.hpp"
#include "ih/components/fix_trading_system_acceptor.hpp"
#include "ih/components/generator.hpp"
#include "ih/components/http_server.hpp"
#include "ih/components/trading_engine.hpp"
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
  fix_generator_initiator_ = create_fix_generator_initiator(fix_config_path);
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
  if (fix_generator_initiator_) {
    middleware::bind_market_data_request_channel(fix_generator_initiator_);
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
  if (fix_generator_initiator_) {
    fix_generator_initiator_->launch();
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
  if (fix_generator_initiator_) {
    fix_generator_initiator_->terminate();
  }
  http_server_->terminate();

  middleware::release_trading_admin_channel();
  middleware::release_trading_reply_channel();
  middleware::release_trading_request_channel();
  middleware::release_trading_session_connection_event_channel();
  middleware::release_trading_session_termination_event_channel();
  middleware::release_generator_admin_channel();
  if (fix_generator_initiator_) {
    middleware::release_market_data_request_channel();
    middleware::release_market_data_reply_channel();
    middleware::release_generator_initiator_connection_event_channel();
    middleware::release_generator_initiator_termination_event_channel();
  }

  trading_engine_.reset();
  fix_trading_system_acceptor_.reset();
  fix_generator_initiator_.reset();
  generator_.reset();
  http_server_.reset();
  log::info("venue simulation platform has been terminated");
}

}  // namespace simulator