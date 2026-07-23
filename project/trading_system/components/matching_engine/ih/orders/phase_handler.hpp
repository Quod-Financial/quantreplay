#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_PHASE_HANDLER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_PHASE_HANDLER_HPP_

#include <memory>

#include "common/events.hpp"
#include "core/domain/attributes.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine::order {

class PhaseHandler : EventReporter {
 public:
  struct Subscription;
  struct Index;

  explicit PhaseHandler(EventListener& event_listener);

  PhaseHandler() = delete;
  PhaseHandler(const PhaseHandler&) = delete;
  PhaseHandler(PhaseHandler&&) noexcept = delete;

  auto operator=(const PhaseHandler&) -> PhaseHandler& = delete;
  auto operator=(PhaseHandler&&) noexcept -> PhaseHandler& = delete;

  ~PhaseHandler() override;

  [[nodiscard]]
  auto in_closed_phase() const -> bool {
    return current_state_.trading_phase() == TradingPhase::Option::Closed;
  }

  [[nodiscard]]
  auto in_halt_phase() const -> bool {
    return current_state_.trading_status() == TradingStatus::Option::Halt;
  }

  [[nodiscard]]
  auto in_auction_phase() const -> bool {
    const auto phase = current_state_.trading_phase();
    return phase == TradingPhase::Option::OpeningAuction ||
           phase == TradingPhase::Option::IntradayAuction ||
           phase == TradingPhase::Option::ClosingAuction;
  }

  [[nodiscard]]
  auto in_auction_call() const -> bool {
    return in_auction_phase() &&
           current_state_.trading_status() == TradingStatus::Option::Resume;
  }

  [[nodiscard]]
  auto in_auction_uncross() const -> bool {
    return in_auction_phase() &&
           current_state_.trading_status() == TradingStatus::Option::Halt;
  }

  [[nodiscard]]
  auto current_phase() const -> MarketPhase {
    return current_state_;
  }

  auto handle(event::PhaseTransition transition) -> bool;

  auto process(const protocol::SecurityStatusRequest& request) -> void;

  auto unsubscribe(const protocol::Session& session) -> void;

 private:
  auto subscribe(const protocol::SecurityStatusRequest& request) -> void;

  auto snapshot(const protocol::SecurityStatusRequest& request) -> void;

  auto unsubscribe(const protocol::SecurityStatusRequest& request) -> void;

  auto publish(const Subscription& subscription) -> void;

  auto reject(const protocol::SecurityStatusRequest& request,
              BusinessRejectReason reason,
              std::string text) const -> void;

  std::unique_ptr<Index> subscriptions_;
  MarketPhase current_state_ = MarketPhase::open();
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_PHASE_HANDLER_HPP_
