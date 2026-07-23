#include <gmock/gmock.h>

#include <chrono>
#include <optional>

#include "core/tools/time.hpp"
#include "ih/phases/states.hpp"

namespace simulator::trading_system::ies::test {
namespace {

using namespace ::testing;  // NOLINT
using namespace std::chrono_literals;

struct TradingSystemIesState : public Test {
  template <typename StateT>
  auto IsState() {
    return Optional(VariantWith<StateT>(_));
  }

  static auto get_open(const std::optional<State>& state) -> OpenState {
    return std::get<OpenState>(*state);
  }

  static auto get_closed(const std::optional<State>& state) -> ClosedState {
    return std::get<ClosedState>(*state);
  }

  protocol::HaltPhaseRequest halt_request;
  protocol::HaltPhaseReply halt_reply;
  protocol::ResumePhaseRequest resume_request;
  protocol::ResumePhaseReply resume_reply;
};

struct TradingSystemIesOpenState : public TradingSystemIesState {
  OpenState open_state{
      Phase(TradingPhase::Option::Open, TradingStatus::Option::Resume, {})};
};

TEST_F(TradingSystemIesOpenState, Halts) {
  const auto new_state = open_state.halt(halt_request, halt_reply);

  ASSERT_THAT(new_state, IsState<OpenState>());
  ASSERT_EQ(std::get<OpenState>(*new_state).phase(),
            Phase(TradingPhase::Option::Open,
                  TradingStatus::Option::Halt,
                  {.allow_cancels = false}));

  ASSERT_EQ(halt_reply.result, protocol::HaltPhaseReply::Result::Halted);
}

TEST_F(TradingSystemIesOpenState, SetsHaltRequestSettings) {
  halt_request.allow_cancels = true;
  const auto new_state = open_state.halt(halt_request, halt_reply);

  ASSERT_THAT(new_state, IsState<OpenState>());
  ASSERT_EQ(get_open(new_state).phase().settings()->allow_cancels, true);

  ASSERT_EQ(halt_reply.result, protocol::HaltPhaseReply::Result::Halted);
}

TEST_F(TradingSystemIesOpenState, DoesNotHaltHaltedPhaseByRequest) {
  const auto halted_open_state = open_state.halt(halt_request, halt_reply);
  const auto new_state =
      get_open(halted_open_state).halt(halt_request, halt_reply);

  ASSERT_EQ(new_state, std::nullopt);
  ASSERT_EQ(halt_reply.result,
            protocol::HaltPhaseReply::Result::AlreadyHaltedByRequest);
}

TEST_F(TradingSystemIesOpenState, DoesNotHaltScheduledHaltedPhase) {
  const auto halted_phase = Phase(TradingPhase::Option::Open,
                                  TradingStatus::Option::Halt,
                                  {.allow_cancels = false});
  const auto halted_by_schedule = open_state.update(halted_phase);
  const auto halted_by_request =
      get_open(halted_by_schedule).halt(halt_request, halt_reply);

  ASSERT_EQ(halted_by_request, std::nullopt);
  ASSERT_EQ(halt_reply.result, protocol::HaltPhaseReply::Result::UnableToHalt);
}

TEST_F(TradingSystemIesOpenState, DoesNotResumeAlreadyResumePhase) {
  const auto new_state = open_state.resume(resume_request, resume_reply);

  ASSERT_EQ(new_state, std::nullopt);
  ASSERT_EQ(resume_reply.result,
            protocol::ResumePhaseReply::Result::NoRequestedHalt);
}

TEST_F(TradingSystemIesOpenState, DoesNotResumeScheduleHaltedPhase) {
  const Phase halted_open{TradingPhase::Option::Open,
                          TradingStatus::Option::Halt,
                          {.allow_cancels = false}};
  const auto halted_state = open_state.update(halted_open);
  const auto resumed_state =
      get_open(halted_state).resume(resume_request, resume_reply);

  ASSERT_EQ(resumed_state, std::nullopt);
  ASSERT_EQ(resume_reply.result,
            protocol::ResumePhaseReply::Result::NoRequestedHalt);
}

TEST_F(TradingSystemIesOpenState, ResumesHaltedByRequest) {
  const auto halted_state = open_state.halt(halt_request, halt_reply);
  const auto resumed_state =
      get_open(halted_state).resume(resume_request, resume_reply);

  ASSERT_THAT(resumed_state, IsState<OpenState>());
  ASSERT_EQ(
      get_open(resumed_state).phase(),
      Phase(TradingPhase::Option::Open, TradingStatus::Option::Resume, {}));
  ASSERT_EQ(resume_reply.result, protocol::ResumePhaseReply::Result::Resumed);
}

TEST_F(TradingSystemIesOpenState,
       DoesNotResumeHaltedByRequestAfterScheduledHalting) {
  const auto halted_by_request = open_state.halt(halt_request, halt_reply);
  const auto halted_by_schedule = get_open(halted_by_request)
                                      .update({TradingPhase::Option::Open,
                                               TradingStatus::Option::Halt,
                                               {.allow_cancels = false}});

  const auto resumed_state =
      get_open(halted_by_schedule).resume(resume_request, resume_reply);

  ASSERT_EQ(resumed_state, std::nullopt);
  ASSERT_EQ(resume_reply.result,
            protocol::ResumePhaseReply::Result::NoRequestedHalt);
}

TEST_F(TradingSystemIesOpenState, SetsScheduledHalt) {
  const Phase open_halted{TradingPhase::Option::Open,
                          TradingStatus::Option::Halt,
                          {.allow_cancels = true}};
  const auto new_state = open_state.update(open_halted);

  ASSERT_THAT(new_state, IsState<OpenState>());
  ASSERT_EQ(get_open(new_state).phase(), open_halted);
}

TEST_F(
    TradingSystemIesOpenState,
    TransformsFromHaltedOpenStateToHaltedOpenStateIfTheSettingsAreDifferent) {
  const Phase open_halted_allow_cancels_true{TradingPhase::Option::Open,
                                             TradingStatus::Option::Halt,
                                             {.allow_cancels = true}};
  auto state_allow_cancels_true =
      open_state.update(open_halted_allow_cancels_true);

  const Phase open_halted_allow_cancels_false{TradingPhase::Option::Open,
                                              TradingStatus::Option::Halt,
                                              {.allow_cancels = false}};
  const auto state_allow_cancels_false =
      get_open(state_allow_cancels_true)
          .update(open_halted_allow_cancels_false);

  ASSERT_THAT(state_allow_cancels_false, IsState<OpenState>());
  ASSERT_EQ(get_open(state_allow_cancels_false).phase(),
            open_halted_allow_cancels_false);
}

TEST_F(TradingSystemIesOpenState, DoesNotTransformsToTheSamePhase) {
  const Phase open{
      TradingPhase::Option::Open, TradingStatus::Option::Resume, {}};
  const auto new_state = open_state.update(open);

  ASSERT_EQ(new_state, std::nullopt);
}

TEST_F(TradingSystemIesOpenState, TransformsToClosedState) {
  const Phase closed_phase{
      TradingPhase::Option::Closed, TradingStatus::Option::Halt, {}};
  const auto new_state = open_state.update(closed_phase);

  ASSERT_THAT(new_state, IsState<ClosedState>());
  ASSERT_EQ(get_closed(new_state).phase(), closed_phase);
}

struct TradingSystemIesClosedState : public TradingSystemIesState {
  ClosedState closed_state;
};

TEST_F(TradingSystemIesClosedState, DefaultPhaseIsClosedAndHalted) {
  ASSERT_EQ(
      closed_state.phase(),
      Phase(TradingPhase::Option::Closed, TradingStatus::Option::Halt, {}));
}

TEST_F(TradingSystemIesClosedState, DoesNotHalt) {
  const auto new_state = closed_state.halt(halt_request, halt_reply);

  ASSERT_EQ(new_state, std::nullopt);
  ASSERT_EQ(halt_reply.result, protocol::HaltPhaseReply::Result::UnableToHalt);
}

TEST_F(TradingSystemIesClosedState, DoesNotResume) {
  const auto new_state = closed_state.resume(resume_request, resume_reply);

  ASSERT_EQ(new_state, std::nullopt);
  ASSERT_EQ(resume_reply.result,
            protocol::ResumePhaseReply::Result::NoRequestedHalt);
}

TEST_F(TradingSystemIesClosedState, DoesNotTransformToClosedState) {
  const auto new_state = closed_state.update(
      {TradingPhase::Option::Closed, TradingStatus::Option::Halt, {}});

  ASSERT_EQ(new_state, std::nullopt);
}

TEST_F(TradingSystemIesClosedState, TransformsToOpenState) {
  const Phase open_phase{
      TradingPhase::Option::Open, TradingStatus::Option::Resume, {}};
  const auto new_state = closed_state.update(open_phase);

  ASSERT_THAT(new_state, IsState<OpenState>());
  ASSERT_EQ(get_open(new_state).phase(), open_phase);
}

struct TradingSystemIesAuctionState : public TradingSystemIesState {
  static auto instant(std::chrono::microseconds time) -> core::local_us {
    return core::local_us{} + time;
  }

  AuctionState auction{
      TradingPhase::Option::OpeningAuction,
      AuctionActivation{.end = instant(10h), .uncross_at = instant(10h)}};
};

TEST_F(TradingSystemIesAuctionState, IsNotDueBeforeUncrossingTime) {
  EXPECT_FALSE(auction.uncrossing_due(instant(10h) - 1us));
}

TEST_F(TradingSystemIesAuctionState, BecomesDueWhenUncrossingTimeIsReached) {
  EXPECT_TRUE(auction.uncrossing_due(instant(10h)));
  EXPECT_TRUE(auction.uncrossing_due(instant(10h) + 1us));
}

TEST_F(TradingSystemIesAuctionState, StopsBeingDueOnceUncrossingHasBegun) {
  const auto uncrossing = auction.begin_uncrossing();

  EXPECT_FALSE(uncrossing.uncrossing_due(instant(10h) + 1us));
}

TEST_F(TradingSystemIesAuctionState, RejectsHaltRequest) {
  const auto new_state = auction.halt(halt_request, halt_reply);

  ASSERT_EQ(new_state, std::nullopt);
  ASSERT_EQ(halt_reply.result,
            protocol::HaltPhaseReply::Result::AuctionInProgress);
}

TEST_F(TradingSystemIesAuctionState, RejectsResumeRequest) {
  const auto new_state = auction.resume(resume_request, resume_reply);

  ASSERT_EQ(new_state, std::nullopt);
  ASSERT_EQ(resume_reply.result,
            protocol::ResumePhaseReply::Result::AuctionInProgress);
}

TEST_F(TradingSystemIesAuctionState, RejectsHaltRequestDuringUncrossing) {
  const auto new_state =
      auction.begin_uncrossing().halt(halt_request, halt_reply);

  ASSERT_EQ(new_state, std::nullopt);
  ASSERT_EQ(halt_reply.result,
            protocol::HaltPhaseReply::Result::AuctionInProgress);
}

TEST_F(TradingSystemIesAuctionState, RejectsResumeRequestDuringUncrossing) {
  const auto new_state =
      auction.begin_uncrossing().resume(resume_request, resume_reply);

  ASSERT_EQ(new_state, std::nullopt);
  ASSERT_EQ(resume_reply.result,
            protocol::ResumePhaseReply::Result::AuctionInProgress);
}

TEST_F(TradingSystemIesAuctionState, IgnoresScheduledPhaseUpdate) {
  const auto new_state = auction.update(
      {TradingPhase::Option::Closed, TradingStatus::Option::Halt, {}});

  ASSERT_EQ(new_state, std::nullopt);
}

struct TradingSystemIesAuctionPhaseReporting
    : public TestWithParam<TradingPhase::Option> {
  static auto instant(std::chrono::microseconds time) -> core::local_us {
    return core::local_us{} + time;
  }

  AuctionState auction{
      TradingPhase{GetParam()},
      AuctionActivation{.end = instant(10h), .uncross_at = instant(10h)}};
};

TEST_P(TradingSystemIesAuctionPhaseReporting, CallSubPhaseResumesAuctionPhase) {
  ASSERT_EQ(auction.phase(),
            Phase(TradingPhase{GetParam()}, TradingStatus::Option::Resume, {}));
}

TEST_P(TradingSystemIesAuctionPhaseReporting,
       UncrossingSubPhaseHaltsAuctionPhase) {
  ASSERT_EQ(auction.begin_uncrossing().phase(),
            Phase(TradingPhase{GetParam()}, TradingStatus::Option::Halt, {}));
}

INSTANTIATE_TEST_SUITE_P(AuctionPhases,
                         TradingSystemIesAuctionPhaseReporting,
                         Values(TradingPhase::Option::OpeningAuction,
                                TradingPhase::Option::IntradayAuction,
                                TradingPhase::Option::ClosingAuction));

struct TradingSystemIesStateCreation : public TradingSystemIesState {
  static auto instant(std::chrono::microseconds time) -> core::local_us {
    return core::local_us{} + time;
  }

  AuctionActivation activation{.end = instant(10h), .uncross_at = instant(10h)};
};

TEST_F(TradingSystemIesStateCreation,
       CreatesAuctionStateForAuctionPhaseWithActivation) {
  const auto state = create_state(
      {TradingPhase::Option::OpeningAuction, TradingStatus::Option::Resume, {}},
      activation);

  ASSERT_THAT(state, IsState<AuctionState>());
  EXPECT_EQ(std::get<AuctionState>(*state).phase(),
            Phase(TradingPhase::Option::OpeningAuction,
                  TradingStatus::Option::Resume,
                  {}));
}

TEST_F(TradingSystemIesStateCreation,
       ReturnsNoStateForAuctionPhaseWithoutActivation) {
  const auto state = create_state({TradingPhase::Option::OpeningAuction,
                                   TradingStatus::Option::Resume,
                                   {}});

  ASSERT_EQ(state, std::nullopt);
}

TEST_F(TradingSystemIesStateCreation, ReturnsNoStateForPostTradingPhase) {
  const auto state = create_state(
      {TradingPhase::Option::PostTrading, TradingStatus::Option::Resume, {}});

  ASSERT_EQ(state, std::nullopt);
}

}  // namespace
}  // namespace simulator::trading_system::ies::test