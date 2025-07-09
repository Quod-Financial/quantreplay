#include "http/http.hpp"

#include <pistache/http_headers.h>

#include <cstdlib>
#include <memory>
#include <stdexcept>

#include "cfg/api/cfg.hpp"
#include "data_layer/api/data_access_layer.hpp"
#include "ih/headers/x_api_version.hpp"
#include "ih/router.hpp"
#include "ih/server.hpp"
#include "log/logging.hpp"

namespace database = simulator::data_layer::database;

namespace Pistache::Http::Header {
namespace {

using simulator::http::XApiVersion;

RegisterHeader(XApiVersion);

}  // namespace
}  // namespace Pistache::Http::Header

namespace simulator::http {
namespace {

[[nodiscard]] auto retrieve_configured_http_port(const data_layer::Venue& venue)
    -> std::uint16_t {
  log::debug("selecting http port from `{}' venue record", venue.venue_id());

  const std::optional<std::uint16_t> assigned_port = venue.rest_port();
  if (assigned_port.has_value()) {
    log::debug("http server port is {}", *assigned_port);
    return *assigned_port;
  }

  throw std::runtime_error(
      "http (rest) port is not specified in currently simulated venue "
      "database record");
}

[[nodiscard]] auto create_server_implementation(database::Context db)
    -> std::unique_ptr<Server::Implementation> {
  try {
    const std::uint16_t server_port =
        retrieve_configured_http_port(data_layer::select_simulated_venue(db));
    return std::make_unique<Server::Implementation>(server_port, std::move(db));
  } catch (const std::exception& exception) {
    log::err("failed to create http server, an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to create http server, unknown error occurred");
  }
  throw std::runtime_error("failed to create http server");
}

auto launch_server(Server::Implementation& server) -> void {
  try {
    server.launch();
    return;
  } catch (const std::exception& exception) {
    log::err("failed to launch http server, an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to launch http server, unknown error occurred");
  }
  throw std::runtime_error("failed to launch http server");
}

auto terminate_server(Server::Implementation& server) noexcept -> void {
  try {
    server.terminate();
  } catch (const std::exception& exception) {
    log::err("failed to terminate http server, an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to terminate http server, unknown error occurred");
  }
}

}  // namespace

Server::Server(std::unique_ptr<Implementation> impl) noexcept
    : impl_(std::move(impl)) {}

Server::Server(simulator::http::Server&&) noexcept = default;

Server::~Server() noexcept = default;

auto Server::operator=(simulator::http::Server&&) noexcept -> Server& = default;

auto Server::implementation() noexcept -> Implementation& {
  if (impl_) [[likely]] {
    return *impl_;
  }

  log::err(
      "http server is not allocated/initialized, this may indicate "
      "a critical bug in the component, can not continue program execution, "
      "aborting...");

  std::abort();
}

Server::Implementation::Implementation(std::uint16_t accept_port,
                                       database::Context database)
    : endpoint_(create_endpoint(accept_port)) {
  setup_handler(std::move(database));
}

auto Server::Implementation::launch() -> void { endpoint_->serveThreaded(); }

auto Server::Implementation::terminate() -> void { endpoint_->shutdown(); }

auto Server::Implementation::create_endpoint(std::uint16_t accept_port)
    -> std::unique_ptr<Pistache::Http::Endpoint> {
  log::debug("creating http server endpoint");

  const Pistache::Address address{Pistache::Ipv4::any(), accept_port};
  const auto options = Pistache::Http::Endpoint::options().threads(1);

  auto endpoint = std::make_unique<Pistache::Http::Endpoint>(address);
  log::info("created http endpoint configured to listen on port {}",
            accept_port);

  return endpoint;
}

auto Server::Implementation::setup_handler(database::Context database) -> void {
  endpoint_->setHandler(std::make_shared<Router>(std::move(database)));
}

auto create_http_server(database::Context database) -> Server {
  log::debug("creating http server");
  Server server{create_server_implementation(std::move(database))};
  log::info("http server has been created");
  return server;
}

auto launch_http_server(Server& server) -> void {
  log::debug("launching http server");
  launch_server(server.implementation());
  log::info("http server has been launched");
}

auto terminate_http_server(Server& server) noexcept -> void {
  log::debug("terminating http server");
  terminate_server(server.implementation());
  log::info("http server has been terminated");
}

}  // namespace simulator::http