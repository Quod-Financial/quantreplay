#include "ih/tick_event_factory.hpp"

#include <gtest/gtest.h>

#include <chrono>

#include "core/tools/time.hpp"

using namespace std::chrono_literals;

namespace simulator::trading_system::ies {
namespace {

struct TickEventFactoryTest : public ::testing::Test {
  void SetUp() override { factory.set_tz_clock(clock); }

  // 2024-09-19 03:00:00 UNIX, 2024-09-18 23:00:00 America/New_York
  core::sys_us tick_time{1726714800s};
  core::TzClock clock{"America/New_York"};
  TickEventFactory factory;
};

TEST_F(TickEventFactoryTest, CreatesNewTickEventWithGivenSystemTime) {
  const auto tick = factory.create_tick_event(tick_time);

  ASSERT_EQ(tick.sys_tick_time, tick_time);
}

TEST_F(TickEventFactoryTest, CreatesNewTickEventWithGivenSystemTimeInTz) {
  const auto tick = factory.create_tick_event(tick_time);

  ASSERT_EQ(tick.tz_tick_time, core::as_tz_time(tick_time, clock));
}

TEST_F(TickEventFactoryTest, DoesNotSetNewDayFlagsWhenNoPreviousDate) {
  const auto tick = factory.create_tick_event(tick_time);

  EXPECT_FALSE(tick.is_new_sys_day);
  EXPECT_FALSE(tick.is_new_tz_day);
}

TEST_F(TickEventFactoryTest, DoesNotSetNewSysDayFlagWhenSysDateNotChanged) {
  (void)factory.create_tick_event(tick_time);

  const auto tick = factory.create_tick_event(tick_time + 20h);

  ASSERT_FALSE(tick.is_new_sys_day);
}

TEST_F(TickEventFactoryTest, SetsNewSysDayFlagWhenSysDateNotChanged) {
  (void)factory.create_tick_event(tick_time);

  const auto tick = factory.create_tick_event(tick_time + 21h);

  ASSERT_TRUE(tick.is_new_sys_day);
}

TEST_F(TickEventFactoryTest, DoesNotSetNewTzDayFlagWhenTzDateNotChanged) {
  (void)factory.create_tick_event(tick_time);

  const auto tick = factory.create_tick_event(tick_time + 59min);

  ASSERT_FALSE(tick.is_new_tz_day);
}

TEST_F(TickEventFactoryTest, SetsNewTzDayFlagWhenTzDateNotChanged) {
  (void)factory.create_tick_event(tick_time);

  const auto tick = factory.create_tick_event(tick_time + 1h);

  ASSERT_TRUE(tick.is_new_tz_day);
}

}  // namespace
}  // namespace simulator::trading_system::ies
