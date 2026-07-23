#include "ih/trading_phase_controller.hpp"

#include <algorithm>
#include <chrono>
#include <memory>
#include <random>
#include <utility>
#include <variant>

#include "common/phase.hpp"
#include "core/tools/time.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::ies {

namespace {

auto make_default_uncross_delay_generator()
    -> TradingPhaseController::UncrossDelayGenerator {
  auto engine = std::make_shared<std::mt19937>(std::random_device{}());
  return [engine](std::chrono::seconds low,
                  std::chrono::seconds high) -> std::chrono::seconds {
    if (high <= low) {
      return low;
    }
    std::uniform_int_distribution<std::chrono::seconds::rep> distribution{
        low.count(), high.count()};
    return std::chrono::seconds{distribution(*engine)};
  };
}

auto fallback_open_phase() -> Phase {
  return {TradingPhase::Option::Open, TradingStatus::Option::Resume, {}};
}

}  // namespace

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

auto TradingPhaseController::set_uncross_delay_generator(
    UncrossDelayGenerator generator) -> void {
  uncross_delay_generator_ = std::move(generator);
  log::debug("auction uncross delay generator was set");
}

auto TradingPhaseController::update(const event::Tick& tick) -> void {
  std::lock_guard lock{mutex_};

  log::trace("updating phase controller triggered by {}", tick);

  // An active auction owns the timeline (uncrossing + jump driven below); the
  // scheduler is held, so its last phase stays the auction.
  if (active_state_ && std::holds_alternative<AuctionState>(*active_state_)) {
    advance_active_auction(tick);
  } else if (const auto scheduled = scheduler_.update(tick)) {
    enter_scheduled_phase(*scheduled, tick);
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

auto TradingPhaseController::enter_scheduled_phase(
    const ScheduledPhase& scheduled, const event::Tick& tick) -> void {
  std::optional<State> new_state;
  if (is_auction_phase(scheduled.phase.phase())) {
    new_state = activate_auction(scheduled, tick);
  } else if (active_state_) {
    new_state = std::visit(
        [&scheduled](auto& state) -> std::optional<State> {
          return state.update(scheduled.phase);
        },
        *active_state_);
  } else {
    new_state = create_state(scheduled.phase);
  }

  if (new_state && new_state != active_state_) {
    active_state_ = std::move(new_state);
    send_phase_transition(tick.tz_tick_time);
  }
}

auto TradingPhaseController::advance_active_auction(const event::Tick& tick)
    -> void {
  const auto now = core::as_local_time(tick.tz_tick_time);
  const auto& auction = std::get<AuctionState>(*active_state_);
  if (!auction.uncrossing_due(now)) {
    return;
  }

  const auto uncrossing = auction.begin_uncrossing();

  active_state_ = uncrossing;
  send_phase_transition(tick.tz_tick_time);

  // However, the uncrossing duration depends on the processing time (handled in
  // MatchingEngines); this method immediately sends the next phase
  // event::PhaseTransition. It may cause an issue: if the next scheduled phase
  // ends before the actual uncrossing processing completes, the simulator will
  // activate it (already passed phase) for a short period.
  const auto resume_at = std::max(auction.end_time(), now);
  const auto next = scheduler_.phase_at(resume_at);
  auto next_state = make_state(next, tick);
  if (!next_state) {
    log::debug(
        "the '{}' phase scheduled after the auction has no dedicated state "
        "yet, reporting Open",
        next.phase.phase());
    next_state = create_state(fallback_open_phase());
  }

  active_state_ = std::move(next_state);
  send_phase_transition(tick.tz_tick_time);
}

auto TradingPhaseController::make_state(const ScheduledPhase& scheduled,
                                        const event::Tick& tick)
    -> std::optional<State> {
  if (is_auction_phase(scheduled.phase.phase())) {
    return activate_auction(scheduled, tick);
  }
  return create_state(scheduled.phase);
}

auto TradingPhaseController::activate_auction(const ScheduledPhase& scheduled,
                                              const event::Tick& tick)
    -> std::optional<State> {
  if (!scheduled.auction) {
    return std::nullopt;
  }
  const auto& timing = *scheduled.auction;

  if (const auto* open =
          active_state_ ? std::get_if<OpenState>(&*active_state_) : nullptr;
      open != nullptr && open->halted_by_request()) {
    log::info(
        "overriding an active request-initiated halt to start the '{}' auction "
        "phase",
        scheduled.phase.phase());
  }

  const auto now = core::as_local_time(tick.tz_tick_time);
  const core::local_us end =
      std::chrono::floor<std::chrono::days>(now) + timing.end;

  const core::local_us uncross_at =
      pick_uncross_time(end, timing.end_range, now);

  log::debug("'{}' auction scheduled to uncross at {} (configured end {})",
             scheduled.phase.phase(),
             uncross_at,
             end);

  return create_state(scheduled.phase,
                      AuctionActivation{.end = end, .uncross_at = uncross_at});
}

auto TradingPhaseController::pick_uncross_time(core::local_us end,
                                               std::chrono::seconds range,
                                               core::local_us earliest)
    -> core::local_us {
  if (!uncross_delay_generator_) {
    uncross_delay_generator_ = make_default_uncross_delay_generator();
  }
  const std::chrono::seconds lowest_offset =
      std::max(-range, std::chrono::ceil<std::chrono::seconds>(earliest - end));
  return end + uncross_delay_generator_(lowest_offset, range);
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
