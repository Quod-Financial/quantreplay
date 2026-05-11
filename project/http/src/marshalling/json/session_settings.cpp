#include "ih/marshalling/json/session_settings.hpp"

#include <rapidjson/rapidjson.h>

namespace simulator::http::json {

auto marshal_session_settings(
    rapidjson::Document& dest_doc,
    const std::vector<core::FixSessionSettings>& session_settings) -> void {
  auto& allocator = dest_doc.GetAllocator();
  rapidjson::Document doc_session_settings{std::addressof(allocator)};
  doc_session_settings.SetArray();

  for (const auto& session : session_settings) {
    rapidjson::Value session_value{rapidjson::kObjectType};

    rapidjson::Value heading_value;
    heading_value.SetString(session.heading.c_str(), allocator);
    session_value.AddMember("section", heading_value.Move(), allocator);

    if (session.id.has_value()) {
      rapidjson::Value section_id_value;
      section_id_value.SetString(session.id.value().c_str(), allocator);
      session_value.AddMember("id", section_id_value.Move(), allocator);
    }

    rapidjson::Value settings_value{rapidjson::kObjectType};
    for (const auto& setting : session.settings) {
      rapidjson::Value setting_key;
      setting_key.SetString(setting.key.c_str(), allocator);

      rapidjson::Value setting_value;
      setting_value.SetString(setting.value.c_str(), allocator);

      settings_value.AddMember(
          setting_key.Move(), setting_value.Move(), allocator);
    }
    session_value.AddMember("settings", settings_value.Move(), allocator);

    doc_session_settings.PushBack(session_value.Move(), allocator);
  }

  dest_doc.AddMember("sessionSettings", doc_session_settings, allocator);
}

}  // namespace simulator::http::json
