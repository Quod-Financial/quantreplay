#include "ih/trading_phase_controller.hpp"

#include <gmock/gmock.h>

#include "ih/tick_event_factory.hpp"

namespace simulator::trading_system::ies {
namespace {

using namespace ::testing;

using namespace std::chrono_literals;

struct TradingSystemIesTradingPhaseController : public Test {
  std::optional<Phase> phase;

  auto tick(auto time) -> void {
    auto event_tick = event_factory_.create_tick_event(core::sys_us(time));
    controller.update(event_tick);
  }

  auto schedule(PhaseSchedule phase_schedule) -> void {
    controller.configure(phase_schedule);
  }

  auto send_halt_request(protocol::HaltPhaseRequest request = {})
      -> protocol::HaltPhaseReply {
    protocol::HaltPhaseReply reply;
    controller.process(request, reply);
    return reply;
  }

  auto send_resume_request() -> protocol::ResumePhaseReply {
    constexpr protocol::ResumePhaseRequest request;
    protocol::ResumePhaseReply reply;
    controller.process(request, reply);
    return reply;
  }

  static auto TradingPhaseIs(TradingPhase phase) {
    return Optional(Property(&Phase::phase, Eq(phase)));
  }

  static auto TradingStatusIs(TradingStatus status) {
    return Optional(Property(&Phase::status, Eq(status)));
  }

  static auto TradingStatusNe(TradingStatus status) {
    return Optional(Property(&Phase::status, Ne(status)));
  }

  auto SetUp() -> void override {
    controller.bind(
        [this](event::PhaseTransition event) { phase = event.phase; });
  }

 private:
  TradingPhaseController controller;
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

}  // namespace
}  // namespace simulator::trading_system::ies