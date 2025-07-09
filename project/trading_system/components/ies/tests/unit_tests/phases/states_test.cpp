#include "ih/phases/states.hpp"

#include <gmock/gmock.h>

namespace simulator::trading_system::ies {
namespace {
using namespace ::testing;  // NOLINT

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

}  // namespace
}  // namespace simulator::trading_system::ies