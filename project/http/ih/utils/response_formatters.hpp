#ifndef SIMULATOR_HTTP_IH_UTILS_RESPONSE_FORMATTERS_HPP_
#define SIMULATOR_HTTP_IH_UTILS_RESPONSE_FORMATTERS_HPP_

#include <fmt/format.h>
#include <pistache/http_defs.h>

#include <string>
#include <string_view>
#include <unordered_map>

#include "data_layer/api/models/venue.hpp"
#include "ih/config_provider.hpp"
#include "ih/controllers/fix_session_controller.hpp"

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

// Formats the status of another simulator instance when response_code != OK.
[[nodiscard]]
auto format_venue_status(const data_layer::Venue& venue,
                         const Pistache::Http::Code& response_code)
    -> std::string;

// Formats the status of the current simulator instance
[[nodiscard]]
auto format_current_venue_status(
    const data_layer::Venue& venue,
    const Pistache::Http::Code& response_code,
    const ConfigProvider& config_provider,
    const std::unordered_map<std::string, FixSessionController::FixSessionInfo>&
        fix_sessions) -> std::string;

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_UTILS_RESPONSE_FORMATTERS_HPP_
