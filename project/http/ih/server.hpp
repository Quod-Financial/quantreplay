#ifndef SIMULATOR_HTTP_IH_SERVER_HPP_
#define SIMULATOR_HTTP_IH_SERVER_HPP_

#include <pistache/endpoint.h>

#include "data_layer/api/database/context.hpp"
#include "http/http.hpp"

namespace simulator::http {

struct Server::Implementation {
 public:
  Implementation(std::uint16_t accept_port,
                 data_layer::database::Context database,
                 ControlCallbacks callbacks);

  auto launch() -> void;

  auto terminate() -> void;

 private:
  static auto create_endpoint(std::uint16_t accept_port)
      -> std::unique_ptr<Pistache::Http::Endpoint>;

  auto setup_handler(data_layer::database::Context database,
                     std::uint16_t current_rest_port,
                     ControlCallbacks callbacks) -> void;

  std::unique_ptr<Pistache::Http::Endpoint> endpoint_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_SERVER_HPP_
