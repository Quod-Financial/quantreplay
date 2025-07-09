#include <gtest/gtest.h>

#include "common/events.hpp"

namespace simulator::trading_system::event::test {
namespace {

TEST(TradingSystemTick, FormatsToString) {
  using namespace std::chrono_literals;

  constexpr auto sys_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                         30min + 59s + 123456us};
  const Tick tick{
      .sys_tick_time = sys_time,
      .tz_tick_time = core::as_tz_time(sys_time, core::TzClock{"Europe/Kyiv"}),
      .is_new_sys_day = true,
      .is_new_tz_day = false};

  ASSERT_EQ(fmt::to_string(tick),
            R"({ "SystemTick": { )"
            R"("sys_tick_time": "2025-12-30 13:30:59.123456", )"
            R"("tz_tick_time": "2025-12-30 15:30:59.123456", )"
            R"("is_new_sys_day": true, )"
            R"("is_new_tz_day": false } })");
}

TEST(TradingSystemPhaseTransition, FormatsToString) {
  using namespace std::chrono_literals;

  constexpr auto sys_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                         30min + 59s + 123456us};
  const PhaseTransition phase_transition{
      .tz_time_point = core::as_tz_time(sys_time, core::TzClock{"Europe/Kyiv"}),
      .phase = Phase{TradingPhase::Option::Open,
                     TradingStatus::Option::Halt,
                     Phase::Settings{.allow_cancels = true}}};

  ASSERT_EQ(fmt::to_string(phase_transition),
            R"({ "PhaseTransition": { )"
            R"("phase": { "Phase": { "phase": "Open", "status": "Halt", )"
            R"("settings": { "Settings": { "allow_cancels": true } } } }, )"
            R"("tz_tick_time": "2025-12-30 15:30:59.123456" } })");
}

}  // namespace
}  // namespace simulator::trading_system::event::test
