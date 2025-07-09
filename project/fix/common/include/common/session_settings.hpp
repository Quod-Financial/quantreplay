#ifndef SIMULATOR_FIX_COMMON_SESSION_SETTINGS_HPP_
#define SIMULATOR_FIX_COMMON_SESSION_SETTINGS_HPP_

#include <quickfix/SessionSettings.h>

#include <filesystem>

namespace simulator::fix {

[[nodiscard]] auto read_settings_from_file(
    const std::filesystem::path& file_path) -> FIX::SessionSettings;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_SESSION_SETTINGS_HPP_