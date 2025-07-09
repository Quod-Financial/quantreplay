#include "common/phase.hpp"

namespace simulator::trading_system {

Phase::Phase(TradingPhase phase,
             TradingStatus status,
             Settings setting) noexcept
    : phase_{phase}, status_{status} {
  if (phase_ == TradingPhase::Option::Closed) {
    status_ = TradingStatus::Option::Halt;
  }

  if (phase_ != TradingPhase::Option::Closed &&
      status_ == TradingStatus::Option::Halt) {
    settings_ = setting;
  }
}

auto Phase::phase() const -> TradingPhase { return phase_; }

auto Phase::status() const -> TradingStatus { return status_; }

auto Phase::settings() const -> std::optional<Settings> {
  return settings_;
}

}  // namespace simulator::trading_system
