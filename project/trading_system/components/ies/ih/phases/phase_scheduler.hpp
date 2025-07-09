#ifndef SIMULATOR_IES_IH_PHASES_PHASE_SCHEDULER_HPP_
#define SIMULATOR_IES_IH_PHASES_PHASE_SCHEDULER_HPP_

#include <optional>
#include <utility>

#include "common/events.hpp"
#include "common/phase.hpp"
#include "ies/phase_schedule.hpp"

namespace simulator::trading_system::ies {

class PhaseScheduler {
 public:
  auto configure(PhaseSchedule schedule) -> void {
    std::swap(schedule_, schedule);
  }

  auto update(const event::Tick& tick) -> std::optional<Phase> {
    std::optional<Phase> new_phase;
    const auto scheduled = schedule_at(core::as_local_time(tick.tz_tick_time));
    if (!current_phase_ || scheduled != *current_phase_) {
      new_phase = scheduled;
      current_phase_ = scheduled;
    }
    return new_phase;
  }

 private:
  auto schedule_at(core::local_us local_time) const -> Phase {
    return schedule_.get_scheduled_phase(local_time);
  }

  PhaseSchedule schedule_;
  std::optional<Phase> current_phase_;
};

}  // namespace simulator::trading_system::ies

#endif  // SIMULATOR_IES_IH_PHASES_PHASE_SCHEDULER_HPP_
