#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>

#include "common/events.hpp"
#include "core/tools/time.hpp"
#include "ih/phases/phase_scheduler.hpp"

namespace simulator::trading_system::ies::test {
namespace {

using namespace ::testing;  // NOLINT
using namespace std::chrono_literals;

struct PhaseSchedulerTest : public ::testing::Test {
  constexpr static event::Tick DefaultTick{};

  constexpr static auto make_tick(auto time_of_day) {
    const auto tod = std::chrono::floor<std::chrono::microseconds>(time_of_day);
    return event::Tick{
        .sys_tick_time = core::sys_us(tod),
        .tz_tick_time = core::tz_time(tod),
        .is_new_sys_day = false,
        .is_new_tz_day = false,
    };
  }

  static auto local_at(auto time_of_day) -> core::local_us {
    return core::as_local_time(make_tick(time_of_day).tz_tick_time);
  }

  static auto TradingPhaseIs(TradingPhase trading_phase) {
    return Optional(Field(&ScheduledPhase::phase,
                          Property(&Phase::phase, Eq(trading_phase))));
  }

  PhaseScheduler scheduler;
};

TEST_F(PhaseSchedulerTest, ReportsNothingWhenDefaultPhaseKeptActive) {
  (void)scheduler.update(DefaultTick);  // schedule default phase

  ASSERT_THAT(scheduler.update(DefaultTick), Eq(std::nullopt));
}

TEST_F(PhaseSchedulerTest, SchedulesConfiguredPhase) {
  scheduler.configure({
      PhaseRecord{
          .begin = 12h, .end = 14h, .phase = TradingPhase::Option::Closed},
  });

  ASSERT_THAT(scheduler.update(make_tick(13h)),
              TradingPhaseIs(TradingPhase::Option::Closed));
}

TEST_F(PhaseSchedulerTest, ReportsNothingWhenScheduledPhaseKeptActive) {
  scheduler.configure({
      PhaseRecord{
          .begin = 12h, .end = 14h, .phase = TradingPhase::Option::Closed},
  });

  (void)scheduler.update(make_tick(13h));  // schedule closed phase

  ASSERT_THAT(scheduler.update(make_tick(13h + 30min)), Eq(std::nullopt));
}

TEST_F(PhaseSchedulerTest, SchedulesDefaultPhaseWhenNoPhasesConfiguredAfter) {
  scheduler.configure({
      PhaseRecord{
          .begin = 12h, .end = 14h, .phase = TradingPhase::Option::Closed},
  });

  (void)scheduler.update(make_tick(13h));  // schedule closed phase
  ASSERT_THAT(scheduler.update(make_tick(14h)),
              TradingPhaseIs(TradingPhase::Option::Open));
}

TEST_F(PhaseSchedulerTest, SchedulesNextPhaseAtStartTime) {
  scheduler.configure({
      PhaseRecord{.begin = 8h, .end = 17h, .phase = TradingPhase::Option::Open},
      PhaseRecord{.begin = 9h,
                  .end = 10h,
                  .phase = TradingPhase::Option::OpeningAuction},
  });

  EXPECT_THAT(scheduler.update(make_tick(8h)),
              TradingPhaseIs(TradingPhase::Option::Open));
  EXPECT_THAT(scheduler.update(make_tick(9h)),
              TradingPhaseIs(TradingPhase::Option::OpeningAuction));
}

TEST_F(PhaseSchedulerTest, DoesNotReportTransitionWhenSamePhaseRescheduled) {
  scheduler.configure({
      PhaseRecord{.begin = 8h, .end = 17h, .phase = TradingPhase::Option::Open},
      PhaseRecord{.begin = 9h, .end = 10h, .phase = TradingPhase::Option::Open},
  });

  EXPECT_THAT(scheduler.update(make_tick(8h)),
              TradingPhaseIs(TradingPhase::Option::Open));
  EXPECT_THAT(scheduler.update(make_tick(9h)), Eq(std::nullopt));
}

TEST_F(PhaseSchedulerTest, SurfacesConfiguredEndWindowForScheduledAuction) {
  scheduler.configure({
      PhaseRecord{.begin = 9h,
                  .end = 10h,
                  .end_range = 5min,
                  .phase = TradingPhase::Option::OpeningAuction},
  });

  EXPECT_THAT(
      scheduler.update(make_tick(9h)),
      Optional(
          Field(&ScheduledPhase::auction,
                Optional(AllOf(Field(&AuctionTiming::end, Eq(10h)),
                               Field(&AuctionTiming::end_range, Eq(5min)))))));
}

TEST_F(PhaseSchedulerTest, LeavesAuctionWindowUnsetForNonAuctionPhase) {
  scheduler.configure({
      PhaseRecord{
          .begin = 12h, .end = 14h, .phase = TradingPhase::Option::Closed},
  });

  EXPECT_THAT(scheduler.update(make_tick(13h)),
              Optional(Field(&ScheduledPhase::auction, Eq(std::nullopt))));
}

TEST_F(PhaseSchedulerTest,
       PhaseAtReturnsScheduledPhaseWithEndWindowForScheduledAuction) {
  scheduler.configure({
      PhaseRecord{.begin = 9h,
                  .end = 10h,
                  .end_range = 5min,
                  .phase = TradingPhase::Option::OpeningAuction},
  });

  const auto scheduled = scheduler.phase_at(local_at(9h + 30min));

  EXPECT_THAT(
      scheduled.phase,
      Property(&Phase::phase, Eq(TradingPhase::Option::OpeningAuction)));
  ASSERT_TRUE(scheduled.auction.has_value());
  EXPECT_EQ(scheduled.auction->end, 10h);
  EXPECT_EQ(scheduled.auction->end_range, 5min);
}

TEST_F(PhaseSchedulerTest,
       PhaseAtReturnsScheduledPhaseWithoutEndWindowForNonAuctionPhase) {
  scheduler.configure({
      PhaseRecord{
          .begin = 12h, .end = 14h, .phase = TradingPhase::Option::Closed},
  });

  const auto scheduled = scheduler.phase_at(local_at(13h));

  EXPECT_THAT(scheduled.phase,
              Property(&Phase::phase, Eq(TradingPhase::Option::Closed)));
  EXPECT_FALSE(scheduled.auction.has_value());
}

}  // namespace
}  // namespace simulator::trading_system::ies::test
