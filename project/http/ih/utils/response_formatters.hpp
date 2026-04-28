#ifndef SIMULATOR_HTTP_IH_UTILS_RESPONSE_FORMATTERS_HPP_
#define SIMULATOR_HTTP_IH_UTILS_RESPONSE_FORMATTERS_HPP_

#include <fmt/format.h>
#include <rapidjson/document.h>

#include <string>
#include <string_view>

#include "data_layer/api/models/venue.hpp"
#include "ih/config_provider.hpp"
#include "ih/marshalling/json/detail/utils.hpp"

namespace simulator::http {

[[nodiscard]]
inline auto format_response(std::string_view key, std::string_view msg)
    -> std::string {
  return fmt::format(R"({{"{}":"{}"}})", key, msg);
}

[[nodiscard]]
inline auto format_result_response(std::string_view result_message)
    -> std::string {
  return format_response("result", result_message);
}

[[nodiscard]]
inline auto format_venue_status(const data_layer::Venue& venue,
                                const Pistache::Http::Code& response_code,
                                const ConfigProvider* config_provider = nullptr)
    -> std::string {
  rapidjson::Document document;
  rapidjson::Value value;

  auto& allocator = document.GetAllocator();
  document.SetObject();

  value.SetString(venue.venue_id().data(), allocator);
  document.AddMember("id", value, allocator);

  if (const auto& venue_name = venue.name()) {
    value.SetString(venue_name->data(), allocator);
    document.AddMember("name", value, allocator);
  }

  if (config_provider != nullptr) {
    auto time_str =
        fmt::format("{:%Y-%b-%d %T}", config_provider->venue_start_time());
    value.SetString(time_str.data(), allocator);
    document.AddMember("startTime", value, allocator);

    const auto& version = config_provider->version();
    value.SetString(version.data(), allocator);
    document.AddMember("version", value, allocator);
  }

  value.SetInt(static_cast<int>(response_code));
  document.AddMember("statusCode", value, allocator);

  return json::encode(document);
}

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_UTILS_RESPONSE_FORMATTERS_HPP_
