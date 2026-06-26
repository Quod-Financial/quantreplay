#ifndef SIMULATOR_APP_IH_COMPONENTS_HTTP_SERVER_HPP_
#define SIMULATOR_APP_IH_COMPONENTS_HTTP_SERVER_HPP_

#include "core/common/session_settings.hpp"
#include "data_layer/api/database/context.hpp"
#include "http/http.hpp"
#include "middleware/channels/trading_session_event_channel.hpp"
#include "protocol/app/session_connected_event.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator {

class HttpServer final
    : public middleware::TradingSessionConnectionEventListener,
      public middleware::TradingSessionTerminationEventListener {
 public:
  explicit HttpServer(
      data_layer::database::Context database,
      http::ControlCallbacks callbacks,
      const std::vector<core::FixSessionSettings>& session_settings)
      : http_server_(http::create_http_server(
            std::move(database), std::move(callbacks), session_settings)) {}

  auto launch() -> void { http::launch_http_server(http_server_); }

  auto terminate() -> void { http::terminate_http_server(http_server_); }

  auto on_event(const protocol::SessionConnectedEvent& event) -> void override {
    http::react_on(event, http_server_);
  }

  auto on_event(const protocol::SessionTerminatedEvent& event)
      -> void override {
    http::react_on(event, http_server_);
  }

 private:
  http::Server http_server_;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_COMPONENTS_HTTP_SERVER_HPP_