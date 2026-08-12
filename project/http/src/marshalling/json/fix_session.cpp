#include "ih/marshalling/json/fix_session.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>
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
    for (const auto& [key, value] : session.settings) {
      rapidjson::Value setting_key;
      setting_key.SetString(key.c_str(), allocator);

      rapidjson::Value setting_value;
      setting_value.SetString(value.c_str(), allocator);

      settings_value.AddMember(
          setting_key.Move(), setting_value.Move(), allocator);
    }
    session_value.AddMember("settings", settings_value.Move(), allocator);

    doc_session_settings.PushBack(session_value.Move(), allocator);
  }

  dest_doc.AddMember("sessionSettings", doc_session_settings, allocator);
}

auto marshal_fix_sessions(
    rapidjson::Document& dest_doc,
    const std::unordered_map<std::string, FixSessionController::FixSessionInfo>&
        sessions) -> void {
  auto& allocator = dest_doc.GetAllocator();

  rapidjson::Value sessions_array{rapidjson::kArrayType};
  for (const auto& [session_id, info] : sessions) {
    rapidjson::Value session_value{rapidjson::kObjectType};

    rapidjson::Value id_value;
    id_value.SetString(session_id.c_str(), allocator);
    session_value.AddMember("id", id_value.Move(), allocator);

    rapidjson::Value host_value;
    host_value.SetString(info.host_port.c_str(), allocator);
    session_value.AddMember("host", host_value.Move(), allocator);

    rapidjson::Value last_connected_value;
    if (info.last_connected_time.has_value()) {
      const auto time_str =
          fmt::format("{:%Y-%m-%d %T}", *info.last_connected_time);
      last_connected_value.SetString(time_str.c_str(), allocator);
    } else {
      last_connected_value.SetNull();
    }
    session_value.AddMember(
        "lastConnectedTime", last_connected_value.Move(), allocator);

    rapidjson::Value connected_value;
    connected_value.SetBool(info.connected);
    session_value.AddMember("connected", connected_value.Move(), allocator);

    sessions_array.PushBack(session_value.Move(), allocator);
  }

  dest_doc.AddMember("sessions", sessions_array.Move(), allocator);
}

}  // namespace simulator::http::json
