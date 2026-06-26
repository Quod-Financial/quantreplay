#include "api/models/fix_session.hpp"

#include "ih/common/exceptions.hpp"

namespace simulator::data_layer {

auto FixSession::create(FixSession::Patch snapshot) -> FixSession {
  if (!snapshot.venue_id_.has_value()) {
    throw RequiredAttributeMissing("FixSession", "VenueId");
  }
  if (!snapshot.session_id_.has_value()) {
    throw RequiredAttributeMissing("FixSession", "SessionId");
  }

  FixSession fix_session{};
  fix_session.venue_id_ = std::move(*snapshot.venue_id_);
  fix_session.session_id_ = std::move(*snapshot.session_id_);
  fix_session.last_connected_time_ = std::move(snapshot.last_connected_time_);
  return fix_session;
}

auto FixSession::venue_id() const noexcept -> const std::string& {
  return venue_id_;
}

auto FixSession::session_id() const noexcept -> const std::string& {
  return session_id_;
}

auto FixSession::last_connected_time() const noexcept
    -> const std::optional<core::sys_us>& {
  return last_connected_time_;
}

auto FixSession::Patch::venue_id() const noexcept
    -> const std::optional<std::string>& {
  return venue_id_;
}

auto FixSession::Patch::with_venue_id(std::string venue_id) noexcept -> Patch& {
  venue_id_ = std::move(venue_id);
  return *this;
}

auto FixSession::Patch::session_id() const noexcept
    -> const std::optional<std::string>& {
  return session_id_;
}

auto FixSession::Patch::with_session_id(std::string session_id) noexcept
    -> Patch& {
  session_id_ = std::move(session_id);
  return *this;
}

auto FixSession::Patch::last_connected_time() const noexcept
    -> const std::optional<core::sys_us>& {
  return last_connected_time_;
}

auto FixSession::Patch::with_last_connected_time(
    core::sys_us last_connected_time) noexcept -> Patch& {
  last_connected_time_ = last_connected_time;
  return *this;
}

}  // namespace simulator::data_layer
