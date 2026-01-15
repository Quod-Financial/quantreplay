#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_SETTING_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_SETTING_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include <string>
#include <utility>

#include "ih/data_bridge/setting_accessor.hpp"

namespace simulator::http {

class SettingController {
 public:
  using Result = std::pair<Pistache::Http::Code, std::string>;

  explicit SettingController(
      std::shared_ptr<data_bridge::SettingAccessor> data_accessor) noexcept;

  [[nodiscard]]
  auto select_all_settings() const -> Result;

  [[nodiscard]]
  auto update_settings(const std::string& body) const -> Result;

 private:
  static auto format_error_response(data_bridge::Failure failure)
      -> std::string;

  static auto is_updatable_setting(std::string_view setting_key) -> bool;

  static auto is_readonly_setting(std::string_view setting_key) -> bool;

  std::shared_ptr<data_bridge::SettingAccessor> settings_accessor_;

  static const std::string DisplayNameSetting;
  static const std::string ExternalPriceSeedConnectionSetting;
  static const std::string PriceSeedsLastUpdateTimeSetting;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_SETTING_CONTROLLER_HPP_
