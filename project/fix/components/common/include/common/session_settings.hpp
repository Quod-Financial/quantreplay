#ifndef SIMULATOR_FIX_COMMON_SESSION_SETTINGS_HPP_
#define SIMULATOR_FIX_COMMON_SESSION_SETTINGS_HPP_

#include <quickfix/SessionSettings.h>

#include <filesystem>
#include <vector>

#include "core/common/session_settings.hpp"

namespace simulator::fix {

[[nodiscard]] auto read_settings_from_file(
    const std::filesystem::path& file_path) -> FIX::SessionSettings;

auto to_fix_session_settings(const FIX::SessionSettings& session_settings)
    -> std::vector<core::FixSessionSettings>;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_SESSION_SETTINGS_HPP_
