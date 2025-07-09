#ifndef SIMULATOR_IES_IH_CONTROLLER_IMPL_HPP_
#define SIMULATOR_IES_IH_CONTROLLER_IMPL_HPP_

#include "common/events.hpp"
#include "core/tools/time.hpp"
#include "ies/controller.hpp"
#include "ih/system_tick_controller.hpp"
#include "ih/tick_event_factory.hpp"
#include "ih/trading_phase_controller.hpp"
#include "runtime/loop.hpp"

namespace simulator::trading_system::ies {

class Controller::Implementation {
 public:
  explicit Implementation(runtime::Loop& loop);

  Implementation() = delete;
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) noexcept = delete;
  ~Implementation() noexcept = default;

  auto operator=(const Implementation&) -> Implementation& = delete;
  auto operator=(Implementation&&) noexcept -> Implementation& = delete;

  auto set_tz_clock(const core::TzClock& tz_clock) -> void;

  auto schedule_phases(const PhaseSchedule& schedule) -> void;

  auto bind(Handler<event::Tick> handler) -> void;

  auto bind(Handler<event::PhaseTransition> handler) -> void;

  auto process(protocol::HaltPhaseRequest request,
               protocol::HaltPhaseReply& reply) -> void;

  auto process(protocol::ResumePhaseRequest request,
               protocol::ResumePhaseReply& reply) -> void;

 private:
  auto tick() -> void;

  TradingPhaseController trading_phase_controller_;
  SystemTickController system_tick_controller_;
  TickEventFactory tick_factory_;
};

}  // namespace simulator::trading_system::ies

#endif  // SIMULATOR_IES_IH_CONTROLLER_HPP_
