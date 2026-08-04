#include <gmock/gmock.h>

#include <chrono>

#include "common/events.hpp"
#include "common/instrument.hpp"
#include "core/tools/time.hpp"
#include "ih/common/data/market_data_updates.hpp"
#include "ih/orders/order_system_facade.hpp"
#include "matching_engine/configuration.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status.hpp"
#include "protocol/app/security_status_request.hpp"
#include "tests/mocks/auction_reference_price_provider_mock.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/protocol_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineOrderSystemFacade : public Test {
  static auto make_instrument() -> Instrument {
    Instrument instrument;
    instrument.symbol = Symbol{"AAPL"};
    return instrument;
  }

  static auto make_configuration() -> Configuration {
    return {.clock = core::TzClock{"Europe/Kyiv"}};
  }

  static auto transition(TradingPhase::Option phase,
                         TradingStatus::Option status,
                         bool allow_cancels = false) -> event::PhaseTransition {
    return {
        .tz_time_point = core::tz_us{},
        .phase = Phase{
            phase, status, Phase::Settings{.allow_cancels = allow_cancels}}};
  }

  auto place_limit(Side side, OrderPrice price, OrderQuantity quantity)
      -> void {
    auto request = make_message<protocol::OrderPlacementRequest>();
    request.order_type = OrderType::Option::Limit;
    request.side = side;
    request.order_price = price;
    request.order_quantity = quantity;
    facade.process(request);
  }

  auto place_identifiable_limit(Side side,
                                ClientOrderId id,
                                OrderPrice price,
                                OrderQuantity quantity) -> void {
    auto request = make_message<protocol::OrderPlacementRequest>();
    request.order_type = OrderType::Option::Limit;
    request.side = side;
    request.order_price = price;
    request.order_quantity = quantity;
    request.client_order_id = id;
    facade.process(request);
  }

  auto place_market(Side side, OrderQuantity quantity) -> void {
    auto request = make_message<protocol::OrderPlacementRequest>();
    request.order_type = OrderType::Option::Market;
    request.side = side;
    request.order_quantity = quantity;
    facade.process(request);
  }

  auto amend(Side side,
             OrigClientOrderId id,
             OrderPrice price,
             OrderQuantity quantity) -> void {
    auto request = make_message<protocol::OrderModificationRequest>();
    request.order_type = OrderType::Option::Limit;
    request.side = side;
    request.order_price = price;
    request.order_quantity = quantity;
    request.orig_client_order_id = id;
    facade.process(request);
  }

  auto cancel(Side side, OrigClientOrderId id) -> void {
    auto request = make_message<protocol::OrderCancellationRequest>();
    request.side = side;
    request.orig_client_order_id = id;
    facade.process(request);
  }

  NiceMock<EventListenerMock> event_listener;
  NiceMock<AuctionReferencePriceProviderMock> reference_price_provider;
  OrderSystemFacade facade = OrderSystemFacade::setup(make_instrument(),
                                                      make_configuration(),
                                                      reference_price_provider,
                                                      event_listener);
};

struct MatchingEngineOrderSystemFacadeUncrossing
    : public MatchingEngineOrderSystemFacade {
  auto enter_uncrossing(bool allow_cancels) -> void {
    facade.handle(transition(TradingPhase::Option::IntradayAuction,
                             TradingStatus::Option::Halt,
                             allow_cancels));
  }
};

TEST_F(MatchingEngineOrderSystemFacadeUncrossing, RejectsOrderPlacement) {
  enter_uncrossing(/*allow_cancels=*/false);

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderPlacementReject>(Field(
          &protocol::OrderPlacementReject::reject_text,
          Optional(Eq(RejectText{
              "orders are not allowed during the uncrossing phase"})))))));

  facade.process(make_message<protocol::OrderPlacementRequest>());
}

TEST_F(MatchingEngineOrderSystemFacadeUncrossing, RejectsOrderModification) {
  enter_uncrossing(/*allow_cancels=*/false);

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(
          VariantWith<protocol::OrderModificationReject>(Field(
              &protocol::OrderModificationReject::reject_text,
              Optional(Eq(RejectText{
                  "orders are not allowed during the uncrossing phase"})))))));

  facade.process(make_message<protocol::OrderModificationRequest>());
}

TEST_F(MatchingEngineOrderSystemFacadeUncrossing,
       RejectsOrderCancellationEvenWhenPhaseAllowsCancels) {
  enter_uncrossing(/*allow_cancels=*/true);

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(
          VariantWith<protocol::OrderCancellationReject>(Field(
              &protocol::OrderCancellationReject::reject_text,
              Optional(Eq(RejectText{
                  "orders are not allowed during the uncrossing phase"})))))));

  facade.process(make_message<protocol::OrderCancellationRequest>());
}

struct MatchingEngineOrderSystemFacadeMarketAmendment
    : public MatchingEngineOrderSystemFacade {
  MatchingEngineOrderSystemFacadeMarketAmendment() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  static auto market_modification() -> protocol::OrderModificationRequest {
    auto request = make_message<protocol::OrderModificationRequest>();
    request.order_type = OrderType::Option::Market;
    request.side = Side::Option::Buy;
    request.order_quantity = OrderQuantity{100};
    request.venue_order_id = VenueOrderId{"1"};
    return request;
  }
};

TEST_F(MatchingEngineOrderSystemFacadeMarketAmendment,
       RejectsMarketModificationOutsideAuctionAsUnknownOrderType) {
  facade.handle(
      transition(TradingPhase::Option::Open, TradingStatus::Option::Resume));

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderModificationReject>(
          Field(&protocol::OrderModificationReject::reject_text,
                Optional(Eq(RejectText{"unknown order type"})))))));

  facade.process(market_modification());
}

TEST_F(MatchingEngineOrderSystemFacadeMarketAmendment,
       RoutesMarketModificationToAmendmentDuringCall) {
  facade.handle(transition(TradingPhase::Option::OpeningAuction,
                           TradingStatus::Option::Resume));

  // No resting market order matches, so routing to the market amendment path
  // surfaces "order not found" rather than the "unknown order type" gate.
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderModificationReject>(
          Field(&protocol::OrderModificationReject::reject_text,
                Optional(Eq(RejectText{"order not found"})))))));

  facade.process(market_modification());
}

struct MatchingEngineOrderSystemFacadePlacementRouting
    : public MatchingEngineOrderSystemFacade {
  MatchingEngineOrderSystemFacadePlacementRouting() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  static auto limit_placement(Side side,
                              OrderPrice price,
                              OrderQuantity quantity)
      -> protocol::OrderPlacementRequest {
    auto request = make_message<protocol::OrderPlacementRequest>();
    request.order_type = OrderType::Option::Limit;
    request.side = side;
    request.order_price = price;
    request.order_quantity = quantity;
    return request;
  }
};

TEST_F(MatchingEngineOrderSystemFacadePlacementRouting,
       RestsCrossingLimitOrdersWithoutTradingDuringAuctionCall) {
  facade.handle(transition(TradingPhase::Option::OpeningAuction,
                           TradingStatus::Option::Resume));

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);

  facade.process(
      limit_placement(Side::Option::Sell, OrderPrice{40}, OrderQuantity{100}));
  facade.process(
      limit_placement(Side::Option::Buy, OrderPrice{50}, OrderQuantity{100}));
}

TEST_F(MatchingEngineOrderSystemFacadePlacementRouting,
       CrossesPlacedLimitOrdersOutsideAuctionCall) {
  facade.handle(
      transition(TradingPhase::Option::Open, TradingStatus::Option::Resume));

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(AtLeast(1));

  facade.process(
      limit_placement(Side::Option::Sell, OrderPrice{40}, OrderQuantity{100}));
  facade.process(
      limit_placement(Side::Option::Buy, OrderPrice{50}, OrderQuantity{100}));
}

struct MatchingEngineOrderSystemFacadeAuctionUncross
    : public MatchingEngineOrderSystemFacade {
  MatchingEngineOrderSystemFacadeAuctionUncross() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  auto enter_call() -> PhaseTransitionOutcome {
    return facade.handle(transition(TradingPhase::Option::OpeningAuction,
                                    TradingStatus::Option::Resume));
  }

  auto enter_uncrossing() -> PhaseTransitionOutcome {
    return facade.handle(transition(TradingPhase::Option::OpeningAuction,
                                    TradingStatus::Option::Halt));
  }

  auto subscribe_to_security_status() -> void {
    auto request = make_message<protocol::SecurityStatusRequest>();
    request.request_id = SecurityStatusReqId{"status-request"};
    request.request_type = MdSubscriptionRequestType::Option::Subscribe;
    facade.process(request);
  }
};

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       UncrossesRestingBookOnCallToUncrossingTransition) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
  place_limit(Side::Option::Buy, OrderPrice{110}, OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(AtLeast(1));

  enter_uncrossing();
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       DoesNotReUncrossOnRedeliveredUncrossingTransition) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{5});
  place_limit(Side::Option::Sell, OrderPrice{108}, OrderQuantity{5});
  place_limit(Side::Option::Buy, OrderPrice{110}, OrderQuantity{100});
  place_limit(Side::Option::Buy, OrderPrice{109}, OrderQuantity{100});
  enter_uncrossing();

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);

  EXPECT_EQ(enter_uncrossing(), PhaseTransitionOutcome::Regular);
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       FlushesHaltStatusReportToClientsBeforeUncrossTrades) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
  place_limit(Side::Option::Buy, OrderPrice{110}, OrderQuantity{100});
  subscribe_to_security_status();

  InSequence sequence;
  EXPECT_CALL(event_listener,
              on(IsClientNotification(VariantWith<protocol::SecurityStatus>(
                  Field(&protocol::SecurityStatus::trading_status,
                        Optional(Eq(TradingStatus::Option::Halt)))))));
  EXPECT_CALL(event_listener, on(IsClientNotificationFlush()));
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(AtLeast(1));

  enter_uncrossing();
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       FlushesClientNotificationsEvenWhenNoOrdersCross) {
  enter_call();
  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{100});

  EXPECT_CALL(event_listener, on(IsClientNotificationFlush()));

  enter_uncrossing();
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       DoesNotReFlushOnRedeliveredUncrossingTransition) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
  place_limit(Side::Option::Buy, OrderPrice{110}, OrderQuantity{100});
  enter_uncrossing();

  EXPECT_CALL(event_listener, on(IsClientNotificationFlush())).Times(0);

  enter_uncrossing();
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       SignalsUncrossWhenOrdersCross) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
  place_limit(Side::Option::Buy, OrderPrice{110}, OrderQuantity{100});

  EXPECT_EQ(enter_uncrossing(), PhaseTransitionOutcome::AuctionUncross);
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       SignalsUncrossEvenWhenNoOrdersCross) {
  enter_call();
  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);

  EXPECT_EQ(enter_uncrossing(), PhaseTransitionOutcome::AuctionUncross);
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       DoesNotSignalUncrossOnAuctionCallEntry) {
  EXPECT_EQ(enter_call(), PhaseTransitionOutcome::Regular);
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       DoesNotSignalUncrossOnClosedPhaseTransition) {
  const auto outcome = facade.handle(
      transition(TradingPhase::Option::Closed, TradingStatus::Option::Halt));

  EXPECT_EQ(outcome, PhaseTransitionOutcome::Regular);
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       DoesNotCarryAuctionResultOverIntoANewAuctionCallSession) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{150});
  // Fully fills the sell order, leaving 50 of the buy order resting.
  enter_uncrossing();

  enter_call();
  // Alone with the 50 resting from the prior session, this crosses for 30 -
  // only a calculator still carrying the prior session's cumulative 150/100
  // buy/sell quantities would report anything larger.
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{30});

  EXPECT_CALL(
      event_listener,
      on(IsOrderBookNotification(VariantWith<AuctionFinalPriceUpdate>(
          Field(&AuctionFinalPriceUpdate::clearing_value,
                Optional(Field(&TradeResult::quantity, Eq(Quantity{30}))))))));

  enter_uncrossing();
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       UncrossReflectsAnOrderAmendedDuringTheCall) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{60});
  place_identifiable_limit(Side::Option::Buy,
                           ClientOrderId{"bid"},
                           OrderPrice{110},
                           OrderQuantity{100});

  // Reducing the buy side to 50 (same price) must lower the clearing
  // quantity/price accordingly (50@100, not the pre-amendment 60@110) - the
  // real cross would still execute the real resting 50 either way, so this
  // can only be observed through the reported clearing values.
  amend(Side::Option::Buy,
        OrigClientOrderId{"bid"},
        OrderPrice{110},
        OrderQuantity{50});

  EXPECT_CALL(
      event_listener,
      on(IsOrderBookNotification(VariantWith<AuctionFinalPriceUpdate>(Field(
          &AuctionFinalPriceUpdate::clearing_value,
          Optional(AllOf(Field(&TradeResult::price, Eq(Price{100})),
                         Field(&TradeResult::quantity, Eq(Quantity{50})))))))));

  enter_uncrossing();
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       UncrossReflectsAnOrderCancelledDuringTheCall) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{60});
  place_identifiable_limit(Side::Option::Sell,
                           ClientOrderId{"ask"},
                           OrderPrice{100},
                           OrderQuantity{40});
  place_limit(Side::Option::Buy, OrderPrice{110}, OrderQuantity{100});

  // Cancelling the sell side down to 60 must be reflected in the clearing
  // quantity (60, not the pre-cancellation 100) - the actual cross would still
  // execute the real resting 60 either way, so this can only be observed
  // through the reported clearing quantity, not through trade presence.
  cancel(Side::Option::Sell, OrigClientOrderId{"ask"});

  EXPECT_CALL(
      event_listener,
      on(IsOrderBookNotification(VariantWith<AuctionFinalPriceUpdate>(
          Field(&AuctionFinalPriceUpdate::clearing_value,
                Optional(Field(&TradeResult::quantity, Eq(Quantity{60}))))))));

  enter_uncrossing();
}

TEST_F(MatchingEngineOrderSystemFacadeAuctionUncross,
       IncludesMarketOrderQuantityPlacedDuringTheCallInTheClearingQuantity) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{50});
  place_market(Side::Option::Buy, OrderQuantity{30});

  EXPECT_CALL(
      event_listener,
      on(IsOrderBookNotification(VariantWith<AuctionFinalPriceUpdate>(
          Field(&AuctionFinalPriceUpdate::clearing_value,
                Optional(Field(&TradeResult::quantity, Eq(Quantity{80}))))))));

  enter_uncrossing();
}

struct MatchingEngineOrderSystemFacadeEarlyPrice
    : public MatchingEngineOrderSystemFacade {
  MatchingEngineOrderSystemFacadeEarlyPrice() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  auto enter_call() -> void {
    facade.handle(transition(TradingPhase::Option::OpeningAuction,
                             TradingStatus::Option::Resume));
  }

  auto enter_uncrossing() -> void {
    facade.handle(transition(TradingPhase::Option::OpeningAuction,
                             TradingStatus::Option::Halt));
  }

  auto place_crossed_book() -> void {
    place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
    place_limit(Side::Option::Buy, OrderPrice{110}, OrderQuantity{100});
  }

  auto tick_after(std::chrono::seconds elapsed) -> void {
    facade.handle(event::Tick{.sys_tick_time = epoch_ + elapsed,
                              .tz_tick_time = core::tz_us{}});
  }

  static auto PublishesEarly(Price price, Quantity quantity) {
    return IsOrderBookNotification(VariantWith<EarlyPriceUpdate>(
        Field(&EarlyPriceUpdate::early_value,
              Optional(AllOf(Field(&TradeResult::price, Eq(price)),
                             Field(&TradeResult::quantity, Eq(quantity)))))));
  }

  static auto ClearsEarlyPrice() {
    return IsOrderBookNotification(VariantWith<EarlyPriceUpdate>(
        Field(&EarlyPriceUpdate::early_value, Eq(std::nullopt))));
  }

  static auto EmitsAnyEarlyUpdate() {
    return IsOrderBookNotification(VariantWith<EarlyPriceUpdate>(_));
  }

  core::sys_us epoch_;
};

TEST_F(MatchingEngineOrderSystemFacadeEarlyPrice,
       PublishesEarlyPriceAndVolumeOnThirtySecondTimerWhileCrossed) {
  enter_call();
  place_crossed_book();

  EXPECT_CALL(event_listener, on(PublishesEarly(Price{100}, Quantity{100})));

  tick_after(std::chrono::seconds{0});
  tick_after(std::chrono::seconds{30});
}

TEST_F(MatchingEngineOrderSystemFacadeEarlyPrice,
       ClearsEarlyPriceWhenEnteringUncrossing) {
  enter_call();
  place_crossed_book();

  EXPECT_CALL(event_listener, on(ClearsEarlyPrice()));

  enter_uncrossing();
}

TEST_F(MatchingEngineOrderSystemFacadeEarlyPrice,
       KeepsLastKnownEarlyPriceWhenTheBookDecrossesMidCall) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
  place_identifiable_limit(Side::Option::Buy,
                           ClientOrderId{"bid"},
                           OrderPrice{110},
                           OrderQuantity{100});

  tick_after(std::chrono::seconds{0});
  tick_after(std::chrono::seconds{30});

  cancel(Side::Option::Buy, OrigClientOrderId{"bid"});

  EXPECT_CALL(event_listener, on(EmitsAnyEarlyUpdate())).Times(0);

  tick_after(std::chrono::seconds{60});
}

struct MatchingEngineOrderSystemFacadeReferencePrice
    : public MatchingEngineOrderSystemFacade {
  MatchingEngineOrderSystemFacadeReferencePrice() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  auto enter_call(TradingPhase::Option phase) -> void {
    facade.handle(transition(phase, TradingStatus::Option::Resume));
  }

  static auto PublishesIndicativePrice(Price price) {
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(Field(
        &AuctionIndicativeUpdate::price_qty,
        Optional(Field(&AuctionIndicativeUpdate::IndicativePriceQuantity::price,
                       Optional(Eq(price)))))));
  }
};

TEST_F(MatchingEngineOrderSystemFacadeReferencePrice,
       UsesClosingPriceAsReferenceForOpeningAuction) {
  EXPECT_CALL(reference_price_provider, closing_price());

  enter_call(TradingPhase::Option::OpeningAuction);
}

TEST_F(MatchingEngineOrderSystemFacadeReferencePrice,
       UsesLastOpenPhaseTradeAsReferenceForClosingAuction) {
  EXPECT_CALL(reference_price_provider, last_open_phase_traded_price());

  enter_call(TradingPhase::Option::ClosingAuction);
}

TEST_F(MatchingEngineOrderSystemFacadeReferencePrice,
       UsesLastOpenPhaseTradeAsReferenceForIntradayAuction) {
  EXPECT_CALL(reference_price_provider, last_open_phase_traded_price());

  enter_call(TradingPhase::Option::IntradayAuction);
}

TEST_F(MatchingEngineOrderSystemFacadeReferencePrice,
       FeedsResolvedReferencePriceToTheAuctionPriceCalculator) {
  ON_CALL(reference_price_provider, closing_price())
      .WillByDefault(Return(Price{110}));
  enter_call(TradingPhase::Option::OpeningAuction);
  place_limit(Side::Option::Buy, OrderPrice{110}, OrderQuantity{100});
  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{50});
  place_limit(Side::Option::Sell, OrderPrice{110}, OrderQuantity{50});

  // Both 100 and 110 clear 100 with opposite imbalances of the same size, so
  // only a calculator that received the reference price picks 110 over the
  // lower candidate it falls back to otherwise.
  EXPECT_CALL(event_listener, on(PublishesIndicativePrice(Price{110})));

  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
}

struct MatchingEngineOrderSystemFacadeIndicativePrice
    : public MatchingEngineOrderSystemFacade {
  MatchingEngineOrderSystemFacadeIndicativePrice() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  auto enter_call(TradingPhase::Option phase =
                      TradingPhase::Option::OpeningAuction) -> void {
    facade.handle(transition(phase, TradingStatus::Option::Resume));
  }

  auto enter_uncrossing() -> void {
    facade.handle(transition(TradingPhase::Option::OpeningAuction,
                             TradingStatus::Option::Halt));
  }

  static auto PublishesIndicative(Price price, Quantity quantity) {
    using PriceQuantity = AuctionIndicativeUpdate::IndicativePriceQuantity;
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(
        Field(&AuctionIndicativeUpdate::price_qty,
              Optional(AllOf(Field(&PriceQuantity::price, Optional(Eq(price))),
                             Field(&PriceQuantity::quantity, Eq(quantity)))))));
  }

  static auto PublishesImbalance(Quantity size, TradeCondition side) {
    using Imbalance = AuctionIndicativeUpdate::Imbalance;
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(
        Field(&AuctionIndicativeUpdate::imbalance,
              Optional(AllOf(Field(&Imbalance::size, Eq(size)),
                             Field(&Imbalance::side, Eq(side)))))));
  }

  static auto PublishesNoIndicativePrice() {
    using PriceQuantity = AuctionIndicativeUpdate::IndicativePriceQuantity;
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(AllOf(
        Field(
            &AuctionIndicativeUpdate::price_qty,
            Optional(AllOf(Field(&PriceQuantity::price, Eq(std::nullopt)),
                           Field(&PriceQuantity::quantity, Eq(Quantity{0}))))),
        Field(&AuctionIndicativeUpdate::imbalance, Eq(std::nullopt)))));
  }

  static auto ClearsIndicativeValues() {
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(
        AllOf(Field(&AuctionIndicativeUpdate::price_qty, Eq(std::nullopt)),
              Field(&AuctionIndicativeUpdate::imbalance, Eq(std::nullopt)))));
  }

  static auto PublishesIndicativeForPhase(TradingPhase phase) {
    return IsOrderBookNotification(VariantWith<AuctionIndicativeUpdate>(
        Field(&AuctionIndicativeUpdate::auction_phase, Eq(phase))));
  }
};

TEST_F(MatchingEngineOrderSystemFacadeIndicativePrice,
       PublishesIndicativePriceAndQuantityOnPlacementDuringTheCall) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(PublishesIndicative(Price{100}, Quantity{100})));

  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{150});
}

TEST_F(MatchingEngineOrderSystemFacadeIndicativePrice,
       PublishesIndicativeQuantityIncludingMarketOrdersPlacedDuringTheCall) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{50});

  EXPECT_CALL(event_listener,
              on(PublishesIndicative(Price{100}, Quantity{80})));

  place_market(Side::Option::Buy, OrderQuantity{30});
}

TEST_F(MatchingEngineOrderSystemFacadeIndicativePrice,
       PublishesIndicativeQuantityOnAmendmentDuringTheCall) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
  place_identifiable_limit(Side::Option::Buy,
                           ClientOrderId{"bid"},
                           OrderPrice{100},
                           OrderQuantity{150});

  EXPECT_CALL(event_listener,
              on(PublishesIndicative(Price{100}, Quantity{60})));

  amend(Side::Option::Buy,
        OrigClientOrderId{"bid"},
        OrderPrice{100},
        OrderQuantity{60});
}

TEST_F(MatchingEngineOrderSystemFacadeIndicativePrice,
       PublishesIndicativeQuantityOnCancellationDuringTheCall) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{40});
  place_identifiable_limit(Side::Option::Buy,
                           ClientOrderId{"bid"},
                           OrderPrice{100},
                           OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(PublishesIndicative(Price{100}, Quantity{40})));

  cancel(Side::Option::Buy, OrigClientOrderId{"bid"});
}

TEST_F(MatchingEngineOrderSystemFacadeIndicativePrice,
       PublishesImbalanceSizeAndSideWhenBuySideIsLarger) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(PublishesImbalance(
                  Quantity{50}, TradeCondition::Option::ImbalanceMoreBuyers)));

  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{150});
}

TEST_F(MatchingEngineOrderSystemFacadeIndicativePrice,
       PublishesImbalanceSizeAndSideWhenSellSideIsLarger) {
  enter_call();
  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(PublishesImbalance(
                  Quantity{50}, TradeCondition::Option::ImbalanceMoreSellers)));

  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{150});
}

TEST_F(MatchingEngineOrderSystemFacadeIndicativePrice,
       PublishesZeroQuantityWithoutPriceAndImbalanceWhenBookDoesNotCross) {
  enter_call();

  EXPECT_CALL(event_listener, on(PublishesNoIndicativePrice()));

  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{100});
}

TEST_F(MatchingEngineOrderSystemFacadeIndicativePrice,
       ReportsTheOngoingAuctionPhaseInTheIndicativeUpdate) {
  enter_call(TradingPhase::Option::ClosingAuction);

  EXPECT_CALL(event_listener,
              on(PublishesIndicativeForPhase(
                  TradingPhase{TradingPhase::Option::ClosingAuction})));

  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{100});
}

TEST_F(MatchingEngineOrderSystemFacadeIndicativePrice,
       ClearsIndicativeValuesWhenEnteringUncrossing) {
  enter_call();
  place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
  place_limit(Side::Option::Buy, OrderPrice{100}, OrderQuantity{150});

  EXPECT_CALL(event_listener, on(ClearsIndicativeValues()));

  enter_uncrossing();
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
