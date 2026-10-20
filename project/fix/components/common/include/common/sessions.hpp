#ifndef SIMULATOR_FIX_COMMON_SESSIONS_HPP_
#define SIMULATOR_FIX_COMMON_SESSIONS_HPP_

#include <quickfix/Session.h>

#include <vector>

namespace simulator::fix {

template <typename Connector>
[[nodiscard]]
auto get_sessions(const Connector& connector) -> std::vector<FIX::Session*> {
  std::vector<FIX::Session*> sessions;
  for (const auto& session_id : connector.getSessions()) {
    sessions.push_back(connector.getSession(session_id));
  }
  return sessions;
}

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_SESSIONS_HPP_
