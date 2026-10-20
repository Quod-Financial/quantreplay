#include "common/communicators/fix_message_sender.hpp"

#include <quickfix/Exceptions.h>
#include <quickfix/Session.h>

#include <exception>

#include "log/logging.hpp"

namespace simulator::fix {

auto FixMessageSender::send_message(
    FIX::Message& fix_message, const FIX::SessionID& fix_session) const noexcept
    -> void try {
  log::debug("sending fix message");
  FIX::Session::sendToTarget(fix_message, fix_session);
} catch (const FIX::SessionNotFound& exception) {
  log::err("failed to send message: session not found: {}", exception.detail);
} catch (const std::exception& exception) {
  log::err("failed to send message: an error occurred: {}", exception.what());
} catch (...) {
  log::err("failed to send message: unknown error occurred");
}

}  // namespace simulator::fix
