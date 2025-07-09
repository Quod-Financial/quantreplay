#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_IES_CONTROLLER_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_IES_CONTROLLER_HPP_

#include <functional>
#include <memory>

#include "common/events.hpp"
#include "ies/phase_schedule.hpp"
#include "protocol/admin/trading_phase.hpp"
#include "runtime/loop.hpp"

namespace simulator::trading_system::ies {

class Controller {
  class Implementation;

 public:
  template <typename EventType>
  using Handler = std::function<void(const EventType&)>;

  explicit Controller(runtime::Loop& loop);

  Controller() = delete;
  Controller(const Controller&) = delete;
  Controller(Controller&&) noexcept;
  ~Controller() noexcept;

  auto operator=(const Controller&) -> Controller& = delete;
  auto operator=(Controller&&) noexcept -> Controller&;

  auto set_tz_clock(const core::TzClock& tz_clock) -> void;

  auto schedule_phases(const PhaseSchedule& schedule) -> void;

  template <typename EventType>
  auto bind(Handler<EventType> handler) -> Controller& {
    register_event_handler(std::move(handler));
    return *this;
  }

  auto process(protocol::HaltPhaseRequest request,
               protocol::HaltPhaseReply& reply) -> void;

  auto process(protocol::ResumePhaseRequest request,
               protocol::ResumePhaseReply& reply) -> void;

 private:
  auto register_event_handler(Handler<event::Tick> handler) -> void;

  auto register_event_handler(Handler<event::PhaseTransition> handler) -> void;

  std::unique_ptr<Implementation> impl_;
};

}  // namespace simulator::trading_system::ies

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_IES_CONTROLLER_HPP_
