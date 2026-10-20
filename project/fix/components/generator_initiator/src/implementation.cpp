#include "ih/implementation.hpp"

#include <quickfix/Session.h>
#include <quickfix/SessionID.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/SocketInitiator.h>

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iterator>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

#include "common/fix_logger.hpp"
#include "common/mapping/setting/mapping_settings.hpp"
#include "common/message_store.hpp"
#include "common/processors/event_processor.hpp"
#include "common/processors/message_processor.hpp"
#include "common/session_conversion.hpp"
#include "common/sessions.hpp"
#include "log/logging.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix {
namespace {

[[nodiscard]]
auto create_initiator_application(const MessageProcessor& reply_processor,
                                  const EventProcessor& event_processor)
    -> std::unique_ptr<Application> {
  try {
    log::debug("creating quickfix initiator application");
    return std::make_unique<Application>(reply_processor, event_processor);
  } catch (const std::exception& exception) {
    log::err(
        "failed to create a fix initiator application, an error occurred: {}",
        exception.what());
  } catch (...) {
    log::err(
        "failed to create a fix initiator application, unknown error occurred");
  }
  throw std::runtime_error{"failed to create fix initiator application"};
}

[[nodiscard]]
auto unregistered_sessions(const FIX::SessionSettings& settings)
    -> std::vector<FIX::SessionID> {
  const std::set<FIX::SessionID> sessions = settings.getSessions();
  std::vector<FIX::SessionID> unregistered;
  std::ranges::copy_if(sessions,
                       std::back_inserter(unregistered),
                       [](const FIX::SessionID& session_id) {
                         return !FIX::Session::doesSessionExist(session_id);
                       });
  return unregistered;
}

// A throwing FIX::Initiator constructor abandons the sessions it has already
// created, which would keep their ids registered for the rest of the process.
auto destroy_abandoned_sessions(const std::vector<FIX::SessionID>& session_ids)
    -> void {
  for (const FIX::SessionID& session_id : session_ids) {
    delete FIX::Session::lookupSession(session_id);
  }
}

[[nodiscard]]
auto create_fix_initiator_connection(
    const FIX::SessionSettings& settings,
    FIX::Application& application,
    FIX::MessageStoreFactory& persistence_factory,
    FIX::LogFactory& logger_factory) -> std::unique_ptr<FIX::SocketInitiator> {
  log::debug("creating quickfix initiator connection");
  const auto new_sessions = unregistered_sessions(settings);
  try {
    return std::make_unique<FIX::SocketInitiator>(
        application, persistence_factory, settings, logger_factory);
  } catch (const std::exception& exception) {
    log::err("failed to setup fix initiator connection, an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err(
        "failed to setup fix initiator connection, unknown error occurred");
  }
  destroy_abandoned_sessions(new_sessions);
  throw std::runtime_error{"failed to setup quickfix initiator connection"};
}

[[nodiscard]]
auto decode_sessions(const std::vector<FIX::Session*>& sessions)
    -> std::vector<protocol::fix::Session> {
  std::vector<protocol::fix::Session> decoded;
  decoded.reserve(sessions.size());
  for (const FIX::Session* session : sessions) {
    decoded.push_back(decode_fix_session(session->getSessionID()));
  }
  return decoded;
}

auto report_configured_sessions(const std::vector<FIX::Session*>& sessions)
    -> void {
  for (const FIX::Session* session : sessions) {
    log::info("fix initiator is configured with the '{}' session",
              session->getSessionID().toString());
  }
}

}  // namespace

GeneratorInitiator::Implementation::Implementation(
    const FIX::SessionSettings& settings)
    : application_{
          create_initiator_application(reply_processor_, event_processor_)},
      persistence_factory_{create_message_store_factory(settings)},
      logger_factory_{create_log_factory()},
      connection_{create_fix_initiator_connection(
          settings, *application_, *persistence_factory_, *logger_factory_)},
      sessions_{decode_sessions(get_sessions(*connection_))},
      request_sender_{message_sender_,
                      MappingSettings{get_sessions(*connection_)}} {
  report_configured_sessions(get_sessions(*connection_));
}

GeneratorInitiator::Implementation::~Implementation() noexcept {
  stop_connection();
}

auto GeneratorInitiator::Implementation::requester() noexcept
    -> RequestSender& {
  return request_sender_;
}

auto GeneratorInitiator::Implementation::sessions() const noexcept
    -> const std::vector<protocol::fix::Session>& {
  return sessions_;
}

auto GeneratorInitiator::Implementation::start_connection() -> void {
  try {
    fix_initiator_connection().start();
    return;
  } catch (const std::exception& exception) {
    log::err("failed to start fix initiator connection, an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err(
        "failed to start fix initiator connection, unknown error occurred");
  }
  throw std::runtime_error{"failed to start quickfix initiator connection"};
}

auto GeneratorInitiator::Implementation::stop_connection() noexcept -> void {
  FIX::Initiator& fix_initiator = fix_initiator_connection();
  try {
    if (!fix_initiator.isStopped()) {
      fix_initiator.stop();
    }
  } catch (const std::exception& exception) {
    log::err("an error occurred while stopping fix initiator connection: {}",
             exception.what());
  } catch (...) {
    log::err("unknown error occurred while stopping fix initiator connection");
  }
}

auto GeneratorInitiator::Implementation::fix_initiator_connection() noexcept
    -> FIX::Initiator& {
  if (connection_) [[likely]] {
    return *connection_;
  }

  log::err(
      "fix initiator connection is not allocated/initialized, "
      "this may indicate a critical bug in the component, can not continue "
      "program execution, aborting...");

  std::abort();
}

}  // namespace simulator::fix
