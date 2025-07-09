#ifndef SIMULATOR_IES_IH_SYSTEM_TICK_EVENT_FACTORY_HPP_
#define SIMULATOR_IES_IH_SYSTEM_TICK_EVENT_FACTORY_HPP_

#include <optional>

#include "common/events.hpp"
#include "core/tools/time.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::ies {

class TickEventFactory {
 public:
  auto set_tz_clock(const core::TzClock& tz_clock) -> void {
    tz_clock_ = tz_clock;
  }

  auto create_tick_event() -> event::Tick {
    return create_tick_event(core::get_current_system_time());
  }

  auto create_tick_event(core::sys_us tick_time) -> event::Tick {
    log::trace("creating new tick event for {} system tick time", tick_time);

    event::Tick event{.sys_tick_time = tick_time,
                      .tz_tick_time = core::as_tz_time(tick_time, tz_clock_),
                      .is_new_sys_day = false,
                      .is_new_tz_day = false};

    if (last_sys_date_.has_value()) {
      event.is_new_sys_day = last_sys_date_ < core::to_date(tick_time);
    }
    if (last_tz_date_.has_value()) {
      event.is_new_tz_day = last_tz_date_ < core::to_date(event.tz_tick_time);
    }

    last_sys_date_ = core::to_date(tick_time);
    last_tz_date_ = core::to_date(event.tz_tick_time);

    log::trace("created new tick event: {}", event);
    return event;
  }

 private:
  core::TzClock tz_clock_;
  std::optional<core::sys_days> last_sys_date_;
  std::optional<core::tz_days> last_tz_date_;
};

}  // namespace simulator::trading_system::ies

#endif  // SIMULATOR_IES_IH_SYSTEM_TICK_EVENT_FACTORY_HPP_
