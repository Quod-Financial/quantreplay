#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_EVENTS_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_EVENTS_HPP_

#include <fmt/base.h>

#include "common/phase.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"

namespace simulator::trading_system::event {

struct Tick {
  core::sys_us sys_tick_time;
  core::tz_us tz_tick_time;
  bool is_new_sys_day = false;
  bool is_new_tz_day = false;

  bool operator==(const Tick& rhs) const = default;
};

struct PhaseTransition {
  core::tz_us tz_time_point;
  Phase phase;

  bool operator==(const PhaseTransition& rhs) const = default;
};

}  // namespace simulator::trading_system::event

template <>
struct fmt::formatter<simulator::trading_system::event::Tick> {
  using formattable = simulator::trading_system::event::Tick;

  constexpr static auto parse(auto& ctx) { return ctx.begin(); }

  auto format(const formattable& event, auto& ctx) const {
    return fmt::format_to(ctx.out(),
                          // clang-format off
                          "{{ "
                            "\"SystemTick\": {{ "
                              "\"sys_tick_time\": \"{}\", "
                              "\"tz_tick_time\": \"{}\", "
                              "\"is_new_sys_day\": {}, "
                              "\"is_new_tz_day\": {} "
                            "}} "
                          "}}",
                          // clang-format on
                          event.sys_tick_time,
                          event.tz_tick_time,
                          event.is_new_sys_day,
                          event.is_new_tz_day);
  }
};

template <>
struct fmt::formatter<simulator::trading_system::event::PhaseTransition> {
  using formattable = simulator::trading_system::event::PhaseTransition;

  constexpr static auto parse(auto& ctx) { return ctx.begin(); }

  auto format(const formattable& event, auto& ctx) const {
    return fmt::format_to(ctx.out(),
                          // clang-format off
                          "{{ "
                            "\"PhaseTransition\": {{ "
                              "\"phase\": {}, "
                              "\"tz_tick_time\": \"{}\" "
                            "}} "
                          "}}",
                          // clang-format on
                          event.phase,
                          event.tz_time_point);
  }
};

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_EVENTS_HPP_
