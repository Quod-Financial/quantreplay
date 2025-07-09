#include "ih/phases/states.hpp"

namespace simulator::trading_system::ies {

OpenState::OpenState(const Phase& phase) : phase_{phase} {}

OpenState::OpenState(const Phase& phase, bool halted_by_request)
    : phase_{phase}, halted_by_request_{halted_by_request} {}

auto OpenState::halt(const protocol::HaltPhaseRequest& request,
                     protocol::HaltPhaseReply& reply) const
    -> std::optional<State> {
  if (phase_.status() == TradingStatus::Option::Resume) {
    reply.result = protocol::HaltPhaseReply::Result::Halted;
    return OpenState{{TradingPhase::Option::Open,
                      TradingStatus::Option::Halt,
                      {.allow_cancels = request.allow_cancels}},
                     true};
  }

  if (halted_by_request_) {
    reply.result = protocol::HaltPhaseReply::Result::AlreadyHaltedByRequest;
  } else {
    reply.result = protocol::HaltPhaseReply::Result::UnableToHalt;
  }
  return std::nullopt;
}

auto OpenState::resume(
    [[maybe_unused]] const protocol::ResumePhaseRequest& request,
    protocol::ResumePhaseReply& reply) const -> std::optional<State> {
  if (halted_by_request_ && phase_.status() == TradingStatus::Option::Halt) {
    reply.result = protocol::ResumePhaseReply::Result::Resumed;
    return OpenState{
        {TradingPhase::Option::Open, TradingStatus::Option::Resume, {}}, false};
  }

  reply.result = protocol::ResumePhaseReply::Result::NoRequestedHalt;
  return std::nullopt;
}

auto OpenState::update(const Phase& scheduled_phase) const
    -> std::optional<State> {
  if (phase_ == scheduled_phase) {
    return std::nullopt;
  }

  if (scheduled_phase.phase() == TradingPhase::Option::Closed) {
    return ClosedState{};
  }

  return OpenState{scheduled_phase};
}

auto OpenState::phase() const -> Phase { return phase_; }

auto ClosedState::halt(
    [[maybe_unused]] const protocol::HaltPhaseRequest& request,
    protocol::HaltPhaseReply& reply) const -> std::optional<State> {
  reply.result = protocol::HaltPhaseReply::Result::UnableToHalt;
  return std::nullopt;
}
auto ClosedState::resume(
    [[maybe_unused]] const protocol::ResumePhaseRequest& request,
    protocol::ResumePhaseReply& reply) const -> std::optional<State> {
  reply.result = protocol::ResumePhaseReply::Result::NoRequestedHalt;
  return std::nullopt;
}
auto ClosedState::update(const Phase& scheduled_phase) const
    -> std::optional<State> {
  if (scheduled_phase.phase() == TradingPhase::Option::Open) {
    return OpenState{scheduled_phase};
  }
  return std::nullopt;
}
auto ClosedState::phase() const -> Phase { return phase_; }

auto create_state(const Phase& phase) -> std::optional<State> {
  switch (phase.phase()) {
    case TradingPhase::Option::Open:
      return OpenState{phase};
    case TradingPhase::Option::Closed:
      return ClosedState{};
    case TradingPhase::Option::PostTrading:
    case TradingPhase::Option::OpeningAuction:
    case TradingPhase::Option::IntradayAuction:
    case TradingPhase::Option::ClosingAuction:
      return std::nullopt;
  }
  return std::nullopt;
}

}  // namespace simulator::trading_system::ies