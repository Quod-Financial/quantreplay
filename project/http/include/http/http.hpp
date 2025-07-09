#ifndef SIMULATOR_HTTP_HTTP_HPP_
#define SIMULATOR_HTTP_HTTP_HPP_

#include <memory>

#include "data_layer/api/database/context.hpp"

namespace simulator::http {

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

auto create_http_server(data_layer::database::Context db) -> Server;

auto launch_http_server(Server& server) -> void;

auto terminate_http_server(Server& server) noexcept -> void;

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_HTTP_HPP_
