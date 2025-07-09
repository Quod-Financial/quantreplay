#ifndef SIMULATOR_IES_IH_TRADING_PHASE_CONTROLLER_HPP_
#define SIMULATOR_IES_IH_TRADING_PHASE_CONTROLLER_HPP_

#include <functional>
#include <mutex>

#include "common/events.hpp"
#include "ies/phase_schedule.hpp"
#include "ih/phases/phase_scheduler.hpp"
#include "ih/phases/states.hpp"
#include "protocol/admin/trading_phase.hpp"

namespace simulator::trading_system::ies {

class TradingPhaseController {
 public:
  auto set_tz_clock(const core::TzClock& tz_clock) -> void;

  auto configure(const PhaseSchedule& schedule) -> void;

  auto bind(std::function<void(event::PhaseTransition)> handler) -> void;

  auto update(const event::Tick& tick) -> void;

  auto process(protocol::HaltPhaseRequest request,
               protocol::HaltPhaseReply& reply) -> void;

  auto process(protocol::ResumePhaseRequest request,
               protocol::ResumePhaseReply& reply) -> void;

 private:
  auto create_tz_time_point() const -> core::tz_us;

  auto send_phase_transition(const core::tz_us& tz_time_point) -> void;

  auto transition_to(event::PhaseTransition event) -> void;

  core::TzClock tz_clock_;
  PhaseScheduler scheduler_;
  std::function<void(event::PhaseTransition)> handler_;

  std::mutex mutex_;
  std::optional<State> active_state_;
};

}  // namespace simulator::trading_system::ies

#endif  // SIMULATOR_IES_IH_TRADING_PHASE_CONTROLLER_HPP_
