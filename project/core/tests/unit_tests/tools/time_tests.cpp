#include <fmt/format.h>
#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>

#include "core/tools/time.hpp"

using namespace std::chrono_literals;

namespace simulator::core {
namespace {

TEST(TimeTest, TzClockCantBeCreatedForBadTimezone) {
  ASSERT_THROW(TzClock{"bad/timezone"}, std::runtime_error);
}

TEST(TimeTest, TzClockCreatedWithValidTimezone) {
  ASSERT_NO_THROW(TzClock{"Europe/Warsaw"});
}

TEST(TimeTest, TzClockCreatedWithDefaultUtcTimezone) {
  ASSERT_NO_THROW(TzClock{});
}

TEST(TimeTest, TzClocksWithDefaultUtcTimezonesAreEqual) {
  const TzClock first;
  const TzClock second;
  ASSERT_EQ(first, second);
}

TEST(TimeTest, TzClockWithSameTimezoneAreEqual) {
  const TzClock first("Europe/Warsaw");
  const TzClock second("Europe/Warsaw");
  ASSERT_EQ(first, second);
}

TEST(TimeTest, TzClockWithDifferentTimezonesAreNotEqual) {
  const TzClock first("Europe/Warsaw");
  const TzClock second("America/New_York");
  ASSERT_NE(first, second);
}

TEST(TimeTest, TzClockCorrectlyConvertsSystemTimeToTz) {
  // 2024-09-18 15:32:55 GMT (UNIX time)
  constexpr auto sys_time = std::chrono::system_clock::time_point(1726673575s);
  const auto clock = TzClock("Europe/Warsaw");

  const auto tz_time = core::as_tz_time(sys_time, clock);

  // 2024-09-18 17:32:55 CEST (in Warsaw)
  ASSERT_EQ(tz_time, TzClock::time_point(1726680775s));
}

TEST(TimeTest, TzClockCorrectlyConvertsTzTimeToSystem) {
  // 2024-09-18 17:32:55 CEST (in Warsaw)
  const auto ztime = core::tz_time(1726680775s);
  const auto clock = TzClock("Europe/Warsaw");

  const auto sys_time = core::as_sys_time(ztime, clock);

  // 2024-09-18 15:32:55 GMT (UNIX time)
  ASSERT_EQ(sys_time, std::chrono::system_clock::time_point(1726673575s));
}

TEST(TimeTest, TzClockFailsToConvertNonexistentTzTime) {
  // Daylight saving starts on 2024-3-10 02:00:00 (America/New_York),
  // clocks are moved forward 1 hour to 2024-3-10 03:00:00,
  // 02:00 - 03:00: the hour does not exist
  const auto clock = TzClock("America/New_York");
  const auto bad_time = core::local_days(2024y / 3 / 10) + 2h + 30min;

  ASSERT_THROW(
      (void)core::as_sys_time(tz_time(bad_time.time_since_epoch()), clock),
      std::runtime_error);
}

TEST(TimeTest, TzClockFailsToConvertAmbiguousTzTime) {
  // Daylight saving ends on 2024-11-03 02:00:00 (America/New_York),
  // clocks go back 1 hour, so 02:00:00 becomes 01:00:00,
  // 01:00 - 02:00: the hour happens twice
  const auto clock = TzClock("America/New_York");
  const auto bad_time = core::local_days(2024y / 11 / 3) + 1h + 30min;

  ASSERT_THROW(
      (void)core::as_sys_time(tz_time(bad_time.time_since_epoch()), clock),
      std::runtime_error);
}

TEST(TimeTest, TzTimeCorrectlyFormatted) {
  // 2024-09-18 15:32:55 GMT (UNIX time)
  // 2024-09-18 17:32:55 CEST (in Warsaw)
  constexpr auto sys = std::chrono::system_clock::time_point(1726673575s);

  const TzClock clock("Europe/Warsaw");

  ASSERT_EQ(fmt::to_string(TzClock::to_timezone(sys, clock)),
            "2024-09-18 17:32:55.000000000");
}

}  // namespace
}  // namespace simulator::core