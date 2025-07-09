#ifndef SIMULATOR_APP_IH_DISPATCHERS_VENUE_TRADING_REPLY_DISPATCHER_HPP_
#define SIMULATOR_APP_IH_DISPATCHERS_VENUE_TRADING_REPLY_DISPATCHER_HPP_

#include <memory>

#include "core/tools/overload.hpp"
#include "ih/components/fix_acceptor.hpp"
#include "ih/components/generator.hpp"
#include "protocol/types/session.hpp"

namespace simulator {

struct VenueTradingReplyDispatcher final
    : public middleware::TradingReplyReceiver {
 public:
  VenueTradingReplyDispatcher(
      std::shared_ptr<Generator> generator,
      std::shared_ptr<FixAcceptor> fix_acceptor) noexcept
      : generator_(std::move(generator)),
        fix_acceptor_(std::move(fix_acceptor)) {}

  auto process(protocol::BusinessMessageReject reject) -> void override {
    dispatch_message(std::move(reject));
  }

  auto process(protocol::ExecutionReport report) -> void override {
    dispatch_message(std::move(report));
  }

  auto process(protocol::OrderPlacementReject reject) -> void override {
    dispatch_message(std::move(reject));
  }

  auto process(protocol::OrderPlacementConfirmation confirmation)
      -> void override {
    dispatch_message(std::move(confirmation));
  }

  auto process(protocol::OrderModificationReject reject) -> void override {
    dispatch_message(std::move(reject));
  }

  auto process(protocol::OrderModificationConfirmation confirmation)
      -> void override {
    dispatch_message(std::move(confirmation));
  }

  auto process(protocol::OrderCancellationReject reject) -> void override {
    dispatch_message(std::move(reject));
  }

  auto process(protocol::OrderCancellationConfirmation confirmation)
      -> void override {
    dispatch_message(std::move(confirmation));
  }

  auto process(protocol::MarketDataReject reject) -> void override {
    dispatch_message(std::move(reject));
  }

  auto process(protocol::MarketDataSnapshot snapshot) -> void override {
    dispatch_message(std::move(snapshot));
  }

  auto process(protocol::MarketDataUpdate update) -> void override {
    dispatch_message(std::move(update));
  }

  auto process(protocol::SecurityStatus status) -> void override {
    dispatch_message(std::move(status));
  }

 private:
  template <typename Message>
  auto dispatch_message(Message message) -> void {
    const auto by_session_type_dispatcher = core::overload(
        [&](const protocol::fix::Session& /*session*/) {
          fix_acceptor_->process(std::move(message));
        },
        [&](const protocol::generator::Session& /*session*/) {
          generator_->process(std::move(message));
        });

    std::visit(by_session_type_dispatcher, message.session.value);
  }

  std::shared_ptr<Generator> generator_;
  std::shared_ptr<FixAcceptor> fix_acceptor_;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_DISPATCHERS_VENUE_TRADING_REPLY_DISPATCHER_HPP_
