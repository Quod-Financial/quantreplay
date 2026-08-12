#include "common/session_settings.hpp"

#include <quickfix/SessionSettings.h>

#include <filesystem>
#include <stdexcept>

#include "core/common/session_settings.hpp"
#include "log/logging.hpp"

namespace simulator::fix {

auto read_settings_from_file(const std::filesystem::path& file_path)
    -> FIX::SessionSettings {
  log::debug("reading quickfix settings from `{}'", file_path.c_str());
  try {
    return FIX::SessionSettings{file_path};
  } catch (const FIX::ConfigError& config_error) {
    log::err(
        "failed to load quickfix configuration from `{}', "
        "quickfix engine reported error: {}",
        file_path.c_str(),
        config_error.detail);
  } catch (const std::exception& exception) {
    log::err(
        "failed to load quickfix configuration from `{}',"
        "an error occurred: {}",
        file_path.c_str(),
        exception.what());
  } catch (...) {
    log::err(
        "failed to load quickfix configuration from `{}',"
        "unknown error occurred",
        file_path.c_str());
  }
  throw std::runtime_error("failed to load quickfix configuration from a file");
}

auto to_fix_session_settings(const FIX::SessionSettings& session_settings)
    -> std::vector<core::FixSessionSettings> {
  std::vector<core::FixSessionSettings> result;

  const auto& fix_default_session = session_settings.get();
  core::FixSessionSettings default_session;
  default_session.heading = fix_default_session.getName();
  for (auto it = fix_default_session.begin(); it != fix_default_session.end();
       ++it) {
    default_session.settings.emplace(it->first, it->second);
  }
  result.emplace_back(std::move(default_session));

  auto fix_sessions_ids = session_settings.getSessions();

  for (const auto& fix_session_id : fix_sessions_ids) {
    auto& fix_session = session_settings.get(fix_session_id);

    core::FixSessionSettings session;
    session.heading = fix_session.getName();
    session.id =
        std::make_optional<std::string>(fix_session_id.toStringFrozen());

    for (auto it = fix_session.begin(); it != fix_session.end(); ++it) {
      if (!fix_default_session.has(it->first) ||
          fix_default_session.getString(it->first) != it->second) {
        session.settings.emplace(it->first, it->second);
      }
    }
    result.emplace_back(std::move(session));
  }

  return result;
}

}  // namespace simulator::fix