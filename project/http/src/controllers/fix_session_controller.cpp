#include "ih/controllers/fix_session_controller.hpp"

#include <fmt/format.h>

#include <string_view>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "core/common/session_settings.hpp"
#include "core/tools/time.hpp"
#include "data_layer/api/models/fix_session.hpp"
#include "ih/config_provider.hpp"
#include "ih/data_bridge/fix_session_accessor.hpp"
#include "log/logging.hpp"
#include "protocol/app/session_connected_event.hpp"
#include "protocol/app/session_terminated_event.hpp"
#include "protocol/types/session.hpp"

namespace simulator::http {
namespace {

constexpr auto SocketAcceptPortKey = std::string_view{"SOCKETACCEPTPORT"};
constexpr auto DefaultHeading = std::string_view{"DEFAULT"};

[[nodiscard]]
auto make_session_id(const protocol::fix::Session& session) -> std::string {
  auto session_id = fmt::format("{}:{}->{}",
                                session.begin_string.value(),
                                session.sender_comp_id.value(),
                                session.target_comp_id.value());
  if (session.session_qualifier.has_value()) {
    session_id += fmt::format(":{}", session.session_qualifier->value());
  }
  return session_id;
}

[[nodiscard]]
auto resolve_session_id(const protocol::Session& session)
    -> std::optional<std::string> {
  if (const auto* fix = std::get_if<protocol::fix::Session>(&session.value)) {
    return make_session_id(*fix);
  }
  return std::nullopt;
}

[[nodiscard]]
auto find_accept_port(const core::FixSessionSettings& settings)
    -> std::optional<std::string> {
  for (const auto& [key, value] : settings.settings) {
    if (key == SocketAcceptPortKey) {
      return value;
    }
  }
  return std::nullopt;
}

[[nodiscard]]
auto find_default_accept_port(
    const std::vector<core::FixSessionSettings>& session_settings)
    -> std::optional<std::string> {
  for (const auto& settings : session_settings) {
    if (settings.heading == DefaultHeading) {
      return find_accept_port(settings);
    }
  }
  return std::nullopt;
}

}  // namespace

FixSessionControllerImpl::FixSessionControllerImpl(
    std::shared_ptr<data_bridge::FixSessionAccessor> data_accessor,
    std::shared_ptr<ConfigProvider> config_provider,
    std::string host_ip)
    : data_accessor_{std::move(data_accessor)},
      config_provider_{std::move(config_provider)},
      host_ip_{std::move(host_ip)} {
  const auto db_sessions = select_db_sessions();
  populate_sessions(db_sessions);
  remove_absent_db_sessions(db_sessions);
  insert_missing_sessions(db_sessions);
}

auto FixSessionControllerImpl::handle(
    const protocol::SessionConnectedEvent& event) -> void {
  const auto session_id = resolve_session_id(event.session);
  if (!session_id.has_value()) {
    return;
  }

  const auto connected_time = core::get_current_system_time();
  {
    const std::lock_guard lock{sessions_mutex_};
    const auto iter = sessions_.find(*session_id);
    if (iter == sessions_.end()) {
      return;
    }
    iter->second.last_connected_time = connected_time;
    iter->second.connected = true;
  }

  const auto result = data_accessor_->update(
      data_layer::FixSession::Patch{}.with_last_connected_time(connected_time),
      config_provider_->venue_id(),
      *session_id);
  if (!result.has_value()) {
    log::warn("failed to persist last connected time of fix session '{}'",
              *session_id);
  }
}

auto FixSessionControllerImpl::handle(
    const protocol::SessionTerminatedEvent& event) -> void {
  const auto session_id = resolve_session_id(event.session);
  if (!session_id.has_value()) {
    return;
  }

  const std::lock_guard lock{sessions_mutex_};
  if (const auto iter = sessions_.find(*session_id); iter != sessions_.end()) {
    iter->second.connected = false;
  }
}

auto FixSessionControllerImpl::sessions() const
    -> std::unordered_map<std::string, FixSessionInfo> {
  const std::lock_guard lock{sessions_mutex_};
  return sessions_;
}

auto FixSessionControllerImpl::select_db_sessions() const
    -> std::vector<data_layer::FixSession> {
  auto result = data_accessor_->select_all(config_provider_->venue_id());
  if (!result.has_value()) {
    log::warn("failed to load fix sessions of venue '{}' from the database",
              config_provider_->venue_id());
    return {};
  }
  return std::move(result).value();
}

auto FixSessionControllerImpl::populate_sessions(
    const std::vector<data_layer::FixSession>& db_sessions) -> void {
  std::unordered_map<std::string, std::optional<core::sys_us>>
      last_connected_times;
  last_connected_times.reserve(db_sessions.size());
  for (const auto& db_session : db_sessions) {
    last_connected_times.emplace(db_session.session_id(),
                                 db_session.last_connected_time());
  }

  const auto& session_settings = config_provider_->session_settings();
  const auto default_accept_port = find_default_accept_port(session_settings);

  const std::lock_guard lock{sessions_mutex_};
  for (const auto& settings : session_settings) {
    if (!settings.id.has_value()) {
      continue;
    }
    const auto& session_id = *settings.id;

    auto info = FixSessionInfo{};
    auto port = find_accept_port(settings);
    if (!port.has_value()) {
      port = default_accept_port;
    }
    if (port.has_value()) {
      info.host_port = fmt::format("{}:{}", host_ip_, *port);
    } else {
      info.host_port = host_ip_;
    }
    if (const auto iter = last_connected_times.find(session_id);
        iter != last_connected_times.end()) {
      info.last_connected_time = iter->second;
    }

    sessions_.insert_or_assign(session_id, std::move(info));
  }
}

auto FixSessionControllerImpl::remove_absent_db_sessions(
    const std::vector<data_layer::FixSession>& db_sessions) const -> void {
  std::vector<std::string> absent_session_ids;
  {
    const std::lock_guard lock{sessions_mutex_};
    for (const auto& db_session : db_sessions) {
      if (!sessions_.contains(db_session.session_id())) {
        absent_session_ids.push_back(db_session.session_id());
      }
    }
  }

  if (absent_session_ids.empty()) {
    return;
  }

  const auto result = data_accessor_->delete_all(config_provider_->venue_id(),
                                                 absent_session_ids);
  if (!result.has_value()) {
    log::warn("failed to delete unconfigured fix sessions of venue '{}'",
              config_provider_->venue_id());
  }
}

auto FixSessionControllerImpl::insert_missing_sessions(
    const std::vector<data_layer::FixSession>& db_sessions) const -> void {
  std::unordered_set<std::string> stored_session_ids;
  stored_session_ids.reserve(db_sessions.size());
  for (const auto& db_session : db_sessions) {
    stored_session_ids.insert(db_session.session_id());
  }

  std::vector<std::string> missing_session_ids;
  {
    const std::lock_guard lock{sessions_mutex_};
    for (const auto& [session_id, info] : sessions_) {
      if (!stored_session_ids.contains(session_id)) {
        missing_session_ids.push_back(session_id);
      }
    }
  }

  for (const auto& session_id : missing_session_ids) {
    const auto result =
        data_accessor_->add(data_layer::FixSession::Patch{}
                                .with_venue_id(config_provider_->venue_id())
                                .with_session_id(session_id));
    if (!result.has_value()) {
      log::warn("failed to insert fix session '{}' into the database",
                session_id);
    }
  }
}

}  // namespace simulator::http
