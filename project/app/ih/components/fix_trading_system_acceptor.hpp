#ifndef SIMULATOR_APP_IH_COMPONENTS_FIX_TRADING_SYSTEM_ACCEPTOR_HPP_
#define SIMULATOR_APP_IH_COMPONENTS_FIX_TRADING_SYSTEM_ACCEPTOR_HPP_

#include <filesystem>

#include "fix/trading_system_acceptor/lifetime.hpp"
#include "fix/trading_system_acceptor/trading_system_acceptor.hpp"
#include "fix/trading_system_acceptor/transport.hpp"
#include "middleware/channels/trading_reply_channel.hpp"

namespace simulator {

class FixTradingSystemAcceptor final : public middleware::TradingReplyReceiver {
 public:
  explicit FixTradingSystemAcceptor(
      const std::filesystem::path& fix_config_path)
      : acceptor_(fix::create_trading_system_acceptor(fix_config_path)) {}

  auto launch() -> void { fix::start_trading_system_acceptor(acceptor_); }

  auto terminate() -> void { fix::stop_trading_system_acceptor(acceptor_); }

  auto process(protocol::BusinessMessageReject reject) -> void override {
    fix::send_reply(reject, acceptor_);
  }

  auto process(protocol::ExecutionReport report) -> void override {
    fix::send_reply(report, acceptor_);
  }

  auto process(protocol::OrderPlacementReject reject) -> void override {
    fix::send_reply(reject, acceptor_);
  }

  auto process(protocol::OrderPlacementConfirmation confirmation)
      -> void override {
    fix::send_reply(confirmation, acceptor_);
  }

  auto process(protocol::OrderModificationReject reject) -> void override {
    fix::send_reply(reject, acceptor_);
  }

  auto process(protocol::OrderModificationConfirmation confirmation)
      -> void override {
    fix::send_reply(confirmation, acceptor_);
  }

  auto process(protocol::OrderCancellationReject reject) -> void override {
    fix::send_reply(reject, acceptor_);
  }

  auto process(protocol::OrderCancellationConfirmation confirmation)
      -> void override {
    fix::send_reply(confirmation, acceptor_);
  }

  auto process(protocol::MarketDataReject reject) -> void override {
    fix::send_reply(reject, acceptor_);
  }

  auto process(protocol::MarketDataSnapshot snapshot) -> void override {
    fix::send_reply(snapshot, acceptor_);
  }

  auto process(protocol::MarketDataUpdate update) -> void override {
    fix::send_reply(update, acceptor_);
  }

  auto process(protocol::SecurityStatus status) -> void override {
    fix::send_reply(status, acceptor_);
  }

 private:
  fix::TradingSystemAcceptor acceptor_;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_COMPONENTS_FIX_TRADING_SYSTEM_ACCEPTOR_HPP_