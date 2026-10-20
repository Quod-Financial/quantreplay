#ifndef SIMULATOR_FIX_COMMON_MAPPING_SETTING_MAPPING_SETTINGS_HPP_
#define SIMULATOR_FIX_COMMON_MAPPING_SETTING_MAPPING_SETTINGS_HPP_

#include <quickfix/Session.h>
#include <quickfix/SessionID.h>

#include <vector>

#include "common/mapping/setting/timestamp_precision.hpp"

namespace simulator::fix {

class MappingSettings {
 public:
  explicit MappingSettings(const std::vector<FIX::Session*>& sessions);

  struct Setting {
    TimestampPrecision timestamp_precision{TimestampPrecision::Milliseconds};
  };

  auto get_setting(const FIX::SessionID& session_id) const -> Setting;

 private:
  std::map<FIX::SessionID, Setting> settings_;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_MAPPING_SETTING_MAPPING_SETTINGS_HPP_
