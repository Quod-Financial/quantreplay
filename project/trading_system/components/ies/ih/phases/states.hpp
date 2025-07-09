#ifndef SIMULATOR_IES_IH_PHASES_STATES_HPP_
#define SIMULATOR_IES_IH_PHASES_STATES_HPP_

#include <variant>

#include "common/phase.hpp"
#include "protocol/admin/trading_phase.hpp"

namespace simulator::trading_system::ies {

class OpenState;
class ClosedState;

using State = std::variant<OpenState, ClosedState>;

class OpenState {
 public:
  explicit OpenState(const Phase& phase);

  auto halt(const protocol::HaltPhaseRequest& request,
            protocol::HaltPhaseReply& reply) const -> std::optional<State>;

  auto resume(const protocol::ResumePhaseRequest& request,
              protocol::ResumePhaseReply& reply) const -> std::optional<State>;

  auto update(const Phase& scheduled_phase) const -> std::optional<State>;

  auto phase() const -> Phase;

  auto operator==(const OpenState& state) const -> bool = default;

 private:
  OpenState(const Phase& phase, bool halted_by_request);

  Phase phase_;
  bool halted_by_request_{false};
};

class ClosedState {
 public:
  auto halt(const protocol::HaltPhaseRequest& request,
            protocol::HaltPhaseReply& reply) const -> std::optional<State>;

  auto resume(const protocol::ResumePhaseRequest& request,
              protocol::ResumePhaseReply& reply) const -> std::optional<State>;

  auto update(const Phase& scheduled_phase) const -> std::optional<State>;

  auto phase() const -> Phase;

  auto operator==(const ClosedState& state) const -> bool = default;

 private:
  Phase phase_{TradingPhase::Option::Closed, TradingStatus::Option::Halt, {}};
};

auto create_state(const Phase& phase) -> std::optional<State>;

}  // namespace simulator::trading_system::ies

#endif  // SIMULATOR_IES_IH_PHASES_STATES_HPP_
