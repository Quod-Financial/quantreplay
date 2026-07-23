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

  NiceMock<EventListenerMock> event_listener;
  OrderSystemFacade facade = OrderSystemFacade::setup(
      make_instrument(), make_configuration(), event_listener);
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

  auto place_limit(Side side,
                   OrderPrice price,
                   OrderQuantity quantity) -> void {
    auto request = make_message<protocol::OrderPlacementRequest>();
    request.order_type = OrderType::Option::Limit;
    request.side = side;
    request.order_price = price;
    request.order_quantity = quantity;
    facade.process(request);
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

  auto enter_open() -> void {
    facade.handle(
        transition(TradingPhase::Option::Open, TradingStatus::Option::Resume));
  }

  auto place_limit(Side side,
                   OrderPrice price,
                   OrderQuantity quantity) -> void {
    auto request = make_message<protocol::OrderPlacementRequest>();
    request.order_type = OrderType::Option::Limit;
    request.side = side;
    request.order_price = price;
    request.order_quantity = quantity;
    facade.process(request);
  }

  auto place_crossed_book() -> void {
    place_limit(Side::Option::Sell, OrderPrice{100}, OrderQuantity{100});
    place_limit(Side::Option::Buy, OrderPrice{110}, OrderQuantity{100});
  }

  auto place_identifiable_buy(ClientOrderId id,
                              OrderPrice price,
                              OrderQuantity quantity) -> void {
    auto request = make_message<protocol::OrderPlacementRequest>();
    request.order_type = OrderType::Option::Limit;
    request.side = Side::Option::Buy;
    request.order_price = price;
    request.order_quantity = quantity;
    request.client_order_id = id;
    facade.process(request);
  }

  auto cancel_buy(OrigClientOrderId id) -> void {
    auto request = make_message<protocol::OrderCancellationRequest>();
    request.side = Side::Option::Buy;
    request.orig_client_order_id = id;
    facade.process(request);
  }

  auto tick_after(std::chrono::seconds elapsed) -> void {
    facade.handle(event::Tick{.sys_tick_time = epoch_ + elapsed,
                              .tz_tick_time = core::tz_us{}});
  }

  static auto PublishesEarly(Price price, Quantity quantity) {
    return IsOrderBookNotification(VariantWith<EarlyPriceUpdate>(AllOf(
        Field(&EarlyPriceUpdate::early_price, Optional(Eq(price))),
        Field(&EarlyPriceUpdate::early_quantity, Optional(Eq(quantity))))));
  }

  static auto PublishesAnyEarlyPrice() {
    return IsOrderBookNotification(VariantWith<EarlyPriceUpdate>(
        Field(&EarlyPriceUpdate::early_price, Optional(_))));
  }

  static auto ClearsEarlyPrice() {
    return IsOrderBookNotification(VariantWith<EarlyPriceUpdate>(
        Field(&EarlyPriceUpdate::early_price, Eq(std::nullopt))));
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

  EXPECT_CALL(event_listener, on(PublishesEarly(Price{105}, Quantity{100})));

  tick_after(std::chrono::seconds{0});
  tick_after(std::chrono::seconds{30});
}

TEST_F(MatchingEngineOrderSystemFacadeEarlyPrice,
       DoesNotPublishEarlyPriceBeforeThirtySecondsElapse) {
  enter_call();
  place_crossed_book();

  EXPECT_CALL(event_listener, on(PublishesAnyEarlyPrice())).Times(0);

  tick_after(std::chrono::seconds{0});
  tick_after(std::chrono::seconds{29});
}

TEST_F(MatchingEngineOrderSystemFacadeEarlyPrice,
       DoesNotPublishEarlyPriceOutsideTheAuctionCall) {
  enter_open();

  EXPECT_CALL(event_listener, on(PublishesAnyEarlyPrice())).Times(0);

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
  place_identifiable_buy(
      ClientOrderId{"bid"}, OrderPrice{110}, OrderQuantity{100});

  tick_after(std::chrono::seconds{0});
  tick_after(std::chrono::seconds{30});

  cancel_buy(OrigClientOrderId{"bid"});

  EXPECT_CALL(event_listener, on(EmitsAnyEarlyUpdate())).Times(0);

  tick_after(std::chrono::seconds{60});
}

TEST_F(MatchingEngineOrderSystemFacadeEarlyPrice,
       ReArmsEarlyTimerForAnImmediatelyFollowingAuction) {
  EXPECT_CALL(event_listener, on(PublishesAnyEarlyPrice())).Times(0);

  enter_call();
  tick_after(std::chrono::seconds{0});  // arms the first auction's timer
  enter_uncrossing();                   // clears and re-arms the timer

  enter_call();
  place_crossed_book();
  tick_after(std::chrono::seconds{31});
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
