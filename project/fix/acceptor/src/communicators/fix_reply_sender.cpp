#include "ih/communicators/fix_reply_sender.hpp"

#include <quickfix/Exceptions.h>
#include <quickfix/Session.h>

#include <exception>

#include "log/logging.hpp"

namespace simulator::fix::acceptor {

auto FixReplySender::send_reply_message(
    FIX::Message& fix_message, const FIX::SessionID& fix_session) const noexcept
    -> void try {
  log::debug("sending fix reply message");
  FIX::Session::sendToTarget(fix_message, fix_session);
} catch (const FIX::SessionNotFound& exception) {
  log::err("failed to send reply message: session not found: {}",
           exception.detail);
} catch (const std::exception& exception) {
  log::err("failed to send reply message: an error occurred: {}",
           exception.what());
} catch (...) {
  log::err("failed to send reply message: unknown error occurred");
}

}  // namespace simulator::fix::acceptor