#include "fix/session_settings.hpp"

#include <filesystem>
#include <vector>

#include "common/session_settings.hpp"
#include "core/common/session_settings.hpp"

namespace simulator::fix {

auto session_settings(const std::filesystem::path& config_path)
    -> std::vector<core::FixSessionSettings> {
  return to_fix_session_settings(read_settings_from_file(config_path));
}

}  // namespace simulator::fix
