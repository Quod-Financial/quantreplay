#ifndef SIMULATOR_HTTP_HTTP_HPP_
#define SIMULATOR_HTTP_HTTP_HPP_

#include <functional>
#include <memory>
#include <vector>

#include "core/common/session_settings.hpp"
#include "data_layer/api/database/context.hpp"
#include "protocol/app/session_connected_event.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::http {

struct ControlCallbacks {
  std::function<void()> reset_app_state;
};

struct Server {
  struct Implementation;

  explicit Server(std::unique_ptr<Implementation> impl) noexcept;
  Server(const Server&) = delete;
  Server(Server&&) noexcept;
  ~Server() noexcept;

  auto operator=(const Server&) -> Server& = delete;
  auto operator=(Server&&) noexcept -> Server&;

  auto implementation() noexcept -> Implementation&;

 private:
  std::unique_ptr<Implementation> impl_;
};

auto create_http_server(
    data_layer::database::Context db,
    ControlCallbacks callbacks,
    const std::vector<core::FixSessionSettings>& session_settings) -> Server;

auto launch_http_server(Server& server) -> void;

auto terminate_http_server(Server& server) noexcept -> void;

auto react_on(const protocol::SessionConnectedEvent& event, Server& server)
    -> void;

auto react_on(const protocol::SessionTerminatedEvent& event, Server& server)
    -> void;

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_HTTP_HPP_
