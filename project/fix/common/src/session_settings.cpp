#include "common/session_settings.hpp"

#include <quickfix/SessionSettings.h>

#include <filesystem>
#include <stdexcept>

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

}  // namespace simulator::fix