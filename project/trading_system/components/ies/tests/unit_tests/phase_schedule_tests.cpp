#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>

#include "core/tools/time.hpp"
#include "ies/phase_record.hpp"
#include "ies/phase_schedule.hpp"

namespace simulator::trading_system::ies::test {
namespace {

using namespace ::testing;  // NOLINT
using namespace std::chrono_literals;

struct PhaseScheduleTest : public Test {
  static auto daytime(std::chrono::seconds time_of_day) {
    // NOLINTNEXTLINE (*magic-numbers*)
    return core::local_days(2024y / 9 / 1) + time_of_day;
  }

  PhaseSchedule schedule;
};

TEST_F(PhaseScheduleTest, ReturnsPhaseRecords) {
  schedule = {{.begin = 12h,
               .end = 13h,
               .phase = TradingPhase::Option::OpeningAuction}};

  ASSERT_THAT(
      schedule.phase_records(),
      ElementsAre(PhaseRecord{.begin = 12h,
                              .end = 13h,
                              .phase = TradingPhase::Option::OpeningAuction}));
}

TEST_F(PhaseScheduleTest, HasPhaseReturnsFalseWhenPhaseIsNotScheduled) {
  schedule = {{.begin = 12h,
               .end = 13h,
               .phase = TradingPhase::Option::OpeningAuction}};

  ASSERT_FALSE(schedule.has_phase(TradingPhase::Option::ClosingAuction));
}

TEST_F(PhaseScheduleTest, HasPhaseReturnsTrueWhenPhaseIsScheduled) {
  schedule = {{.begin = 12h,
               .end = 13h,
               .phase = TradingPhase::Option::ClosingAuction}};

  ASSERT_TRUE(schedule.has_phase(TradingPhase::Option::ClosingAuction));
}

TEST_F(PhaseScheduleTest, ReturnsOpenPhaseResumeStatusWhenNoPhasesAdded) {
  ASSERT_EQ(
      schedule.get_scheduled_phase(daytime(12h)).phase,
      Phase(TradingPhase::Option::Open, TradingStatus::Option::Resume, {}));
}

TEST_F(PhaseScheduleTest, ReturnsPhaseAtBeginTime) {
  schedule = {{.begin = 12h,
               .end = 13h,
               .phase = TradingPhase::Option::OpeningAuction}};

  ASSERT_EQ(schedule.get_scheduled_phase(daytime(12h)).phase,
            Phase(TradingPhase::Option::OpeningAuction,
                  TradingStatus::Option::Resume,
                  {}));
}

TEST_F(PhaseScheduleTest, ReturnsHaltStatusOnClosedPhase) {
  schedule = {
      {.begin = 12h, .end = 13h, .phase = TradingPhase::Option::Closed}};

  ASSERT_THAT(schedule.get_scheduled_phase(daytime(12h + 30min)).phase,
              Property(&Phase::status, Eq(TradingStatus::Option::Halt)));
}

TEST_F(PhaseScheduleTest, ReturnsPhaseInBetweenOfBeginEndTimes) {
  schedule = {{.begin = 12h,
               .end = 13h,
               .phase = TradingPhase::Option::OpeningAuction}};

  ASSERT_EQ(schedule.get_scheduled_phase(daytime(12h + 30min)).phase,
            Phase(TradingPhase::Option::OpeningAuction,
                  TradingStatus::Option::Resume,
                  {}));
}

TEST_F(PhaseScheduleTest, ReturnsOpenPhaseAtEndTime) {
  schedule = {{.begin = 12h,
               .end = 13h,
               .phase = TradingPhase::Option::OpeningAuction}};

  ASSERT_EQ(
      schedule.get_scheduled_phase(daytime(13h)).phase,
      Phase(TradingPhase::Option::Open, TradingStatus::Option::Resume, {}));
}

TEST_F(PhaseScheduleTest, ReturnsOpenPhaseAfterEndTime) {
  schedule = {
      {.begin = 12h, .end = 13h, .phase = TradingPhase::Option::Closed}};

  ASSERT_EQ(
      schedule.get_scheduled_phase(daytime(13h + 1min)).phase,
      Phase(TradingPhase::Option::Open, TradingStatus::Option::Resume, {}));
}

TEST_F(PhaseScheduleTest, ReturnsPhaseWithLatestBeginTime) {
  schedule = {{.begin = 10h, .end = 15h, .phase = TradingPhase::Option::Closed},
              {.begin = 11h, .end = 14h, .phase = TradingPhase::Option::Open},
              {.begin = 12h,
               .end = 13h,
               .phase = TradingPhase::Option::OpeningAuction}};

  ASSERT_EQ(schedule.get_scheduled_phase(daytime(12h + 30min)).phase,
            Phase(TradingPhase::Option::OpeningAuction,
                  TradingStatus::Option::Resume,
                  {}));
}

TEST_F(PhaseScheduleTest, ReturnsPhaseWithLatestBeginTimeAndEarlierEndTime) {
  schedule = {
      {.begin = 10h, .end = 15h, .phase = TradingPhase::Option::Closed},
      {.begin = 11h, .end = 14h, .phase = TradingPhase::Option::PostTrading},
      {.begin = 12h,
       .end = 13h,
       .phase = TradingPhase::Option::OpeningAuction}};

  ASSERT_EQ(schedule.get_scheduled_phase(daytime(13h + 30min)).phase,
            Phase(TradingPhase::Option::PostTrading,
                  TradingStatus::Option::Resume,
                  {}));
}

TEST_F(PhaseScheduleTest, ReturnsHaltStatusWithSettingAtTheHaltBeginTime) {
  schedule = {{.begin = 11h, .end = 14h, .phase = TradingPhase::Option::Open},
              {.begin = 12h, .end = 13h, .phase = TradingStatus::Option::Halt}};

  const auto scheduled = schedule.get_scheduled_phase(daytime(12h));
  ASSERT_EQ(scheduled.phase.phase(), TradingPhase::Option::Open);
  ASSERT_EQ(scheduled.phase.status(), TradingStatus::Option::Halt);
}

TEST_F(PhaseScheduleTest, IgnoresPhaseSettingOnNonHaltPhase) {
  schedule = {{.begin = 11h,
               .end = 14h,
               .phase = TradingPhase::Option::Open,
               .allow_cancels_on_halt = true}};

  const auto scheduled = schedule.get_scheduled_phase(daytime(11h));
  ASSERT_EQ(scheduled.phase.settings(), std::nullopt);
}

TEST_F(PhaseScheduleTest, CopiesPhaseSettingOnHaltPhase) {
  schedule = {{.begin = 11h, .end = 14h, .phase = TradingPhase::Option::Open},
              {.begin = 12h,
               .end = 13h,
               .phase = TradingStatus::Option::Halt,
               .allow_cancels_on_halt = true}};

  const auto scheduled = schedule.get_scheduled_phase(daytime(12h));
  ASSERT_THAT(scheduled.phase.settings(),
              Optional(Field(&Phase::Settings::allow_cancels, Eq(true))));
}

TEST_F(PhaseScheduleTest, CopiesPhaseSettingFromCurrentHaltPhase) {
  schedule = {{.begin = 11h, .end = 14h, .phase = TradingPhase::Option::Open},
              {.begin = 11h,
               .end = 14h,
               .phase = TradingStatus::Option::Halt,
               .allow_cancels_on_halt = true},
              {.begin = 12h,
               .end = 13h,
               .phase = TradingStatus::Option::Halt,
               .allow_cancels_on_halt = false}};

  const auto scheduled = schedule.get_scheduled_phase(daytime(12h));
  ASSERT_THAT(scheduled.phase.settings(),
              Optional(Field(&Phase::Settings::allow_cancels, Eq(false))));
}

TEST_F(PhaseScheduleTest, SurfacesConfiguredEndAndRangeForAuctionPhase) {
  schedule = {{.begin = 12h,
               .end = 13h,
               .end_range = 5min,
               .phase = TradingPhase::Option::OpeningAuction}};

  const auto scheduled = schedule.get_scheduled_phase(daytime(12h + 30min));

  ASSERT_TRUE(scheduled.auction.has_value());
  EXPECT_EQ(scheduled.auction->end, 13h);
  EXPECT_EQ(scheduled.auction->end_range, 5min);
}

TEST_F(PhaseScheduleTest, LeavesAuctionTimingUnsetForDefaultOpenPhase) {
  const auto scheduled = schedule.get_scheduled_phase(daytime(12h));

  EXPECT_EQ(
      scheduled.phase,
      Phase(TradingPhase::Option::Open, TradingStatus::Option::Resume, {}));
  EXPECT_FALSE(scheduled.auction.has_value());
}

}  // namespace
}  // namespace simulator::trading_system::ies::test
