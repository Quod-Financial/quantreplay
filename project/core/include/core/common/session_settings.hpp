#ifndef SIMULATOR_CORE_COMMON_SESSION_SETTINGS_HPP_
#define SIMULATOR_CORE_COMMON_SESSION_SETTINGS_HPP_

#include <optional>
#include <string>
#include <vector>

namespace simulator::core {

struct FixSetting {
  std::string key;
  std::string value;
};

struct FixSessionSettings {
  std::string heading;
  std::optional<std::string> id;
  std::vector<FixSetting> settings;
};

}  // namespace simulator::core

#endif  // SIMULATOR_CORE_COMMON_SESSION_SETTINGS_HPP_
