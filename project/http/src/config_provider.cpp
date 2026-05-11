#include "ih/config_provider.hpp"

#include <utility>

namespace simulator::http {

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

}  // namespace simulator::http
