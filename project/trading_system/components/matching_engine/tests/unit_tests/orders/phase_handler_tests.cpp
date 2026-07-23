#include <gmock/gmock.h>

#include <string>

#include "common/events.hpp"
#include "core/tools/time.hpp"
#include "ih/orders/phase_handler.hpp"
#include "tests/mocks/event_listener_mock.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

struct PhaseHandlerQueryCase {
  std::string name;
  TradingPhase::Option phase;
  TradingStatus::Option status;
  bool in_auction_phase;
  bool in_auction_call;
  bool in_auction_uncross;
};

struct MatchingEnginePhaseHandlerQueries
    : public TestWithParam<PhaseHandlerQueryCase> {
  NiceMock<EventListenerMock> event_listener;
  PhaseHandler phase_handler{event_listener};

  auto transition_to(TradingPhase::Option phase,
                     TradingStatus::Option status) -> void {
    phase_handler.handle(event::PhaseTransition{
        .tz_time_point = core::tz_us{},
        .phase = Phase{phase, status, Phase::Settings{}}});
  }
};

TEST_P(MatchingEnginePhaseHandlerQueries, InAuctionPhaseReflectsTradingPhase) {
  const auto& param = GetParam();
  transition_to(param.phase, param.status);

  ASSERT_EQ(phase_handler.in_auction_phase(), param.in_auction_phase);
}

TEST_P(MatchingEnginePhaseHandlerQueries,
       InAuctionCallReflectsResumeStatusInAuction) {
  const auto& param = GetParam();
  transition_to(param.phase, param.status);

  ASSERT_EQ(phase_handler.in_auction_call(), param.in_auction_call);
}

TEST_P(MatchingEnginePhaseHandlerQueries,
       InAuctionUncrossReflectsHaltStatusInAuction) {
  const auto& param = GetParam();
  transition_to(param.phase, param.status);

  ASSERT_EQ(phase_handler.in_auction_uncross(), param.in_auction_uncross);
}

INSTANTIATE_TEST_SUITE_P(
    PhaseHandler,
    MatchingEnginePhaseHandlerQueries,
    Values(PhaseHandlerQueryCase{.name = "OpeningAuctionCall",
                                 .phase = TradingPhase::Option::OpeningAuction,
                                 .status = TradingStatus::Option::Resume,
                                 .in_auction_phase = true,
                                 .in_auction_call = true,
                                 .in_auction_uncross = false},
           PhaseHandlerQueryCase{.name = "OpeningAuctionUncross",
                                 .phase = TradingPhase::Option::OpeningAuction,
                                 .status = TradingStatus::Option::Halt,
                                 .in_auction_phase = true,
                                 .in_auction_call = false,
                                 .in_auction_uncross = true},
           PhaseHandlerQueryCase{.name = "IntradayAuctionCall",
                                 .phase = TradingPhase::Option::IntradayAuction,
                                 .status = TradingStatus::Option::Resume,
                                 .in_auction_phase = true,
                                 .in_auction_call = true,
                                 .in_auction_uncross = false},
           PhaseHandlerQueryCase{.name = "IntradayAuctionUncross",
                                 .phase = TradingPhase::Option::IntradayAuction,
                                 .status = TradingStatus::Option::Halt,
                                 .in_auction_phase = true,
                                 .in_auction_call = false,
                                 .in_auction_uncross = true},
           PhaseHandlerQueryCase{.name = "ClosingAuctionCall",
                                 .phase = TradingPhase::Option::ClosingAuction,
                                 .status = TradingStatus::Option::Resume,
                                 .in_auction_phase = true,
                                 .in_auction_call = true,
                                 .in_auction_uncross = false},
           PhaseHandlerQueryCase{.name = "ClosingAuctionUncross",
                                 .phase = TradingPhase::Option::ClosingAuction,
                                 .status = TradingStatus::Option::Halt,
                                 .in_auction_phase = true,
                                 .in_auction_call = false,
                                 .in_auction_uncross = true},
           PhaseHandlerQueryCase{.name = "OpenResume",
                                 .phase = TradingPhase::Option::Open,
                                 .status = TradingStatus::Option::Resume,
                                 .in_auction_phase = false,
                                 .in_auction_call = false,
                                 .in_auction_uncross = false},
           PhaseHandlerQueryCase{.name = "OpenHalt",
                                 .phase = TradingPhase::Option::Open,
                                 .status = TradingStatus::Option::Halt,
                                 .in_auction_phase = false,
                                 .in_auction_call = false,
                                 .in_auction_uncross = false},
           PhaseHandlerQueryCase{.name = "ClosedHalt",
                                 .phase = TradingPhase::Option::Closed,
                                 .status = TradingStatus::Option::Halt,
                                 .in_auction_phase = false,
                                 .in_auction_call = false,
                                 .in_auction_uncross = false}),
    [](const TestParamInfo<PhaseHandlerQueryCase>& param_info) {
      return param_info.param.name;
    });

struct MatchingEnginePhaseHandlerTransition : public Test {
  NiceMock<EventListenerMock> event_listener;
  PhaseHandler phase_handler{event_listener};

  auto handle(TradingPhase::Option phase,
              TradingStatus::Option status) -> bool {
    return phase_handler.handle(event::PhaseTransition{
        .tz_time_point = core::tz_us{},
        .phase = Phase{phase, status, Phase::Settings{}}});
  }
};

TEST_F(MatchingEnginePhaseHandlerTransition, ReportsPhaseChangedOnNewState) {
  ASSERT_TRUE(handle(TradingPhase::Option::OpeningAuction,
                     TradingStatus::Option::Resume));
}

TEST_F(MatchingEnginePhaseHandlerTransition,
       ReportsPhaseUnchangedOnRedeliveredState) {
  handle(TradingPhase::Option::OpeningAuction, TradingStatus::Option::Resume);

  ASSERT_FALSE(handle(TradingPhase::Option::OpeningAuction,
                      TradingStatus::Option::Resume));
}

TEST_F(MatchingEnginePhaseHandlerTransition,
       ReportsPhaseChangedOnAuctionCallToUncrossEdge) {
  handle(TradingPhase::Option::OpeningAuction, TradingStatus::Option::Resume);

  ASSERT_TRUE(handle(TradingPhase::Option::OpeningAuction,
                     TradingStatus::Option::Halt));
}

TEST_F(MatchingEnginePhaseHandlerTransition,
       CurrentPhaseReflectsLastTransition) {
  handle(TradingPhase::Option::IntradayAuction, TradingStatus::Option::Halt);

  const MarketPhase expected{TradingPhase::Option::IntradayAuction,
                             TradingStatus::Option::Halt};
  ASSERT_EQ(phase_handler.current_phase(), expected);
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
