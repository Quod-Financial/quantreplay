#include "ih/marshalling/json/generator.hpp"

#include <rapidjson/document.h>

#include <optional>

#include "ih/marshalling/json/detail/unmarshaller.hpp"

namespace simulator::http::json {

auto GenerationStartUnmarshaller::unmarshall(
    std::string_view json, protocol::StartGenerationRequest& request)
    -> void {
  rapidjson::Document document;
  document.Parse(json.data());
  if (!document.IsObject()) {
    throw std::runtime_error{"failed to parse GenerationStart JSON object"};
  }

  Unmarshaller unmarshaller{document};
  if (!unmarshaller("seed", request.seed)) {
    request.seed = std::nullopt;
  }
}

}  // namespace simulator::http::json
