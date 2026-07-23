#ifndef SIMULATOR_IES_IH_TRADING_PHASE_CONTROLLER_HPP_
#define SIMULATOR_IES_IH_TRADING_PHASE_CONTROLLER_HPP_

#include <chrono>
#include <functional>
#include <mutex>
#include <optional>

#include "common/events.hpp"
#include "core/tools/time.hpp"
#include "ies/phase_schedule.hpp"
#include "ih/phases/phase_scheduler.hpp"
#include "ih/phases/states.hpp"
#include "protocol/admin/trading_phase.hpp"

namespace simulator::trading_system::ies {

class TradingPhaseController {
 public:
  using UncrossDelayGenerator = std::function<std::chrono::seconds(
      std::chrono::seconds low, std::chrono::seconds high)>;

  // Setup-only; not synchronised with the running update()/process().
  auto set_tz_clock(const core::TzClock& tz_clock) -> void;

  auto configure(const PhaseSchedule& schedule) -> void;

  auto bind(std::function<void(event::PhaseTransition)> handler) -> void;

  auto set_uncross_delay_generator(UncrossDelayGenerator generator) -> void;

  auto update(const event::Tick& tick) -> void;

  auto process(protocol::HaltPhaseRequest request,
               protocol::HaltPhaseReply& reply) -> void;

  auto process(protocol::ResumePhaseRequest request,
               protocol::ResumePhaseReply& reply) -> void;

 private:
  auto enter_scheduled_phase(const ScheduledPhase& scheduled,
                             const event::Tick& tick) -> void;

  auto advance_active_auction(const event::Tick& tick) -> void;

  auto make_state(const ScheduledPhase& scheduled,
                  const event::Tick& tick) -> std::optional<State>;

  auto activate_auction(const ScheduledPhase& scheduled,
                        const event::Tick& tick) -> std::optional<State>;

  auto pick_uncross_time(core::local_us end,
                         std::chrono::seconds range,
                         core::local_us earliest) -> core::local_us;

  auto create_tz_time_point() const -> core::tz_us;

  auto send_phase_transition(const core::tz_us& tz_time_point) -> void;

  auto transition_to(event::PhaseTransition event) -> void;

  core::TzClock tz_clock_;
  PhaseScheduler scheduler_;
  std::function<void(event::PhaseTransition)> handler_;
  UncrossDelayGenerator uncross_delay_generator_;

  std::mutex mutex_;
  std::optional<State> active_state_;
};

}  // namespace simulator::trading_system::ies

#endif  // SIMULATOR_IES_IH_TRADING_PHASE_CONTROLLER_HPP_
