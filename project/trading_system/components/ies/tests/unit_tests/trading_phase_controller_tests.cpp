#include <gmock/gmock.h>

#include <chrono>
#include <vector>

#include "core/tools/time.hpp"
#include "ih/tick_event_factory.hpp"
#include "ih/trading_phase_controller.hpp"

namespace simulator::trading_system::ies::test {
namespace {

using namespace ::testing;  // NOLINT
using namespace std::chrono_literals;

struct TradingSystemIesTradingPhaseController : public Test {
  std::optional<Phase> phase;

  auto tick(auto time) -> void {
    auto event_tick = event_factory_.create_tick_event(core::sys_us(time));
    controller_.update(event_tick);
  }

  auto schedule(PhaseSchedule phase_schedule) -> void {
    controller_.configure(phase_schedule);
  }

  auto send_halt_request(protocol::HaltPhaseRequest request = {})
      -> protocol::HaltPhaseReply {
    protocol::HaltPhaseReply reply;
    controller_.process(request, reply);
    return reply;
  }

  auto send_resume_request() -> protocol::ResumePhaseReply {
    constexpr protocol::ResumePhaseRequest request;
    protocol::ResumePhaseReply reply;
    controller_.process(request, reply);
    return reply;
  }

  static auto TradingPhaseIs(TradingPhase phase) {
    return Optional(Property(&Phase::phase, Eq(phase)));
  }

  static auto TradingStatusIs(TradingStatus status) {
    return Optional(Property(&Phase::status, Eq(status)));
  }

  auto SetUp() -> void override {
    controller_.bind(
        [this](event::PhaseTransition event) { phase = event.phase; });
  }

 private:
  TradingPhaseController controller_;
  TickEventFactory event_factory_;
};

TEST_F(TradingSystemIesTradingPhaseController, PassesScheduledPhase) {
  schedule({{.begin = 12h, .end = 15h, .phase = TradingPhase::Option::Open}});

  tick(12h);
  ASSERT_THAT(phase, TradingPhaseIs(TradingPhase::Option::Open));
  ASSERT_THAT(phase, TradingStatusIs(TradingStatus::Option::Resume));
}

TEST_F(TradingSystemIesTradingPhaseController,
       AcceptsHaltRequestOnScheduledDefaultPhase) {
  tick(12h);

  const auto reply = send_halt_request();

  ASSERT_THAT(reply.result,
              Optional(Eq(protocol::HaltPhaseReply::Result::Halted)));
}

TEST_F(TradingSystemIesTradingPhaseController,
       AcceptsHaltRequestOnScheduledPhase) {
  schedule({{.begin = 12h, .end = 15h, .phase = TradingPhase::Option::Open}});
  tick(12h);

  const auto reply = send_halt_request();

  ASSERT_THAT(reply.result,
              Optional(Eq(protocol::HaltPhaseReply::Result::Halted)));
}

TEST_F(TradingSystemIesTradingPhaseController,
       RejectsHaltRequestWhenSameHaltIsAlreadyRequested) {
  tick(12h);

  send_halt_request();
  const auto reply = send_halt_request();

  ASSERT_THAT(
      reply.result,
      Optional(Eq(protocol::HaltPhaseReply::Result::AlreadyHaltedByRequest)));
}

TEST_F(TradingSystemIesTradingPhaseController,
       RejectsHaltRequestWhenPhaseIsClosed) {
  schedule({{.begin = 12h, .end = 15h, .phase = TradingPhase::Option::Closed}});
  tick(12h);

  const auto reply = send_halt_request();

  ASSERT_THAT(reply.result,
              Optional(Eq(protocol::HaltPhaseReply::Result::UnableToHalt)));
}

TEST_F(TradingSystemIesTradingPhaseController, HaltsOpenPhaseOnNextTick) {
  schedule({{.begin = 12h, .end = 15h, .phase = TradingPhase::Option::Open}});
  tick(12h);

  send_halt_request();

  tick(12h + 1s);

  ASSERT_THAT(phase, TradingStatusIs(TradingStatus::Option::Halt));
}

TEST_F(TradingSystemIesTradingPhaseController, SetsAllowCancelsOnHalting) {
  schedule({{.begin = 12h, .end = 15h, .phase = TradingPhase::Option::Open}});
  tick(12h);

  send_halt_request(protocol::HaltPhaseRequest{.allow_cancels = true});

  tick(12h + 1s);

  ASSERT_THAT(phase->settings(),
              Optional(Field(&Phase::Settings::allow_cancels, Eq(true))));
}

TEST_F(TradingSystemIesTradingPhaseController,
       RejectsHaltRequestOnHaltedPhase) {
  schedule({{.begin = 12h, .end = 15h, .phase = TradingPhase::Option::Open},
            {.begin = 13h, .end = 14h, .phase = TradingStatus::Option::Halt}});
  tick(13h);

  const auto reply = send_halt_request();

  ASSERT_THAT(reply.result,
              Optional(Eq(protocol::HaltPhaseReply::Result::UnableToHalt)));
}

TEST_F(TradingSystemIesTradingPhaseController,
       DoesNotHaltPhaseAfterScheduledHalt) {
  schedule({{.begin = 12h, .end = 15h, .phase = TradingPhase::Option::Open},
            {.begin = 13h, .end = 14h, .phase = TradingStatus::Option::Halt}});
  tick(12h);

  send_halt_request();
  tick(12h + 1s);
  ASSERT_THAT(phase, TradingStatusIs(TradingStatus::Option::Halt));

  // Scheduled Open Halt phase
  tick(13h);
  // Scheduled Open Resume phase
  tick(14h);

  ASSERT_THAT(phase, TradingStatusIs(TradingStatus::Option::Resume));
}

TEST_F(TradingSystemIesTradingPhaseController,
       DoesNotHaltPhaseAfterScheduledClosed) {
  schedule({{.begin = 12h, .end = 15h, .phase = TradingPhase::Option::Open},
            {.begin = 13h, .end = 14h, .phase = TradingPhase::Option::Closed}});
  tick(12h);

  send_halt_request();
  tick(12h + 1s);
  ASSERT_THAT(phase, TradingStatusIs(TradingStatus::Option::Halt));

  // Scheduled Closed phase
  tick(13h);
  // Scheduled Open Resume phase
  tick(14h);

  ASSERT_THAT(phase, TradingStatusIs(TradingStatus::Option::Resume));
}

TEST_F(TradingSystemIesTradingPhaseController,
       DoesNotResumeWithoutActivePhase) {
  const auto reply = send_resume_request();

  ASSERT_THAT(reply.result,
              Eq(protocol::ResumePhaseReply::Result::NoRequestedHalt));
}

TEST_F(TradingSystemIesTradingPhaseController,
       DoesNotResumeWithoutRequstedHalt) {
  schedule({{.begin = 12h, .end = 13h, .phase = TradingPhase::Option::Open}});
  tick(12h);

  const auto reply = send_resume_request();
  ASSERT_THAT(reply.result,
              Eq(protocol::ResumePhaseReply::Result::NoRequestedHalt));
}

TEST_F(TradingSystemIesTradingPhaseController, DoesNotResumeScheduledHalt) {
  schedule({{.begin = 12h, .end = 13h, .phase = TradingPhase::Option::Open},
            {.begin = 13h, .end = 14h, .phase = TradingStatus::Option::Halt}});
  tick(13h);

  const auto reply = send_resume_request();
  ASSERT_THAT(reply.result,
              Eq(protocol::ResumePhaseReply::Result::NoRequestedHalt));
}

TEST_F(TradingSystemIesTradingPhaseController, ResumesRequestedHalt) {
  schedule({{.begin = 12h, .end = 13h, .phase = TradingPhase::Option::Open}});
  tick(12h);

  send_halt_request();
  tick(12h + 1s);

  const auto reply = send_resume_request();
  tick(12h + 2s);
  ASSERT_THAT(reply.result, Eq(protocol::ResumePhaseReply::Result::Resumed));
  ASSERT_THAT(phase, TradingStatusIs(TradingStatus::Option::Resume));
}

TEST_F(TradingSystemIesTradingPhaseController,
       PassesScheduledTradeAtLastPhase) {
  schedule(
      {{.begin = 12h, .end = 15h, .phase = TradingPhase::Option::PostTrading}});

  tick(12h);

  ASSERT_THAT(phase, TradingPhaseIs(TradingPhase::Option::PostTrading));
  ASSERT_THAT(phase, TradingStatusIs(TradingStatus::Option::Resume));
}

TEST_F(TradingSystemIesTradingPhaseController,
       EntersTradeAtLastPhaseFromClosedPhase) {
  schedule({{.begin = 11h, .end = 12h, .phase = TradingPhase::Option::Closed},
            {.begin = 12h,
             .end = 15h,
             .phase = TradingPhase::Option::PostTrading}});
  tick(11h);

  tick(12h);

  ASSERT_THAT(phase, TradingPhaseIs(TradingPhase::Option::PostTrading));
}

TEST_F(TradingSystemIesTradingPhaseController,
       LeavesTradeAtLastPhaseAtItsEndTimeIgnoringEndTimeRange) {
  schedule({{.begin = 12h,
             .end = 15h,
             .end_range = 5min,
             .phase = TradingPhase::Option::PostTrading},
            {.begin = 15h, .end = 16h, .phase = TradingPhase::Option::Closed}});
  tick(12h);

  tick(15h);

  ASSERT_THAT(phase, TradingPhaseIs(TradingPhase::Option::Closed));
}

TEST_F(TradingSystemIesTradingPhaseController,
       HaltsTradeAtLastPhaseEnteredFromOpenPhase) {
  schedule({{.begin = 11h, .end = 12h, .phase = TradingPhase::Option::Open},
            {.begin = 12h,
             .end = 15h,
             .phase = TradingPhase::Option::PostTrading}});
  tick(11h);
  tick(12h);

  const auto reply = send_halt_request();

  ASSERT_THAT(reply.result,
              Optional(Eq(protocol::HaltPhaseReply::Result::Halted)));
  ASSERT_THAT(phase, TradingPhaseIs(TradingPhase::Option::PostTrading));
  ASSERT_THAT(phase, TradingStatusIs(TradingStatus::Option::Halt));
}

TEST_F(TradingSystemIesTradingPhaseController,
       ResumesRequestedHaltOnTradeAtLastPhase) {
  schedule(
      {{.begin = 12h, .end = 15h, .phase = TradingPhase::Option::PostTrading}});
  tick(12h);
  send_halt_request();

  const auto reply = send_resume_request();

  ASSERT_THAT(reply.result, Eq(protocol::ResumePhaseReply::Result::Resumed));
  ASSERT_THAT(phase, TradingPhaseIs(TradingPhase::Option::PostTrading));
  ASSERT_THAT(phase, TradingStatusIs(TradingStatus::Option::Resume));
}

struct TradingSystemIesAuctionController : public Test {
  std::vector<Phase> reported_phases;
  std::chrono::seconds captured_low{0};
  std::chrono::seconds captured_high{0};

  auto SetUp() -> void override {
    event_factory_.set_tz_clock(utc_);
    controller_.set_tz_clock(utc_);
    controller_.bind([this](event::PhaseTransition event) {
      reported_phases.push_back(event.phase);
    });
    controller_.set_uncross_delay_generator(
        [this](std::chrono::seconds low,
               std::chrono::seconds high) -> std::chrono::seconds {
          captured_low = low;
          captured_high = high;
          return uncross_offset_;
        });
  }

  auto schedule(const PhaseSchedule& phase_schedule) -> void {
    controller_.configure(phase_schedule);
  }

  auto set_uncross_offset(const std::chrono::seconds offset) -> void {
    uncross_offset_ = offset;
  }

  auto tick(std::chrono::microseconds time_of_day) -> void {
    controller_.update(
        event_factory_.create_tick_event(core::sys_us{time_of_day}));
  }

  auto send_halt_request() -> protocol::HaltPhaseReply {
    protocol::HaltPhaseReply reply;
    controller_.process(protocol::HaltPhaseRequest{}, reply);
    return reply;
  }

  auto send_resume_request() -> protocol::ResumePhaseReply {
    protocol::ResumePhaseReply reply;
    controller_.process(protocol::ResumePhaseRequest{}, reply);
    return reply;
  }

  static auto call_of(TradingPhase auction_phase) -> Phase {
    return {auction_phase, TradingStatus::Option::Resume, {}};
  }

  static auto uncrossing_of(TradingPhase auction_phase) -> Phase {
    return {auction_phase, TradingStatus::Option::Halt, {}};
  }

  static auto open_phase() -> Phase {
    return {TradingPhase::Option::Open, TradingStatus::Option::Resume, {}};
  }

  static auto closed_phase() -> Phase {
    return {TradingPhase::Option::Closed, TradingStatus::Option::Halt, {}};
  }

  static auto trade_at_last_phase() -> Phase {
    return {
        TradingPhase::Option::PostTrading, TradingStatus::Option::Resume, {}};
  }

 private:
  std::chrono::seconds uncross_offset_{0};
  core::TzClock utc_{"UTC"};
  TickEventFactory event_factory_;
  TradingPhaseController controller_;
};

TEST_F(TradingSystemIesAuctionController,
       ReportsCallSubPhaseWhenAuctionStarts) {
  schedule({{.begin = 9h,
             .end = 10h,
             .phase = TradingPhase::Option::OpeningAuction}});

  tick(9h);

  ASSERT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::OpeningAuction)));
}

TEST_F(TradingSystemIesAuctionController,
       UncrossesAtConfiguredEndAndJumpsToScheduledPhase) {
  set_uncross_offset(0s);
  schedule({{.begin = 16h,
             .end = 17h,
             .phase = TradingPhase::Option::ClosingAuction},
            {.begin = 17h, .end = 18h, .phase = TradingPhase::Option::Closed}});

  tick(16h);
  tick(17h);

  ASSERT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::ClosingAuction),
                          uncrossing_of(TradingPhase::Option::ClosingAuction),
                          closed_phase()));
}

TEST_F(TradingSystemIesAuctionController,
       FallsBackToOpenWhenNoPhaseIsScheduledAtAuctionEnd) {
  set_uncross_offset(0s);
  schedule({{.begin = 9h,
             .end = 10h,
             .phase = TradingPhase::Option::OpeningAuction}});

  tick(9h);
  tick(10h);

  ASSERT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::OpeningAuction),
                          uncrossing_of(TradingPhase::Option::OpeningAuction),
                          open_phase()));
}

TEST_F(TradingSystemIesAuctionController, ChainsConsecutiveAuctions) {
  set_uncross_offset(0s);
  schedule(
      {{.begin = 9h, .end = 10h, .phase = TradingPhase::Option::OpeningAuction},
       {.begin = 10h,
        .end = 11h,
        .phase = TradingPhase::Option::IntradayAuction}});

  tick(9h);
  tick(10h);
  tick(11h);

  ASSERT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::OpeningAuction),
                          uncrossing_of(TradingPhase::Option::OpeningAuction),
                          call_of(TradingPhase::Option::IntradayAuction),
                          uncrossing_of(TradingPhase::Option::IntradayAuction),
                          open_phase()));
}

TEST_F(TradingSystemIesAuctionController,
       RandomisesUncrossingWithinRangeAndHoldsPastConfiguredEnd) {
  set_uncross_offset(2min);
  schedule({{.begin = 9h,
             .end = 10h,
             .end_range = 5min,
             .phase = TradingPhase::Option::OpeningAuction}});

  tick(9h);
  tick(10h);  // configured end reached, but the randomised uncross is later

  ASSERT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::OpeningAuction)));
  EXPECT_EQ(captured_low, -5min);
  EXPECT_EQ(captured_high, 5min);

  tick(10h + 2min);

  ASSERT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::OpeningAuction),
                          uncrossing_of(TradingPhase::Option::OpeningAuction),
                          open_phase()));
}

TEST_F(TradingSystemIesAuctionController,
       FallsBackToOpenWhenScheduledPhaseEndedBeforeUncrossingCompleted) {
  set_uncross_offset(40min);
  schedule(
      {{.begin = 9h, .end = 10h, .phase = TradingPhase::Option::OpeningAuction},
       {.begin = 10h,
        .end = 10h + 30min,
        .phase = TradingPhase::Option::Closed}});

  tick(9h);
  tick(10h + 15min);  // held: auction owns the timeline, scheduler is ignored

  ASSERT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::OpeningAuction)));

  tick(10h + 40min);  // completes after the scheduled Closed window has elapsed

  ASSERT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::OpeningAuction),
                          uncrossing_of(TradingPhase::Option::OpeningAuction),
                          open_phase()));
}

TEST_F(TradingSystemIesAuctionController,
       JumpsToScheduledPhaseStillActiveWhenUncrossingOverruns) {
  set_uncross_offset(40min);
  schedule(
      {{.begin = 9h, .end = 10h, .phase = TradingPhase::Option::OpeningAuction},
       {.begin = 10h, .end = 11h, .phase = TradingPhase::Option::Closed}});

  tick(9h);
  tick(10h + 40min);  // overruns, but the scheduled Closed window is still open

  ASSERT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::OpeningAuction),
                          uncrossing_of(TradingPhase::Option::OpeningAuction),
                          closed_phase()));
}

TEST_F(TradingSystemIesAuctionController,
       JumpsToScheduledAtEndTimePhaseForEarlyUncrossing) {
  set_uncross_offset(-5min);
  schedule(
      {{.begin = 9h, .end = 10h, .phase = TradingPhase::Option::OpeningAuction},
       {.begin = 10h, .end = 11h, .phase = TradingPhase::Option::Closed}});

  tick(9h);
  tick(9h + 55min);
  tick(9h + 56min);  // still inside the auction's scheduled window

  ASSERT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::OpeningAuction),
                          uncrossing_of(TradingPhase::Option::OpeningAuction),
                          closed_phase()));
}

TEST_F(TradingSystemIesAuctionController,
       JumpsToTradeAtLastScheduledAfterClosingAuction) {
  set_uncross_offset(0s);
  schedule(
      {{.begin = 16h,
        .end = 17h,
        .phase = TradingPhase::Option::ClosingAuction},
       {.begin = 17h, .end = 18h, .phase = TradingPhase::Option::PostTrading},
       {.begin = 18h, .end = 19h, .phase = TradingPhase::Option::Closed}});

  tick(16h);
  tick(17h);
  tick(18h);

  ASSERT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::ClosingAuction),
                          uncrossing_of(TradingPhase::Option::ClosingAuction),
                          trade_at_last_phase(),
                          closed_phase()));
}

TEST_F(TradingSystemIesAuctionController, RejectsHaltRequestDuringAuction) {
  schedule({{.begin = 9h,
             .end = 10h,
             .phase = TradingPhase::Option::OpeningAuction}});
  tick(9h);

  const auto reply = send_halt_request();

  EXPECT_THAT(
      reply.result,
      Optional(Eq(protocol::HaltPhaseReply::Result::AuctionInProgress)));
  EXPECT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::OpeningAuction)));
}

TEST_F(TradingSystemIesAuctionController, RejectsResumeRequestDuringAuction) {
  schedule({{.begin = 9h,
             .end = 10h,
             .phase = TradingPhase::Option::OpeningAuction}});
  tick(9h);

  const auto reply = send_resume_request();

  EXPECT_THAT(reply.result,
              Eq(protocol::ResumePhaseReply::Result::AuctionInProgress));
  EXPECT_THAT(reported_phases,
              ElementsAre(call_of(TradingPhase::Option::OpeningAuction)));
}

TEST_F(TradingSystemIesAuctionController,
       StartsAuctionEvenWhenPhaseWasHaltedByRequest) {
  schedule({{.begin = 8h, .end = 9h, .phase = TradingPhase::Option::Open},
            {.begin = 9h,
             .end = 10h,
             .phase = TradingPhase::Option::OpeningAuction}});

  tick(8h);
  send_halt_request();
  tick(9h);

  ASSERT_THAT(reported_phases,
              ElementsAre(open_phase(),
                          Phase(TradingPhase::Option::Open,
                                TradingStatus::Option::Halt,
                                {.allow_cancels = false}),
                          call_of(TradingPhase::Option::OpeningAuction)));
}

}  // namespace
}  // namespace simulator::trading_system::ies::test