#include "fix/trading_system_acceptor/transport.hpp"

#include <exception>

#include "ih/implementation.hpp"
#include "log/logging.hpp"

namespace simulator::fix {
namespace {

template <typename Message>
auto send_reply_message(
    const Message& reply_message,
    TradingSystemAcceptor::Implementation& acceptor) noexcept -> void {
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

auto send_reply(const protocol::BusinessMessageReject& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending BusinessMessageReject");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::ExecutionReport& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending ExecutionReport");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderPlacementConfirmation& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderPlacementConfirmation reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderPlacementReject& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderPlacementReject reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderModificationConfirmation& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderModificationConfirmation reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderModificationReject& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderModificationReject reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderCancellationConfirmation& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderCancellationConfirmation reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::OrderCancellationReject& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending OrderCancellationReject reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::MarketDataReject& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending MarketDataReject reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::MarketDataSnapshot& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending MarketDataSnapshot reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::MarketDataUpdate& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending MarketDataUpdate reply");
  send_reply_message(reply, acceptor.implementation());
}

auto send_reply(const protocol::SecurityStatus& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void {
  log::debug("fix acceptor sending SecurityStatus reply");
  send_reply_message(reply, acceptor.implementation());
}

}  // namespace simulator::fix
