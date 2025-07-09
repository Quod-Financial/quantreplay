#include "ies/controller.hpp"

#include <memory>

#include "common/events.hpp"
#include "ih/controller_impl.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::ies {

Controller::Implementation::Implementation(runtime::Loop& loop) {
  loop.add([this] { tick(); });
}

auto Controller::Implementation::set_tz_clock(const core::TzClock& tz_clock)
    -> void {
  tick_factory_.set_tz_clock(tz_clock);
  trading_phase_controller_.set_tz_clock(tz_clock);
}

auto Controller::Implementation::schedule_phases(const PhaseSchedule& schedule)
    -> void {
  trading_phase_controller_.configure(schedule);
}

auto Controller::Implementation::bind(Handler<event::Tick> handler) -> void {
  system_tick_controller_.bind(std::move(handler));
}

auto Controller::Implementation::bind(Handler<event::PhaseTransition> handler)
    -> void {
  trading_phase_controller_.bind(std::move(handler));
}

auto Controller::Implementation::process(protocol::HaltPhaseRequest request,
                                         protocol::HaltPhaseReply& reply)
    -> void {
  trading_phase_controller_.process(request, reply);
}

auto Controller::Implementation::process(protocol::ResumePhaseRequest request,
                                         protocol::ResumePhaseReply& reply)
    -> void {
  trading_phase_controller_.process(request, reply);
}

auto Controller::Implementation::tick() -> void {
  const event::Tick event = tick_factory_.create_tick_event();

  log::trace("ies controller tick action started: {}", event);
  trading_phase_controller_.update(event);
  system_tick_controller_.update(event);
  log::trace("ies controller tick action completed: {}", event);
}

Controller::Controller(runtime::Loop& loop)
    : impl_{std::make_unique<Implementation>(loop)} {}

Controller::Controller(Controller&&) noexcept = default;

Controller::~Controller() noexcept = default;

auto Controller::operator=(Controller&&) noexcept -> Controller& = default;

auto Controller::set_tz_clock(const core::TzClock& tz_clock) -> void {
  impl_->set_tz_clock(tz_clock);
}

auto Controller::schedule_phases(const PhaseSchedule& schedule) -> void {
  impl_->schedule_phases(schedule);
}

auto Controller::process(protocol::HaltPhaseRequest request,
                         protocol::HaltPhaseReply& reply) -> void {
  impl_->process(request, reply);
}

auto Controller::process(protocol::ResumePhaseRequest request,
                         protocol::ResumePhaseReply& reply) -> void {
  impl_->process(request, reply);
}

auto Controller::register_event_handler(Handler<event::Tick> handler) -> void {
  impl_->bind(std::move(handler));
}

auto Controller::register_event_handler(Handler<event::PhaseTransition> handler)
    -> void {
  impl_->bind(std::move(handler));
}

}  // namespace simulator::trading_system::ies