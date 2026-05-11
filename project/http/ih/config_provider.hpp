#ifndef SIMULATOR_HTTP_IH_CONFIG_PROVIDER_HPP_
#define SIMULATOR_HTTP_IH_CONFIG_PROVIDER_HPP_

#include <string>

#include "core/common/session_settings.hpp"
#include "core/tools/time.hpp"

namespace simulator::http {

struct RuntimeConfiguration {
  std::string venue_id;
  core::tz_us venue_start_time;
  std::string version;
  std::vector<core::FixSessionSettings> session_settings;
};

class ConfigProvider {
 public:
  virtual ~ConfigProvider() = default;

  [[nodiscard]]
  virtual auto venue_id() const -> const std::string& = 0;

  [[nodiscard]]
  virtual auto venue_start_time() const -> const core::tz_us& = 0;

  [[nodiscard]]
  virtual auto version() const -> const std::string& = 0;

  [[nodiscard]]
  virtual auto session_settings() const
      -> const std::vector<core::FixSessionSettings>& = 0;
};

class ConfigProviderImpl : public ConfigProvider {
 public:
  explicit ConfigProviderImpl(RuntimeConfiguration runtime_configuration);

  [[nodiscard]]
  auto venue_id() const -> const std::string& override;

  [[nodiscard]]
  auto venue_start_time() const -> const core::tz_us& override;

  [[nodiscard]]
  auto version() const -> const std::string& override;

  [[nodiscard]]
  auto session_settings() const
      -> const std::vector<core::FixSessionSettings>& override;

 private:
  RuntimeConfiguration runtime_configuration_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONFIG_PROVIDER_HPP_
