#include "ih/utils/response_formatters.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <rapidjson/document.h>

#include <string>

#include "ih/marshalling/json/detail/utils.hpp"
#include "ih/marshalling/json/fix_session.hpp"

namespace simulator::http {
namespace {

auto marshal_venue_identity(rapidjson::Document& document,
                            const data_layer::Venue& venue) -> void {
  auto& allocator = document.GetAllocator();
  rapidjson::Value value;

  value.SetString(venue.venue_id().data(), allocator);
  document.AddMember("id", value, allocator);

  if (const auto& venue_name = venue.name()) {
    value.SetString(venue_name->data(), allocator);
    document.AddMember("name", value, allocator);
  }
}

auto marshal_status_code(rapidjson::Document& document,
                         const Pistache::Http::Code& response_code) -> void {
  rapidjson::Value value;
  value.SetInt(static_cast<int>(response_code));
  document.AddMember("statusCode", value, document.GetAllocator());
}

}  // namespace

auto format_venue_status(const data_layer::Venue& venue,
                         const Pistache::Http::Code& response_code)
    -> std::string {
  rapidjson::Document document;
  document.SetObject();

  marshal_venue_identity(document, venue);
  marshal_status_code(document, response_code);

  return json::encode(document);
}

auto format_current_venue_status(
    const data_layer::Venue& venue,
    const Pistache::Http::Code& response_code,
    const ConfigProvider& config_provider,
    const std::unordered_map<std::string, FixSessionController::FixSessionInfo>&
        fix_sessions) -> std::string {
  rapidjson::Document document;
  document.SetObject();
  auto& allocator = document.GetAllocator();

  marshal_venue_identity(document, venue);

  rapidjson::Value value;

  auto time_str =
      fmt::format("{:%Y-%m-%d %T}", config_provider.venue_start_time());
  value.SetString(time_str.data(), allocator);
  document.AddMember("startTime", value, allocator);

  const auto& version = config_provider.version();
  value.SetString(version.data(), allocator);
  document.AddMember("version", value, allocator);

  json::marshal_session_settings(document, config_provider.session_settings());

  json::marshal_fix_sessions(document, fix_sessions);

  marshal_status_code(document, response_code);

  return json::encode(document);
}

}  // namespace simulator::http
