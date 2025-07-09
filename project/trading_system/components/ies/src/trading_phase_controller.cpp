#include "ih/trading_phase_controller.hpp"

#include "log/logging.hpp"

namespace simulator::trading_system::ies {

auto TradingPhaseController::set_tz_clock(const core::TzClock& tz_clock)
    -> void {
  tz_clock_ = tz_clock;
}

auto TradingPhaseController::configure(const PhaseSchedule& schedule) -> void {
  scheduler_.configure(schedule);
  log::debug("phase controller was supplied with a phase schedule");
}

auto TradingPhaseController::bind(
    std::function<void(event::PhaseTransition)> handler) -> void {
  std::swap(handler_, handler);
  log::debug("phase transition handler was bound");
}

auto TradingPhaseController::update(const event::Tick& tick) -> void {
  std::lock_guard lock{mutex_};

  log::trace("updating phase controller triggered by {}", tick);
  if (const auto phase = scheduler_.update(tick)) {
    std::optional<State> new_state;
    if (active_state_) {
      new_state = std::visit(
          [&phase](auto& state) -> std::optional<State> {
            return state.update(*phase);
          },
          *active_state_);
    } else {
      new_state = create_state(*phase);
    }

    if (new_state && new_state != active_state_) {
      active_state_ = new_state;
      send_phase_transition(tick.tz_tick_time);
    }
  }
  log::trace("phase controller update triggered by {} was completed", tick);
}

auto TradingPhaseController::process(protocol::HaltPhaseRequest request,
                                     protocol::HaltPhaseReply& reply) -> void {
  std::lock_guard lock{mutex_};

  if (!active_state_) {
    reply.result = protocol::HaltPhaseReply::Result::NoActivePhase;
    log::debug("halt phase request was rejected, no active phase");
    return;
  }

  const auto new_state = std::visit(
      [&request, &reply](auto& state) -> std::optional<State> {
        return state.halt(request, reply);
      },
      *active_state_);
  if (new_state && new_state != active_state_) {
    active_state_ = new_state;
    send_phase_transition(create_tz_time_point());
  }
}

auto TradingPhaseController::process(
    [[maybe_unused]] protocol::ResumePhaseRequest request,
    protocol::ResumePhaseReply& reply) -> void {
  std::lock_guard lock{mutex_};

  if (!active_state_) {
    reply.result = protocol::ResumePhaseReply::Result::NoRequestedHalt;
    log::debug("resume phase request was rejected, no active phase");
    return;
  }

  const auto new_state = std::visit(
      [&request, &reply](auto& state) -> std::optional<State> {
        return state.resume(request, reply);
      },
      *active_state_);
  if (new_state && new_state != active_state_) {
    active_state_ = new_state;
    send_phase_transition(create_tz_time_point());
  }
}

auto TradingPhaseController::create_tz_time_point() const -> core::tz_us {
  return core::as_tz_time(core::get_current_system_time(), tz_clock_);
}

auto TradingPhaseController::send_phase_transition(
    const core::tz_us& tz_time_point) -> void {
  const auto active_phase =
      std::visit([](auto& state) { return state.phase(); }, *active_state_);
  transition_to({.tz_time_point = tz_time_point, .phase = active_phase});
}

auto TradingPhaseController::transition_to(event::PhaseTransition event)
    -> void {
  log::trace("reporting a phase transition event: {}", event);
  if (handler_) {
    handler_(event);
    log::debug("reported transition to {} trading phase", event.phase);
  } else {
    log::err("unable to report {}, no handler is bound", event);
  }
}

}  // namespace simulator::trading_system::ies
