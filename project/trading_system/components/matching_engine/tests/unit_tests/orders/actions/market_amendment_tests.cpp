#include <gmock/gmock.h>

#include "ih/orders/actions/market_amendment.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "ih/orders/replies/modification_reply_builders.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineMarketAmendment : public Test {
  MatchingEngineMarketAmendment() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;
  MarketAmendment amendment{event_listener, order_book, std::nullopt};

  auto rest_market_order(OrderId order_id,
                         OrderQuantity quantity,
                         CumExecutedQuantity executed = CumExecutedQuantity{0})
      -> void {
    auto order = OrderBuilder{}
                     .with_order_id(order_id)
                     .with_side(Side::Option::Buy)
                     .with_order_quantity(quantity)
                     .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
                     .build_market_order();
    if (static_cast<double>(executed) > 0.0) {
      order.execute(ExecutedQuantity{static_cast<double>(executed)},
                    ExecutionPrice{10.0});
    }
    order_book.take_page(Side::Option::Buy).market_orders().emplace(order);
  }

  static auto amend_to(OrderId order_id, OrderQuantity quantity)
      -> MarketUpdate {
    OrderAttributes attributes;
    attributes.set_time_in_force(TimeInForce::Option::ImmediateOrCancel);
    MarketUpdate update{
        protocol::Session{protocol::generator::Session{}},
        Side::Option::Buy,
        MarketOrder::Update{.quantity = quantity,
                            .attributes = std::move(attributes)}};
    update.order_id = order_id;
    return update;
  }
};

TEST_F(MatchingEngineMarketAmendment,
       EmitsModificationConfirmationWithMarketOrderType) {
  rest_market_order(OrderId{42}, OrderQuantity{10});

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderModificationConfirmation>(Field(
                      &protocol::OrderModificationConfirmation::order_type,
                      Optional(Eq(OrderType::Option::Market)))))));

  amendment(amend_to(OrderId{42}, OrderQuantity{5}));
}

TEST_F(MatchingEngineMarketAmendment,
       ReRestsAmendedMarketOrderWithNewQuantity) {
  rest_market_order(OrderId{42}, OrderQuantity{10});

  amendment(amend_to(OrderId{42}, OrderQuantity{5}));

  auto& market_orders = order_book.buy_page().market_orders();
  ASSERT_THAT(market_orders.size(), Eq(1));
  ASSERT_THAT(market_orders.begin()->total_quantity(), Eq(OrderQuantity{5}));
}

TEST_F(MatchingEngineMarketAmendment, EmitsOrderRemovedAndAddedWithoutPrice) {
  rest_market_order(OrderId{42}, OrderQuantity{10});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_price, Eq(std::nullopt))))));
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderAdded>(
                  Field(&OrderAdded::order_price, Eq(std::nullopt))))));

  amendment(amend_to(OrderId{42}, OrderQuantity{20}));
}

TEST_F(MatchingEngineMarketAmendment,
       ReturnsRemoveAndAddUpdatesForAmendedQuantity) {
  rest_market_order(OrderId{42}, OrderQuantity{10});

  ASSERT_THAT(
      amendment(amend_to(OrderId{42}, OrderQuantity{20})),
      ElementsAre(OrderBookUpdate{.side = Side::Option::Buy,
                                  .action = OrderBookUpdate::Action::Remove,
                                  .price = std::nullopt,
                                  .quantity = LeavesQuantity{10}},
                  OrderBookUpdate{.side = Side::Option::Buy,
                                  .action = OrderBookUpdate::Action::Add,
                                  .price = std::nullopt,
                                  .quantity = LeavesQuantity{20}}));
}

TEST_F(MatchingEngineMarketAmendment, DoesNotCrossWhenAmending) {
  rest_market_order(OrderId{42}, OrderQuantity{10});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<Trade>(_))))
      .Times(0);

  amendment(amend_to(OrderId{42}, OrderQuantity{5}));
}

TEST_F(MatchingEngineMarketAmendment, RejectsAmendmentWhenOrderNotFound) {
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderModificationReject>(
          Field(&protocol::OrderModificationReject::reject_text,
                Optional(Eq(RejectText{"order not found"})))))));

  amendment(amend_to(OrderId{99}, OrderQuantity{5}));
}

TEST_F(MatchingEngineMarketAmendment, ReturnsNoUpdatesWhenOrderNotFound) {
  ASSERT_THAT(amendment(amend_to(OrderId{99}, OrderQuantity{5})), IsEmpty());
}

TEST_F(MatchingEngineMarketAmendment, RejectsAmendmentWithInvalidQuantity) {
  rest_market_order(OrderId{42}, OrderQuantity{10}, CumExecutedQuantity{5});

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<protocol::OrderModificationReject>(
          Field(&protocol::OrderModificationReject::reject_text,
                Optional(Eq(RejectText{"invalid quantity"})))))));

  amendment(amend_to(OrderId{42}, OrderQuantity{5}));
}

TEST_F(MatchingEngineMarketAmendment, ReturnsNoUpdatesWithInvalidQuantity) {
  rest_market_order(OrderId{42}, OrderQuantity{10}, CumExecutedQuantity{5});

  ASSERT_THAT(amendment(amend_to(OrderId{42}, OrderQuantity{5})), IsEmpty());
}

TEST_F(MatchingEngineMarketAmendment, RejectsAmendmentWhenTimeInForceChanged) {
  rest_market_order(OrderId{42}, OrderQuantity{10});

  OrderAttributes attributes;
  attributes.set_time_in_force(TimeInForce::Option::Day);
  MarketUpdate update{protocol::Session{protocol::generator::Session{}},
                      Side::Option::Buy,
                      MarketOrder::Update{.quantity = OrderQuantity{5},
                                          .attributes = std::move(attributes)}};
  update.order_id = OrderId{42};

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(
          VariantWith<protocol::OrderModificationReject>(Field(
              &protocol::OrderModificationReject::reject_text,
              Optional(Eq(RejectText{"time in force can not be changed"})))))));

  amendment(std::move(update));
}

TEST_F(MatchingEngineMarketAmendment, ReturnsNoUpdatesWhenTimeInForceChanged) {
  rest_market_order(OrderId{42}, OrderQuantity{10});

  OrderAttributes attributes;
  attributes.set_time_in_force(TimeInForce::Option::Day);
  MarketUpdate update{protocol::Session{protocol::generator::Session{}},
                      Side::Option::Buy,
                      MarketOrder::Update{.quantity = OrderQuantity{5},
                                          .attributes = std::move(attributes)}};
  update.order_id = OrderId{42};

  ASSERT_THAT(amendment(std::move(update)), IsEmpty());
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
