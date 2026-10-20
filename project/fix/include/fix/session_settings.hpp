#ifndef SIMULATOR_FIX_SESSION_SETTINGS_HPP_
#define SIMULATOR_FIX_SESSION_SETTINGS_HPP_

#include <filesystem>
#include <vector>

#include "core/common/session_settings.hpp"

namespace simulator::fix {

[[nodiscard]]
auto session_settings(const std::filesystem::path& config_path)
    -> std::vector<core::FixSessionSettings>;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_SESSION_SETTINGS_HPP_
