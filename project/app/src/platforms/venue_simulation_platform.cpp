#include "ih/platforms/venue_simulation_platform.hpp"

#include <filesystem>

#include "cfg/api/cfg.hpp"
#include "ih/components/fix_acceptor.hpp"
#include "ih/components/generator.hpp"
#include "ih/components/http_server.hpp"
#include "ih/components/trading_engine.hpp"
#include "ih/dispatchers/venue_trading_reply_dispatcher.hpp"
#include "log/logging.hpp"
#include "middleware/channels/generator_admin_channel.hpp"
#include "middleware/channels/trading_admin_channel.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "middleware/channels/trading_request_channel.hpp"
#include "middleware/channels/trading_session_event_channel.hpp"

namespace simulator {
namespace {

auto get_fix_configuration_path() -> std::filesystem::path {
  return cfg::quickfix().session_settings;
}

}  // namespace

VenueSimulationPlatform::VenueSimulationPlatform(
    const data_layer::database::Context& database) {
  log::debug("creating venue simulation platform");
  trading_engine_ = std::make_shared<TradingEngine>(database);
  fix_acceptor_ = std::make_shared<FixAcceptor>(get_fix_configuration_path());
  generator_ = std::make_shared<Generator>(database);
  http_server_ = std::make_shared<HttpServer>(database);

  middleware::bind_trading_admin_channel(trading_engine_);
  middleware::bind_trading_reply_channel(
      std::make_shared<VenueTradingReplyDispatcher>(generator_, fix_acceptor_));
  middleware::bind_trading_request_channel(trading_engine_);
  middleware::bind_trading_session_event_channel(trading_engine_);
  middleware::bind_generator_admin_channel(generator_);
  log::debug("venue simulation platform has been created");
}

auto VenueSimulationPlatform::launch() -> void {
  log::debug("launching venue simulation platform");
  trading_engine_->launch();
  fix_acceptor_->launch();
  generator_->launch();
  http_server_->launch();
  log::info("venue simulation platform has been launched");
}

auto VenueSimulationPlatform::terminate() -> void {
  log::debug("terminating venue simulation platform");
  trading_engine_->terminate();
  fix_acceptor_->terminate();
  generator_->terminate();
  http_server_->terminate();

  middleware::release_trading_admin_channel();
  middleware::release_trading_reply_channel();
  middleware::release_trading_request_channel();
  middleware::release_trading_session_event_channel();
  middleware::release_generator_admin_channel();

  trading_engine_.reset();
  fix_acceptor_.reset();
  generator_.reset();
  http_server_.reset();
  log::info("venue simulation platform has been terminated");
}

}  // namespace simulator