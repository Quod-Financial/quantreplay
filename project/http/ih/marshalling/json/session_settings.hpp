#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_SESSION_SETTINGS_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_SESSION_SETTINGS_HPP_

#include <rapidjson/document.h>

#include "core/common/session_settings.hpp"

namespace simulator::http::json {

auto marshal_session_settings(
    rapidjson::Document& dest_doc,
    const std::vector<core::FixSessionSettings>& session_settings) -> void;

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_SESSION_SETTINGS_HPP_
