#include "ih/implementation.hpp"

#include <quickfix/Session.h>
#include <quickfix/SocketInitiator.h>

#include <cstdlib>
#include <exception>
#include <memory>
#include <stdexcept>
#include <vector>

#include "common/fix_logger.hpp"
#include "common/mapping/setting/mapping_settings.hpp"
#include "common/message_store.hpp"
#include "common/processors/event_processor.hpp"
#include "common/processors/message_processor.hpp"
#include "common/sessions.hpp"
#include "log/logging.hpp"

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
auto create_fix_initiator_connection(
    const FIX::SessionSettings& settings,
    FIX::Application& application,
    FIX::MessageStoreFactory& persistence_factory,
    FIX::LogFactory& logger_factory) -> std::unique_ptr<FIX::SocketInitiator> {
  log::debug("creating quickfix initiator connection");
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
  throw std::runtime_error{"failed to setup quickfix initiator connection"};
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
