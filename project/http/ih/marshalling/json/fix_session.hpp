#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_FIX_SESSION_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_FIX_SESSION_HPP_

#include <rapidjson/document.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "core/common/session_settings.hpp"
#include "ih/controllers/fix_session_controller.hpp"

namespace simulator::http::json {

auto marshal_session_settings(
    rapidjson::Document& dest_doc,
    const std::vector<core::FixSessionSettings>& session_settings) -> void;

auto marshal_fix_sessions(
    rapidjson::Document& dest_doc,
    const std::unordered_map<std::string, FixSessionController::FixSessionInfo>&
        sessions) -> void;

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_FIX_SESSION_HPP_
