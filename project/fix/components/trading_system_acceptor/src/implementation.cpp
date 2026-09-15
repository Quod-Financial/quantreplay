#include "ih/implementation.hpp"

#include <quickfix/ThreadedSocketAcceptor.h>

#include <cstdlib>
#include <exception>
#include <memory>
#include <stdexcept>

#include "common/fix_logger.hpp"
#include "common/message_store.hpp"
#include "common/processors/event_processor.hpp"
#include "common/processors/message_processor.hpp"
#include "common/sessions.hpp"
#include "log/logging.hpp"

namespace simulator::fix {
namespace {

[[nodiscard]]
auto create_acceptor_application(const MessageProcessor& request_processor,
                                 const EventProcessor& event_processor)
    -> std::unique_ptr<Application> {
  try {
    log::debug("creating quickfix acceptor application");
    return std::make_unique<Application>(request_processor, event_processor);
  } catch (const std::exception& exception) {
    log::err(
        "failed to create a fix acceptor application, an error occurred: {}",
        exception.what());
  } catch (...) {
    log::err(
        "failed to create a fix acceptor application, unknown error occurred");
  }
  throw std::runtime_error("failed to create fix acceptor application");
}

[[nodiscard]]
auto create_fix_acceptor_server(const FIX::SessionSettings& settings,
                                FIX::Application& application,
                                FIX::MessageStoreFactory& persistence_factory,
                                FIX::LogFactory& logger_factory)
    -> std::unique_ptr<FIX::ThreadedSocketAcceptor> {
  log::debug("creating quickfix acceptor server");
  try {
    return std::make_unique<FIX::ThreadedSocketAcceptor>(
        application, persistence_factory, settings, logger_factory);
  } catch (const std::exception& exception) {
    log::err("failed to setup fix acceptor server, an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to setup fix acceptor server, unknown error occurred");
  }
  throw std::runtime_error("failed to setup quickfix acceptor server");
}

}  // namespace

TradingSystemAcceptor::Implementation::Implementation(
    const FIX::SessionSettings& settings)
    : application_(
          create_acceptor_application(request_processor_, event_processor_)),
      persistence_factory_(create_message_store_factory(settings)),
      logger_factory_(create_log_factory()),
      server_(create_fix_acceptor_server(
          settings, *application_, *persistence_factory_, *logger_factory_)),
      reply_processor_(reply_sender_, MappingSettings{get_sessions(*server_)}) {
}

TradingSystemAcceptor::Implementation::~Implementation() noexcept {
  stop_server();
}

auto TradingSystemAcceptor::Implementation::replier() noexcept
    -> ReplyProcessor& {
  return reply_processor_;
}

auto TradingSystemAcceptor::Implementation::start_server() -> void {
  try {
    fix_acceptor_server().start();
    return;
  } catch (const std::exception& exception) {
    log::err("failed to start fix acceptor server, an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to start fix acceptor server unknown error occurred");
  }
  throw std::runtime_error("failed to start quickfix acceptor server");
}

auto TradingSystemAcceptor::Implementation::stop_server() noexcept -> void {
  FIX::Acceptor& fix_acceptor = fix_acceptor_server();
  try {
    if (!fix_acceptor.isStopped()) {
      fix_acceptor.stop();
    }
  } catch (const std::exception& exception) {
    log::err("an error occurred while stopping fix acceptor server: {}",
             exception.what());
  } catch (...) {
    log::err("unknown error occurred while stopping fix acceptor server");
  }
}

auto TradingSystemAcceptor::Implementation::fix_acceptor_server() noexcept
    -> FIX::Acceptor& {
  if (server_) [[likely]] {
    return *server_;
  }

  log::err(
      "fix acceptor server is not allocated/initialized, "
      "this may indicate a critical bug in the component, can not continue "
      "program execution, aborting...");

  std::abort();
}

}  // namespace simulator::fix
