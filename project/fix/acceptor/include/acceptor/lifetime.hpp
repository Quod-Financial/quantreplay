#ifndef SIMULATOR_FIX_ACCEPTOR_LIFETIME_HPP_
#define SIMULATOR_FIX_ACCEPTOR_LIFETIME_HPP_

#include <filesystem>

#include "acceptor/acceptor.hpp"

namespace simulator::fix {

[[nodiscard]] auto create_fix_acceptor(const std::filesystem::path& config_path)
    -> Acceptor;

auto start_fix_acceptor(Acceptor& acceptor) -> void;

auto stop_fix_acceptor(Acceptor& acceptor) noexcept -> void;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_ACCEPTOR_LIFETIME_HPP_