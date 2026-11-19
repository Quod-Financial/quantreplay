#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_PHASE_HANDLER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_PHASE_HANDLER_HPP_

#include "common/events.hpp"
#include "common/security_status_publisher.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/market_phase.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "protocol/app/business_message_reject.hpp"
#include "protocol/app/security_status.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine::order {

class PhaseHandler : EventReporter, SecurityStatusPublisher::ReplySender {
 public:
  explicit PhaseHandler(EventListener& event_listener);

  PhaseHandler() = delete;
  PhaseHandler(const PhaseHandler&) = delete;
  PhaseHandler(PhaseHandler&&) noexcept = delete;

  auto operator=(const PhaseHandler&) -> PhaseHandler& = delete;
  auto operator=(PhaseHandler&&) noexcept -> PhaseHandler& = delete;

  ~PhaseHandler() override;

  [[nodiscard]]
  auto in_closed_phase() const -> bool {
    return current_phase().trading_phase() == TradingPhase::Option::Closed;
  }

  [[nodiscard]]
  auto in_halt_phase() const -> bool {
    return current_phase().trading_status() == TradingStatus::Option::Halt;
  }

  [[nodiscard]]
  auto in_auction_phase() const -> bool {
    const auto phase = current_phase().trading_phase();
    return phase == TradingPhase::Option::OpeningAuction ||
           phase == TradingPhase::Option::IntradayAuction ||
           phase == TradingPhase::Option::ClosingAuction;
  }

  [[nodiscard]]
  auto in_auction_call() const -> bool {
    return in_auction_phase() &&
           current_phase().trading_status() == TradingStatus::Option::Resume;
  }

  [[nodiscard]]
  auto in_auction_uncross() const -> bool {
    return in_auction_phase() &&
           current_phase().trading_status() == TradingStatus::Option::Halt;
  }

  [[nodiscard]]
  auto in_trade_at_last() const -> bool {
    return current_phase().trading_phase() == TradingPhase::Option::PostTrading;
  }

  [[nodiscard]]
  auto current_phase() const -> MarketPhase {
    return security_status_.market_phase();
  }

  auto handle(event::PhaseTransition transition) -> bool;

  auto process(const protocol::SecurityStatusRequest& request) -> void;

  auto unsubscribe(const protocol::Session& session) -> void;

 private:
  auto send(protocol::SecurityStatus status) -> void override;

  auto send(protocol::BusinessMessageReject reject) -> void override;

  SecurityStatusPublisher security_status_;
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_PHASE_HANDLER_HPP_
