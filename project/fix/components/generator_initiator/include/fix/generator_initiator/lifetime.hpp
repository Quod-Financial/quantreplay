#ifndef SIMULATOR_FIX_GENERATOR_INITIATOR_LIFETIME_HPP_
#define SIMULATOR_FIX_GENERATOR_INITIATOR_LIFETIME_HPP_

#include <filesystem>
#include <optional>

#include "fix/generator_initiator/generator_initiator.hpp"

namespace simulator::fix {

[[nodiscard]] auto create_generator_initiator(
    const std::filesystem::path& config_path)
    -> std::optional<GeneratorInitiator>;

auto start_generator_initiator(GeneratorInitiator& initiator) -> void;

auto stop_generator_initiator(GeneratorInitiator& initiator) noexcept -> void;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_GENERATOR_INITIATOR_LIFETIME_HPP_
