#include <gmock/gmock.h>

#include <chrono>
#include <optional>

#include "common/events.hpp"
#include "core/tools/time.hpp"
#include "ih/common/data/market_data_updates.hpp"
#include "ih/orders/actions/early_price_reporter.hpp"
#include "ih/orders/matchers/auction_price_calculator.hpp"
#include "ih/orders/phase_handler.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineEarlyPriceReporter : public Test {
  constexpr static core::sys_us BaseTime{std::chrono::sys_days{
      std::chrono::year{2026} / std::chrono::January / 1}};

  MatchingEngineEarlyPriceReporter() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  auto enter_auction_call() -> void {
    transition(TradingPhase::Option::OpeningAuction,
               TradingStatus::Option::Resume);
  }

  auto enter_auction_uncross() -> void {
    transition(TradingPhase::Option::OpeningAuction,
               TradingStatus::Option::Halt);
  }

  auto enter_open() -> void {
    transition(TradingPhase::Option::Open, TradingStatus::Option::Resume);
  }

  auto report_at(std::chrono::seconds elapsed) -> void {
    report_at(elapsed, crossed_result());
  }

  auto report_at(std::chrono::seconds elapsed,
                 std::optional<AuctionResult> result) -> void {
    reporter(event::Tick{.sys_tick_time = BaseTime + elapsed,
                         .tz_tick_time = core::tz_us{}},
             std::move(result));
  }

  static auto crossed_result() -> AuctionResult {
    return {.price = Price{100},
            .quantity = Quantity{50},
            .imbalance = Quantity{10},
            .imbalance_side = TradeCondition::Option::ImbalanceMoreBuyers};
  }

  static auto ReportsEarly(Price price, Quantity quantity) {
    return IsOrderBookNotification(VariantWith<EarlyPriceUpdate>(
        Field(&EarlyPriceUpdate::early_value,
              Optional(AllOf(Field(&TradeResult::price, Eq(price)),
                             Field(&TradeResult::quantity, Eq(quantity)))))));
  }

  static auto ClearsEarlyPrice() {
    return IsOrderBookNotification(VariantWith<EarlyPriceUpdate>(
        Field(&EarlyPriceUpdate::early_value, Eq(std::nullopt))));
  }

  static auto ReportsAnyEarlyPrice() {
    return IsOrderBookNotification(VariantWith<EarlyPriceUpdate>(
        Field(&EarlyPriceUpdate::early_value, Optional(_))));
  }

  static auto EmitsAnyEarlyUpdate() {
    return IsOrderBookNotification(VariantWith<EarlyPriceUpdate>(_));
  }

  NiceMock<EventListenerMock> event_listener;
  PhaseHandler phase_handler{event_listener};
  EarlyPriceReporter reporter{event_listener, phase_handler};

 private:
  auto transition(TradingPhase::Option phase, TradingStatus::Option status)
      -> void {
    phase_handler.handle(event::PhaseTransition{
        .tz_time_point = core::tz_us{},
        .phase = Phase{phase, status, Phase::Settings{}}});
  }
};

TEST_F(MatchingEngineEarlyPriceReporter, DoesNotReportOutsideTheAuctionCall) {
  enter_open();

  EXPECT_CALL(event_listener, on(EmitsAnyEarlyUpdate())).Times(0);

  report_at(std::chrono::seconds{0});
  report_at(std::chrono::seconds{30});
}

TEST_F(MatchingEngineEarlyPriceReporter, DoesNotReportOnFirstTick) {
  enter_auction_call();

  EXPECT_CALL(event_listener, on(EmitsAnyEarlyUpdate())).Times(0);

  report_at(std::chrono::seconds{0});
}

TEST_F(MatchingEngineEarlyPriceReporter,
       DoesNotReportBeforeThe30sIntervalElapses) {
  enter_auction_call();
  report_at(std::chrono::seconds{0});

  EXPECT_CALL(event_listener, on(EmitsAnyEarlyUpdate())).Times(0);

  report_at(std::chrono::seconds{29});
}

TEST_F(MatchingEngineEarlyPriceReporter,
       DoesNotReportDuringTheAuctionUncrossing) {
  enter_auction_uncross();
  report_at(std::chrono::seconds{0});

  EXPECT_CALL(event_listener, on(EmitsAnyEarlyUpdate())).Times(0);

  report_at(std::chrono::seconds{30});
}

TEST_F(MatchingEngineEarlyPriceReporter,
       DoesNotReportWhenThereIsNoAuctionResult) {
  enter_auction_call();
  report_at(std::chrono::seconds{0}, std::nullopt);

  EXPECT_CALL(event_listener, on(EmitsAnyEarlyUpdate())).Times(0);

  report_at(std::chrono::seconds{30}, std::nullopt);
}

TEST_F(MatchingEngineEarlyPriceReporter,
       ReportsEarlyPriceAndQuantityWhenThe30sIntervalElapses) {
  enter_auction_call();
  report_at(std::chrono::seconds{0});

  EXPECT_CALL(event_listener, on(ReportsEarly(Price{100}, Quantity{50})));

  report_at(std::chrono::seconds{30});
}

TEST_F(MatchingEngineEarlyPriceReporter,
       RestartsThe30sIntervalFromTheLastReportedTick) {
  enter_auction_call();
  report_at(std::chrono::seconds{0});
  report_at(std::chrono::seconds{30});

  EXPECT_CALL(event_listener, on(EmitsAnyEarlyUpdate())).Times(0);

  report_at(std::chrono::seconds{59});
}

TEST_F(MatchingEngineEarlyPriceReporter, ReportsOnEveryElapsed30sInterval) {
  enter_auction_call();
  report_at(std::chrono::seconds{0});

  EXPECT_CALL(event_listener, on(ReportsAnyEarlyPrice())).Times(2);

  report_at(std::chrono::seconds{30});
  report_at(std::chrono::seconds{60});
}

TEST_F(MatchingEngineEarlyPriceReporter,
       RestartsThe30sIntervalWhenTheAuctionCallResumes) {
  enter_auction_call();
  report_at(std::chrono::seconds{0});
  enter_open();
  report_at(std::chrono::seconds{30});
  enter_auction_call();

  EXPECT_CALL(event_listener, on(EmitsAnyEarlyUpdate())).Times(0);

  report_at(std::chrono::seconds{31});
}

TEST_F(MatchingEngineEarlyPriceReporter, ReportsClearedEarlyPrice) {
  EXPECT_CALL(event_listener, on(ClearsEarlyPrice()));

  reporter.report_cleared();
}

TEST_F(MatchingEngineEarlyPriceReporter,
       RestartsThe30sIntervalWhenReportingCleared) {
  enter_auction_call();
  report_at(std::chrono::seconds{0});
  reporter.report_cleared();

  EXPECT_CALL(event_listener, on(ReportsAnyEarlyPrice())).Times(0);

  report_at(std::chrono::seconds{31});
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
