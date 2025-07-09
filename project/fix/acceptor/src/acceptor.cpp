#include "ih/acceptor.hpp"

#include <quickfix/ThreadedSocketAcceptor.h>

#include <cstdlib>
#include <memory>
#include <stdexcept>

#include "acceptor/lifetime.hpp"
#include "acceptor/transport.hpp"
#include "common/fix_logger.hpp"
#include "common/message_store.hpp"
#include "common/session_settings.hpp"
#include "ih/processors/event_processor.hpp"
#include "ih/processors/request_processor.hpp"
#include "log/logging.hpp"

namespace simulator::fix {
namespace {

[[nodiscard]]
auto create_acceptor_application(
    const acceptor::RequestProcessor& request_processor,
    const acceptor::EventProcessor& event_processor)
    -> std::unique_ptr<acceptor::Application> {
  try {
    log::debug("creating quickfix acceptor application");
    return std::make_unique<acceptor::Application>(request_processor,
                                                   event_processor);
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

[[nodiscard]]
auto get_sessions(const FIX::Acceptor& acceptor) -> std::vector<FIX::Session*> {
  std::vector<FIX::Session*> sessions;
  for (const auto& session : acceptor.getSessions()) {
    sessions.push_back(acceptor.getSession(session));
  }
  return sessions;
}

auto create_acceptor_implementation(const FIX::SessionSettings& settings)
    -> std::unique_ptr<Acceptor::Implementation> {
  return std::make_unique<Acceptor::Implementation>(settings);
}

template <typename Message>
auto send_reply_message(const Message& reply_message,
                        Acceptor::Implementation& acceptor) noexcept -> void {
  try {
    acceptor.replier().process_reply(reply_message);
  } catch (const std::exception& exception) {
    log::err(
        "failed to send reply message, an error occurred: {}, "
        "undelivered message - {}",
        exception.what(),
        reply_message);
  } catch (...) {
    log::err(
        "failed to send reply message, unknown error occurred, "
        "undelivered message - {}",
        reply_message);
  }
}

}  // namespace

Acceptor::Acceptor(std::unique_ptr<Implementation> impl) noexcept
    : impl_(std::move(impl)) {}

Acceptor::Acceptor(simulator::fix::Acceptor&&) noexcept = default;

Acceptor::~Acceptor() noexcept = default;

auto Acceptor::operator=(simulator::fix::Acceptor&&) noexcept
    -> Acceptor& = default;

auto Acceptor::implementation() noexcept -> Implementation& {
  if (impl_) [[likely]] {
    return *impl_;
  }

  log::err(
      "fix acceptor implementation has not been allocated/initialized, "
      "this may indicate a critical bug in the component, can not continue "
      "execution, aborting...");

  std::abort();
}

Acceptor::Implementation::Implementation(const FIX::SessionSettings& settings)
    : application_(
          create_acceptor_application(request_processor_, event_processor_)),
      persistence_factory_(create_message_store_factory(settings)),
      logger_factory_(create_log_factory()),
      server_(create_fix_acceptor_server(
          settings, *application_, *persistence_factory_, *logger_factory_)),
      reply_processor_(reply_sender_, MappingSettings{get_sessions(*server_)}) {
}

Acceptor::Implementation::~Implementation() noexcept { stop_server(); }

auto Acceptor::Implementation::replier() noexcept -> ReplyProcessor& {
  return reply_processor_;
}

auto Acceptor::Implementation::start_server() -> void {
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

auto Acceptor::Implementation::stop_server() noexcept -> void {
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

auto Acceptor::Implementation::fix_acceptor_server() noexcept
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

auto create_fix_acceptor(const std::filesystem::path& config_path) -> Acceptor {
  log::debug("creating a FIX acceptor instance");

  const auto configuration = read_settings_from_file(config_path);
  Acceptor acceptor{create_acceptor_implementation(configuration)};

  log::info("created a FIX acceptor instance");

  return acceptor;
}

auto start_fix_acceptor(Acceptor& acceptor) -> void {
  log::debug("starting FIX acceptor");

  acceptor.implementation().start_server();

  log::info("started FIX acceptor");
}

auto stop_fix_acceptor(Acceptor& acceptor) noexcept -> void {
  log::debug("stopping FIX acceptor");

  acceptor.implementation().stop_server();

  log::info("stopped FIX acceptor");
}

auto send_reply(const protocol::BusinessMessageReject& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending BusinessMessageReject");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::ExecutionReport& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending ExecutionReport");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderPlacementConfirmation& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderPlacementConfirmation reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderPlacementReject& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderPlacementReject reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderModificationConfirmation& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderModificationConfirmation reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderModificationReject& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderModificationReject reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderCancellationConfirmation& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderCancellationConfirmation reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderCancellationReject& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderCancellationReject reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::MarketDataReject& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending MarketDataReject reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::MarketDataSnapshot& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending MarketDataSnapshot reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::MarketDataUpdate& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending MarketDataUpdate reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::SecurityStatus& reply,
                Acceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending SecurityStatus reply");
  send_reply_message(reply, acceptor.implementation());
}

}  // namespace simulator::fix