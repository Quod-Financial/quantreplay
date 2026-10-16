#ifndef SIMULATOR_FIX_GENERATOR_INITIATOR_LIFETIME_HPP_
#define SIMULATOR_FIX_GENERATOR_INITIATOR_LIFETIME_HPP_

#include <filesystem>
#include <optional>
#include <vector>

#include "fix/generator_initiator/generator_initiator.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix {

// Logs the cause instead of throwing when no initiator can be created.
[[nodiscard]] auto create_generator_initiator(
    const std::filesystem::path& config_path)
    -> std::optional<GeneratorInitiator>;

[[nodiscard]]
auto generator_initiator_sessions(const GeneratorInitiator& initiator)
    -> std::vector<protocol::fix::Session>;

auto start_generator_initiator(GeneratorInitiator& initiator) -> void;

auto stop_generator_initiator(GeneratorInitiator& initiator) noexcept -> void;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_GENERATOR_INITIATOR_LIFETIME_HPP_
