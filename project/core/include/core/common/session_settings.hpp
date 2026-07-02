#ifndef SIMULATOR_CORE_COMMON_SESSION_SETTINGS_HPP_
#define SIMULATOR_CORE_COMMON_SESSION_SETTINGS_HPP_

#include <map>
#include <optional>
#include <string>

namespace simulator::core {

struct FixSessionSettings {
  std::string heading;
  std::optional<std::string> id;
  std::map<std::string, std::string> settings;
};

}  // namespace simulator::core

#endif  // SIMULATOR_CORE_COMMON_SESSION_SETTINGS_HPP_
