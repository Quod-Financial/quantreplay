#include <gmock/gmock.h>

#include "common/events.hpp"
#include "core/tools/time.hpp"
#include "ih/common/data/market_data_updates.hpp"
#include "ih/orders/actions/time_reporter.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

struct MatchingEngineTimeReporter : public Test {
  inline static const core::TzClock Timezone{"Europe/Kyiv"};

  static auto default_tick() -> event::Tick {
    return event::Tick{.sys_tick_time = core::get_current_system_time(),
                       .tz_tick_time = core::get_current_tz_time(Timezone),
                       .is_new_sys_day = false,
                       .is_new_tz_day = false};
  }

  static auto tick_on_new_tz_day() -> event::Tick {
    auto tick = default_tick();
    tick.is_new_tz_day = true;
    return tick;
  }

  NiceMock<EventListenerMock> event_listener;
};

TEST_F(MatchingEngineTimeReporter, ReportsTzDayPassedWhenTimezoneDayChanges) {
  const auto tick = tick_on_new_tz_day();
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<TzDayPassed>(Field(
                  &TzDayPassed::sys_tick_time, Eq(tick.sys_tick_time))))));

  TimeReporter{event_listener}(tick);
}

TEST_F(MatchingEngineTimeReporter,
       DoesNotReportTzDayPassedWhenTimezoneDayIsNotChanged) {
  EXPECT_CALL(event_listener, on(_)).Times(0);

  TimeReporter{event_listener}(default_tick());
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
