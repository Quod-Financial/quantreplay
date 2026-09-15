#ifndef SIMULATOR_FIX_TRADING_SYSTEM_ACCEPTOR_LIFETIME_HPP_
#define SIMULATOR_FIX_TRADING_SYSTEM_ACCEPTOR_LIFETIME_HPP_

#include <filesystem>

#include "fix/trading_system_acceptor/trading_system_acceptor.hpp"

namespace simulator::fix {

[[nodiscard]] auto create_trading_system_acceptor(
    const std::filesystem::path& config_path) -> TradingSystemAcceptor;

auto start_trading_system_acceptor(TradingSystemAcceptor& acceptor) -> void;

auto stop_trading_system_acceptor(TradingSystemAcceptor& acceptor) noexcept
    -> void;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_TRADING_SYSTEM_ACCEPTOR_LIFETIME_HPP_