#include "ih/communicators/application.hpp"

#include <quickfix/Exceptions.h>
#include <quickfix/Session.h>

#include <stdexcept>

#include "ih/processors/event_processor.hpp"
#include "ih/processors/request_processor.hpp"
#include "log/logging.hpp"

namespace simulator::fix::acceptor {

Application::Application(const RequestProcessor& request_processor,
                         const EventProcessor& event_processor) noexcept
    : request_processor_(request_processor),
      event_processor_(event_processor) {}

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
    process_request(request_processor_, fix_session, fix_message);
  } catch (const FIX::FieldNotFound& exception) {
    log::warn("failed to process request: {}, tag '{}'",
              exception.what(),
              exception.field);
    throw;
  } catch (const FIX::IncorrectDataFormat& exception) {
    log::warn("failed to process request: {}, tag '{}'",
              exception.what(),
              exception.field);
    throw;
  } catch (const FIX::IncorrectTagValue& exception) {
    log::warn("failed to process request: {}, tag '{}'",
              exception.what(),
              exception.field);
    throw;
  } catch (const FIX::UnsupportedMessageType& exception) {
    log::warn("failed to process request: message type '{}' is not supported",
              exception.detail);
    throw;
  } catch (const std::exception& exception) {
    log::err("failed to process request: an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to process request: unknown error occurred");
  }
}

auto Application::process_request(const RequestProcessor& request_processor,
                                  const FIX::SessionID& fix_session,
                                  const FIX::Message& fix_message) -> void {
  request_processor.process_fix_request(fix_message, fix_session);
}

auto Application::emit_session_disconnection_event(
    const EventProcessor& event_processor, const FIX::SessionID& fix_session)
    -> void {
  event_processor.process_session_disconnection(fix_session);
}

}  // namespace simulator::fix::acceptor