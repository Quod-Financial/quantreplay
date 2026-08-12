#ifndef SIMULATOR_HTTP_IH_SERVER_HPP_
#define SIMULATOR_HTTP_IH_SERVER_HPP_

#include <pistache/endpoint.h>

#include <memory>

#include "data_layer/api/database/context.hpp"
#include "http/http.hpp"
#include "protocol/app/session_connected_event.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::http {

class FixSessionController;

struct Server::Implementation {
 public:
  Implementation(std::uint16_t accept_port,
                 data_layer::database::Context database,
                 ControlCallbacks callbacks,
                 const std::vector<core::FixSessionSettings>& session_settings);

  auto launch() -> void;

  auto terminate() -> void;

  auto react_on(const protocol::SessionConnectedEvent& event) -> void;

  auto react_on(const protocol::SessionTerminatedEvent& event) -> void;

 private:
  static auto create_endpoint(std::uint16_t accept_port)
      -> std::unique_ptr<Pistache::Http::Endpoint>;

  auto setup_handler(
      data_layer::database::Context database,
      std::uint16_t current_rest_port,
      ControlCallbacks callbacks,
      const std::vector<core::FixSessionSettings>& session_settings) -> void;

  std::unique_ptr<Pistache::Http::Endpoint> endpoint_;
  std::shared_ptr<FixSessionController> fix_session_controller_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_SERVER_HPP_
