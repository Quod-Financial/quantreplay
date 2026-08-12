#include <gmock/gmock.h>

#include <optional>

#include "common/events.hpp"
#include "core/tools/time.hpp"
#include "ih/common/data/market_data_updates.hpp"
#include "ih/orders/actions/auction_indicative_reporter.hpp"
#include "ih/orders/matchers/auction_price_calculator.hpp"
#include "ih/orders/phase_handler.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineAuctionIndicativeReporter : public Test {
  MatchingEngineAuctionIndicativeReporter() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  auto enter_auction_call(TradingPhase::Option phase =
                              TradingPhase::Option::OpeningAuction) -> void {
    transition(phase, TradingStatus::Option::Resume);
  }

  auto enter_auction_uncross() -> void {
    transition(TradingPhase::Option::OpeningAuction,
               TradingStatus::Option::Halt);
  }

  auto enter_open() -> void {
    transition(TradingPhase::Option::Open, TradingStatus::Option::Resume);
  }

  static auto crossed_result() -> AuctionResult {
    return {.price = Price{100},
            .quantity = Quantity{50},
            .imbalance = Quantity{10},
            .imbalance_side = TradeCondition::Option::ImbalanceMoreBuyers};
  }

  static auto ReportsIndicative(Price price, Quantity quantity) {
    using PriceQuantity = AuctionIndicativeUpdate::IndicativePriceQuantity;
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(
        Field(&AuctionIndicativeUpdate::price_qty,
              Optional(AllOf(Field(&PriceQuantity::price, Optional(Eq(price))),
                             Field(&PriceQuantity::quantity, Eq(quantity)))))));
  }

  static auto ReportsImbalance(Quantity size, TradeCondition side) {
    using Imbalance = AuctionIndicativeUpdate::Imbalance;
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(
        Field(&AuctionIndicativeUpdate::imbalance,
              Optional(AllOf(Field(&Imbalance::size, Eq(size)),
                             Field(&Imbalance::side, Eq(side)))))));
  }

  static auto ReportsNoIndicativePrice() {
    using PriceQuantity = AuctionIndicativeUpdate::IndicativePriceQuantity;
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(AllOf(
        Field(
            &AuctionIndicativeUpdate::price_qty,
            Optional(AllOf(Field(&PriceQuantity::price, Eq(std::nullopt)),
                           Field(&PriceQuantity::quantity, Eq(Quantity{0}))))),
        Field(&AuctionIndicativeUpdate::imbalance, Eq(std::nullopt)))));
  }

  static auto ReportsForPhase(TradingPhase phase) {
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(
        Field(&AuctionIndicativeUpdate::auction_phase, Eq(phase))));
  }

  static auto ClearsIndicativeValues() {
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(
        AllOf(Field(&AuctionIndicativeUpdate::price_qty, Eq(std::nullopt)),
              Field(&AuctionIndicativeUpdate::imbalance, Eq(std::nullopt)))));
  }

  static auto EmitsAnyIndicativeUpdate() {
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(_));
  }

  NiceMock<EventListenerMock> event_listener;
  PhaseHandler phase_handler{event_listener};
  AuctionIndicativeReporter reporter{event_listener, phase_handler};

 private:
  auto transition(TradingPhase::Option phase, TradingStatus::Option status)
      -> void {
    phase_handler.handle(event::PhaseTransition{
        .tz_time_point = core::tz_us{},
        .phase = Phase{phase, status, Phase::Settings{}}});
  }
};

TEST_F(MatchingEngineAuctionIndicativeReporter,
       DoesNotReportOutsideTheAuctionCall) {
  enter_open();

  EXPECT_CALL(event_listener, on(EmitsAnyIndicativeUpdate())).Times(0);

  reporter(crossed_result());
}

TEST_F(MatchingEngineAuctionIndicativeReporter,
       DoesNotReportDuringTheAuctionUncrossing) {
  enter_auction_uncross();

  EXPECT_CALL(event_listener, on(EmitsAnyIndicativeUpdate())).Times(0);

  reporter(crossed_result());
}

TEST_F(MatchingEngineAuctionIndicativeReporter,
       ReportsIndicativePriceAndQuantityOfTheAuctionResult) {
  enter_auction_call();

  EXPECT_CALL(event_listener, on(ReportsIndicative(Price{100}, Quantity{50})));

  reporter(crossed_result());
}

TEST_F(MatchingEngineAuctionIndicativeReporter,
       ReportsImbalanceSizeAndSideOfTheAuctionResult) {
  enter_auction_call();

  EXPECT_CALL(event_listener,
              on(ReportsImbalance(
                  Quantity{10}, TradeCondition::Option::ImbalanceMoreBuyers)));

  reporter(crossed_result());
}

TEST_F(MatchingEngineAuctionIndicativeReporter,
       ReportsZeroQuantityWithoutPriceAndImbalanceWithoutAnAuctionResult) {
  enter_auction_call();

  EXPECT_CALL(event_listener, on(ReportsNoIndicativePrice()));

  reporter(std::nullopt);
}

TEST_F(MatchingEngineAuctionIndicativeReporter, ReportsTheOngoingAuctionPhase) {
  enter_auction_call(TradingPhase::Option::ClosingAuction);

  EXPECT_CALL(
      event_listener,
      on(ReportsForPhase(TradingPhase{TradingPhase::Option::ClosingAuction})));

  reporter(crossed_result());
}

TEST_F(MatchingEngineAuctionIndicativeReporter,
       ReportsClearedIndicativeValues) {
  EXPECT_CALL(event_listener, on(ClearsIndicativeValues()));

  reporter.report_cleared();
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
