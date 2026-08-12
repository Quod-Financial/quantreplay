#ifndef SIMULATOR_FIX_ACCEPTOR_SESSION_SETTINGS_HPP_
#define SIMULATOR_FIX_ACCEPTOR_SESSION_SETTINGS_HPP_

#include <vector>

#include "acceptor.hpp"
#include "core/common/session_settings.hpp"

namespace simulator::fix {

auto session_settings(Acceptor& acceptor)
    -> std::vector<core::FixSessionSettings>;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_ACCEPTOR_SESSION_SETTINGS_HPP_