#include "ih/phases/states.hpp"

#include <cassert>

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
  if (phase_ == scheduled_phase && !halted_by_request_) {
    return std::nullopt;
  }

  if (scheduled_phase.phase() == TradingPhase::Option::Closed) {
    return ClosedState{};
  }

  return OpenState{scheduled_phase};
}

auto OpenState::phase() const -> Phase { return phase_; }

auto OpenState::halted_by_request() const -> bool { return halted_by_request_; }

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

AuctionState::AuctionState(TradingPhase auction_phase,
                           const AuctionActivation& activation)
    : AuctionState{auction_phase,
                   SubPhase::Call,
                   activation.end,
                   activation.uncross_at} {}

AuctionState::AuctionState(TradingPhase auction_phase,
                           SubPhase sub_phase,
                           core::local_us end,
                           core::local_us uncross_at)
    : phase_{auction_phase},
      sub_phase_{sub_phase},
      end_{end},
      uncross_at_{uncross_at} {
  assert(is_auction_phase(auction_phase));
}

auto AuctionState::halt(
    [[maybe_unused]] const protocol::HaltPhaseRequest& request,
    protocol::HaltPhaseReply& reply) const -> std::optional<State> {
  reply.result = protocol::HaltPhaseReply::Result::AuctionInProgress;
  return std::nullopt;
}

auto AuctionState::resume(
    [[maybe_unused]] const protocol::ResumePhaseRequest& request,
    protocol::ResumePhaseReply& reply) const -> std::optional<State> {
  reply.result = protocol::ResumePhaseReply::Result::AuctionInProgress;
  return std::nullopt;
}

auto AuctionState::update([[maybe_unused]] const Phase& scheduled_phase) const
    -> std::optional<State> {
  // An active auction follows its own randomised timeline, not scheduler
  // boundaries (the controller drives it)
  return std::nullopt;
}

auto AuctionState::phase() const -> Phase {
  const auto status = sub_phase_ == SubPhase::Uncrossing
                          ? TradingStatus::Option::Halt
                          : TradingStatus::Option::Resume;
  return {phase_, status, {}};
}

auto AuctionState::uncrossing_due(core::local_us now) const -> bool {
  return sub_phase_ == SubPhase::Call && now >= uncross_at_;
}

auto AuctionState::end_time() const -> core::local_us { return end_; }

auto AuctionState::begin_uncrossing() const -> AuctionState {
  return {phase_, SubPhase::Uncrossing, end_, uncross_at_};
}

auto is_auction_phase(TradingPhase phase) -> bool {
  switch (phase) {
    case TradingPhase::Option::OpeningAuction:
    case TradingPhase::Option::IntradayAuction:
    case TradingPhase::Option::ClosingAuction:
      return true;
    case TradingPhase::Option::Open:
    case TradingPhase::Option::Closed:
    case TradingPhase::Option::PostTrading:
      return false;
  }
  return false;
}

auto create_state(const Phase& phase,
                  const std::optional<AuctionActivation>& activation)
    -> std::optional<State> {
  switch (phase.phase()) {
    case TradingPhase::Option::Open:
      return OpenState{phase};
    case TradingPhase::Option::Closed:
      return ClosedState{};
    case TradingPhase::Option::OpeningAuction:
    case TradingPhase::Option::IntradayAuction:
    case TradingPhase::Option::ClosingAuction:
      if (activation) {
        return AuctionState{phase.phase(), *activation};
      }
      return std::nullopt;
    case TradingPhase::Option::PostTrading:
      return std::nullopt;
  }
  return std::nullopt;
}

}  // namespace simulator::trading_system::ies
