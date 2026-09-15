#ifndef SIMULATOR_FIX_COMMON_SESSION_CONVERSION_HPP_
#define SIMULATOR_FIX_COMMON_SESSION_CONVERSION_HPP_

#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include <stdexcept>

#include "protocol/types/session.hpp"

namespace simulator::fix {

struct InvalidSessionCategoryError : std::invalid_argument {
  explicit InvalidSessionCategoryError(const std::string& message);
};

[[nodiscard]]
auto decode_session(const FIX::SessionID& session_id) -> protocol::Session;

[[nodiscard]]
auto encode_session(const protocol::fix::Session& session) -> FIX::SessionID;

[[nodiscard]]
auto encode_session(const protocol::Session& session) -> FIX::SessionID;

auto map_sender_sub_id(const FIX::Message& source,
                       protocol::Session& destination) -> void;

auto map_target_sub_id(const protocol::Session& source,
                       FIX::Message& destination) -> void;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_SESSION_CONVERSION_HPP_