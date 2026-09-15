#include "common/communicators/application.hpp"

#include <quickfix/Exceptions.h>
#include <quickfix/Session.h>

#include <stdexcept>

#include "common/processors/event_processor.hpp"
#include "common/processors/message_processor.hpp"
#include "log/logging.hpp"

namespace simulator::fix {

Application::Application(const MessageProcessor& message_processor,
                         const EventProcessor& event_processor) noexcept
    : message_processor_(message_processor),
      event_processor_(event_processor) {}

auto Application::onLogon(const FIX::SessionID& fix_session) -> void {
  try {
    log::debug("application accepted session connection event");
    emit_session_connection_event(event_processor_, fix_session);
  } catch (const std::exception& exception) {
    log::err("failed to handle session connection: an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to handle session connection: unknown error occurred");
  }
}

auto Application::onLogout(const FIX::SessionID& fix_session) -> void {
  try {
    log::debug("application accepted session disconnection event");
    emit_session_disconnection_event(event_processor_, fix_session);
  } catch (const std::exception& exception) {
    log::err("failed to handle session disconnection: an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to handle session disconnection: unknown error occurred");
  }
}

auto Application::fromApp(const FIX::Message& fix_message,
                          const FIX::SessionID& fix_session) noexcept(false)
    -> void {
  try {
    log::debug("application accepted fix message");
    process_message(message_processor_, fix_session, fix_message);
  } catch (const FIX::FieldNotFound& exception) {
    log::warn("failed to process message: {}, tag '{}'",
              exception.what(),
              exception.field);
    throw;
  } catch (const FIX::IncorrectDataFormat& exception) {
    log::warn("failed to process message: {}, tag '{}'",
              exception.what(),
              exception.field);
    throw;
  } catch (const FIX::IncorrectTagValue& exception) {
    log::warn("failed to process message: {}, tag '{}'",
              exception.what(),
              exception.field);
    throw;
  } catch (const FIX::UnsupportedMessageType& exception) {
    log::warn("failed to process message: message type '{}' is not supported",
              exception.detail);
    throw;
  } catch (const std::exception& exception) {
    log::err("failed to process message: an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to process message: unknown error occurred");
  }
}

auto Application::process_message(const MessageProcessor& message_processor,
                                  const FIX::SessionID& fix_session,
                                  const FIX::Message& fix_message) -> void {
  message_processor.process_message(fix_message, fix_session);
}

auto Application::emit_session_connection_event(
    const EventProcessor& event_processor, const FIX::SessionID& fix_session)
    -> void {
  event_processor.process_session_connection(fix_session);
}

auto Application::emit_session_disconnection_event(
    const EventProcessor& event_processor, const FIX::SessionID& fix_session)
    -> void {
  event_processor.process_session_disconnection(fix_session);
}

}  // namespace simulator::fix
