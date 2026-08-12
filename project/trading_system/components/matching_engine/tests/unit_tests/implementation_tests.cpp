#include <gmock/gmock.h>

#include <memory>

#include "common/events.hpp"
#include "common/instrument.hpp"
#include "core/tools/time.hpp"
#include "ih/implementation.hpp"
#include "matching_engine/configuration.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/instrument_state_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status.hpp"
#include "protocol/app/security_status_request.hpp"
#include "tests/mocks/trading_reply_receiver_mock.hpp"
#include "tools/protocol_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineImplementation : public Test {
  static auto make_instrument() -> Instrument {
    Instrument instrument;
    instrument.symbol = Symbol{"AAPL"};
    return instrument;
  }

  static auto make_configuration() -> Configuration {
    return {.clock = core::TzClock{"Europe/Kyiv"}};
  }

  static auto transition(TradingPhase::Option phase,
                         TradingStatus::Option status)
      -> event::PhaseTransition {
    return {.tz_time_point = core::tz_us{},
            .phase = Phase{phase, status, Phase::Settings{}}};
  }

  auto place_limit(Side side,
                   OrderPrice price,
                   OrderQuantity quantity) -> void {
    auto request = make_message<protocol::OrderPlacementRequest>();
    request.order_type = OrderType::Option::Limit;
    request.side = side;
    request.order_price = price;
    request.order_quantity = quantity;
    implementation.dispatch_order_cmd(std::move(request));
  }

  auto subscribe_to_security_status() -> void {
    auto request = make_message<protocol::SecurityStatusRequest>();
    request.request_id = SecurityStatusReqId{"status-request"};
    request.request_type = MdSubscriptionRequestType::Option::Subscribe;
    implementation.dispatch_order_cmd(std::move(request));
  }

  auto enter_auction_call_with_crossed_book() -> void {
    implementation.dispatch_phase_transition_cmd(transition(
        TradingPhase::Option::OpeningAuction, TradingStatus::Option::Resume));
    place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
    place_limit(Side::Option::Buy, OrderPrice{110}, OrderQuantity{100});
  }

  auto enter_uncrossing() -> void {
    implementation.dispatch_phase_transition_cmd(transition(
        TradingPhase::Option::OpeningAuction, TradingStatus::Option::Halt));
  }

  static auto IsHaltStatusReport() {
    return MatcherCast<protocol::SecurityStatus>(
        Field(&protocol::SecurityStatus::trading_status,
              Optional(Eq(TradingStatus::Option::Halt))));
  }

  NiceMock<TradingReplyReceiverMock> trading_reply_receiver;
  MatchingEngine::Implementation implementation{make_instrument(),
                                                make_configuration()};

 private:
  auto SetUp() -> void override {
    middleware::bind_trading_reply_channel(
        std::shared_ptr<middleware::TradingReplyReceiver>{
            std::addressof(trading_reply_receiver), [](auto* /*pointer*/) {}});
  }

  auto TearDown() -> void override {
    middleware::release_trading_reply_channel();
  }
};

TEST_F(MatchingEngineImplementation,
       DeliversHaltStatusReportBeforeUncrossExecutionReports) {
  enter_auction_call_with_crossed_book();
  subscribe_to_security_status();

  InSequence sequence;
  EXPECT_CALL(trading_reply_receiver, process(IsHaltStatusReport()));
  EXPECT_CALL(trading_reply_receiver, process(A<protocol::ExecutionReport>()))
      .Times(AtLeast(1));

  enter_uncrossing();
}

TEST_F(MatchingEngineImplementation,
       DeliversHaltStatusReportWhilePreUncrossBookStillStands) {
  enter_auction_call_with_crossed_book();
  subscribe_to_security_status();

  protocol::InstrumentState book_at_halt_report;
  EXPECT_CALL(trading_reply_receiver, process(IsHaltStatusReport()))
      .WillOnce(InvokeWithoutArgs([&] {
        implementation.dispatch_instrument_state_capture_cmd(
            book_at_halt_report);
      }));

  enter_uncrossing();

  EXPECT_THAT(book_at_halt_report.current_bid_depth,
              Optional(Eq(CurrentBidDepth{1})));
  EXPECT_THAT(book_at_halt_report.current_offer_depth,
              Optional(Eq(CurrentOfferDepth{1})));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
