#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_GENERATOR_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_GENERATOR_HPP_

#include <string_view>

#include "protocol/admin/generator.hpp"

namespace simulator::http::json {

class GenerationStartUnmarshaller {
 public:
  static auto unmarshall(std::string_view json,
                         protocol::StartGenerationRequest& request) -> void;
};

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_GENERATOR_HPP_
