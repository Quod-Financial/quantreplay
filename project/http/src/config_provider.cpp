#include "ih/config_provider.hpp"

#include <algorithm>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

namespace simulator::http {
namespace {

constexpr auto ConnectionTypeKey = std::string_view{"CONNECTIONTYPE"};
constexpr auto InitiatorConnectionType = std::string_view{"initiator"};
constexpr auto DefaultHeading = std::string_view{"DEFAULT"};

auto is_data_dictionary_key(std::string_view key) -> bool {
  return key == "DATADICTIONARY" || key == "TRANSPORTDATADICTIONARY" ||
         key.starts_with("APPDATADICTIONARY");
}

auto find_setting(const core::FixSessionSettings& session, std::string_view key)
    -> std::optional<std::string> {
  if (const auto it = session.settings.find(std::string{key});
      it != session.settings.end()) {
    return it->second;
  }
  return std::nullopt;
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

auto find_default_session_section(
    const std::vector<core::FixSessionSettings>& sessions)
    -> const core::FixSessionSettings* {
  const auto section = std::ranges::find_if(
      sessions, [](const core::FixSessionSettings& session) {
        return session.heading == DefaultHeading;
      });
  return section != sessions.end() ? std::addressof(*section) : nullptr;
}

auto resolve_session_setting(const core::FixSessionSettings& session,
                             const core::FixSessionSettings* default_section,
                             std::string_view key)
    -> std::optional<std::string> {
  if (auto value = find_setting(session, key); value.has_value()) {
    return value;
  }
  if (default_section != nullptr) {
    return find_setting(*default_section, key);
  }
  return std::nullopt;
}

auto is_initiator_session(const core::FixSessionSettings& session,
                          const core::FixSessionSettings* default_section)
    -> bool {
  return resolve_session_setting(session, default_section, ConnectionTypeKey) ==
         InitiatorConnectionType;
}

auto has_acceptor_session(const std::vector<core::FixSessionSettings>& sessions,
                          std::string_view session_id) -> bool {
  const auto* default_section = find_default_session_section(sessions);
  const auto session = std::ranges::find_if(
      sessions, [&](const core::FixSessionSettings& candidate) {
        return candidate.id == session_id;
      });
  return session != sessions.end() &&
         !is_initiator_session(*session, default_section);
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

  const auto* default_section = find_default_session_section(sessions);
  if (is_initiator_session(*target, default_section)) {
    return std::nullopt;
  }

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
  if (default_section != nullptr) {
    collect(*default_section);
  }
  return paths;
}

}  // namespace simulator::http
