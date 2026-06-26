#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_FIX_SESSION_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_FIX_SESSION_CONTROLLER_HPP_

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/tools/time.hpp"

namespace simulator::data_layer {
class FixSession;
}  // namespace simulator::data_layer

namespace simulator::protocol {
struct SessionConnectedEvent;
struct SessionTerminatedEvent;
}  // namespace simulator::protocol

namespace simulator::http {

class ConfigProvider;

namespace data_bridge {
class FixSessionAccessor;
}  // namespace data_bridge

class FixSessionController {
 public:
  struct FixSessionInfo {
    std::string host_port;
    std::optional<core::sys_us> last_connected_time;
    bool connected = false;
  };

  FixSessionController() = default;
  FixSessionController(const FixSessionController&) = default;
  FixSessionController(FixSessionController&&) noexcept = default;
  virtual ~FixSessionController() = default;

  auto operator=(const FixSessionController&)
      -> FixSessionController& = default;
  auto operator=(FixSessionController&&) noexcept
      -> FixSessionController& = default;

  virtual auto handle(const protocol::SessionConnectedEvent& event) -> void = 0;

  virtual auto handle(const protocol::SessionTerminatedEvent& event)
      -> void = 0;

  [[nodiscard]]
  virtual auto sessions() const
      -> std::unordered_map<std::string, FixSessionInfo> = 0;
};

class FixSessionControllerImpl final : public FixSessionController {
 public:
  FixSessionControllerImpl(
      std::shared_ptr<data_bridge::FixSessionAccessor> data_accessor,
      std::shared_ptr<ConfigProvider> config_provider,
      std::string host_ip);

  auto handle(const protocol::SessionConnectedEvent& event) -> void override;

  auto handle(const protocol::SessionTerminatedEvent& event) -> void override;

  [[nodiscard]]
  auto sessions() const
      -> std::unordered_map<std::string, FixSessionInfo> override;

 private:
  [[nodiscard]]
  auto select_db_sessions() const -> std::vector<data_layer::FixSession>;

  auto populate_sessions(const std::vector<data_layer::FixSession>& db_sessions)
      -> void;

  auto remove_absent_db_sessions(
      const std::vector<data_layer::FixSession>& db_sessions) const -> void;

  auto insert_missing_sessions(
      const std::vector<data_layer::FixSession>& db_sessions) const -> void;

  std::shared_ptr<data_bridge::FixSessionAccessor> data_accessor_;
  std::shared_ptr<ConfigProvider> config_provider_;
  std::string host_ip_;
  std::unordered_map<std::string, FixSessionInfo> sessions_;
  mutable std::mutex sessions_mutex_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_FIX_SESSION_CONTROLLER_HPP_
