#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_HALT_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_HALT_HPP_

#include <string_view>

#include "protocol/admin/trading_phase.hpp"

namespace simulator::http::json {

class HaltUnmarshaller {
 public:
  static auto unmarshall(std::string_view json,
                         protocol::HaltPhaseRequest& request) -> void;
};

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_HALT_HPP_
