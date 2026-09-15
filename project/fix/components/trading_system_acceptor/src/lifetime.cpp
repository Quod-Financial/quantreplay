#include "fix/trading_system_acceptor/lifetime.hpp"

#include <memory>

#include "common/session_settings.hpp"
#include "ih/implementation.hpp"
#include "log/logging.hpp"

namespace simulator::fix {
namespace {

[[nodiscard]]
auto create_acceptor_implementation(const FIX::SessionSettings& settings)
    -> std::unique_ptr<TradingSystemAcceptor::Implementation> {
  return std::make_unique<TradingSystemAcceptor::Implementation>(settings);
}

}  // namespace

auto create_trading_system_acceptor(const std::filesystem::path& config_path)
    -> TradingSystemAcceptor {
  log::debug("creating a FIX acceptor instance");

  const auto configuration = read_settings_from_file(config_path);
  TradingSystemAcceptor acceptor{create_acceptor_implementation(configuration)};

  log::info("created a FIX acceptor instance");

  return acceptor;
}

auto start_trading_system_acceptor(TradingSystemAcceptor& acceptor) -> void {
  log::debug("starting FIX acceptor");

  acceptor.implementation().start_server();

  log::info("started FIX acceptor");
}

auto stop_trading_system_acceptor(TradingSystemAcceptor& acceptor) noexcept
    -> void {
  log::debug("stopping FIX acceptor");

  acceptor.implementation().stop_server();

  log::info("stopped FIX acceptor");
}

}  // namespace simulator::fix
