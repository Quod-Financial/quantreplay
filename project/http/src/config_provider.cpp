#include "ih/config_provider.hpp"

#include <algorithm>
#include <filesystem>
#include <optional>
#include <string_view>
#include <unordered_set>
#include <utility>

namespace simulator::http {
namespace {

auto is_data_dictionary_key(std::string_view key) -> bool {
  return key == "DATADICTIONARY" || key == "TRANSPORTDATADICTIONARY" ||
         key.starts_with("APPDATADICTIONARY");
}

}  // namespace

ConfigProviderImpl::ConfigProviderImpl(
    RuntimeConfiguration runtime_configuration)
    : runtime_configuration_{std::move(runtime_configuration)} {}

auto ConfigProviderImpl::venue_id() const -> const std::string& {
  return runtime_configuration_.venue_id;
}

auto ConfigProviderImpl::venue_start_time() const -> const core::tz_us& {
  return runtime_configuration_.venue_start_time;
}

auto ConfigProviderImpl::version() const -> const std::string& {
  return runtime_configuration_.version;
}

auto ConfigProviderImpl::session_settings() const
    -> const std::vector<core::FixSessionSettings>& {
  return runtime_configuration_.session_settings;
}

auto collect_session_dictionaries(
    const std::vector<core::FixSessionSettings>& sessions,
    std::string_view session_id)
    -> std::optional<std::unordered_set<std::filesystem::path>> {
  const auto target = std::ranges::find_if(
      sessions, [&](const core::FixSessionSettings& session) {
        return session.id == session_id;
      });

  if (target == sessions.end()) {
    return std::nullopt;
  }

  const auto default_section = std::ranges::find_if(
      sessions, [](const core::FixSessionSettings& session) {
        return !session.id.has_value();
      });

  std::unordered_set<std::filesystem::path> paths;
  // To track and not insert different values from the same key in SESSION and
  // DEFAULT settings
  std::unordered_set<std::string_view> seen_keys;
  const auto collect = [&](const core::FixSessionSettings& section) {
    for (const auto& [key, value] : section.settings) {
      if (is_data_dictionary_key(key) && seen_keys.insert(key).second) {
        paths.insert(value);
      }
    }
  };

  collect(*target);
  if (default_section != sessions.end()) {
    collect(*default_section);
  }
  return paths;
}

}  // namespace simulator::http
