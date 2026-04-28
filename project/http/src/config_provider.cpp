#include "ih/config_provider.hpp"

#include <utility>

#include "cfg/api/cfg.hpp"
#include "core/version.hpp"

namespace simulator::http {

ConfigProviderImpl::ConfigProviderImpl()
    : runtime_configuration_{cfg::venue().name,
                             cfg::venue().start_time,
                             std::string{core::version()}} {}

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

}  // namespace simulator::http
