#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_UTILS_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_UTILS_HPP_

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <string>

namespace simulator::http::json {

inline auto encode(const rapidjson::Document& document) -> std::string {
  rapidjson::StringBuffer buffer{};
  rapidjson::Writer writer{buffer};

  document.Accept(writer);

  return {buffer.GetString(), buffer.GetSize()};
}

inline auto make_key(std::string_view key) {
  return rapidjson::StringRef(key.data(), key.size());
}

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_UTILS_HPP_
