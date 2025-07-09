#include "common/mapping/setting/mapping_settings.hpp"

#include "log/logging.hpp"

namespace simulator::fix {

MappingSettings::MappingSettings(const std::vector<FIX::Session*>& sessions) {
  for (const auto& session : sessions) {
    if (session != nullptr) {
      auto& setting = settings_[session->getSessionID()];
      setting.timestamp_precision =
          TimestampPrecision{session->getTimestampPrecision()};
    }
  }
}

auto MappingSettings::get_setting(const FIX::SessionID& session_id) const
    -> Setting {
  if (const auto iter = settings_.find(session_id); iter != settings_.end()) {
    return iter->second;
  }
  return {};
}

}  // namespace simulator::fix