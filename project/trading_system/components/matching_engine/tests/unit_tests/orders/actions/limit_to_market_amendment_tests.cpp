#include <gmock/gmock.h>

#include <optional>
#include <utility>

#include "ih/orders/actions/limit_to_market_amendment.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/mocks/regular_matcher_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

[[nodiscard]]
auto AnyModificationConfirmation() {
  return IsClientNotification(
      VariantWith<protocol::OrderModificationConfirmation>(_));
}

struct MatchingEngineLimitToMarketAmendment : public Test {
  MatchingEngineLimitToMarketAmendment() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
    ON_CALL(matcher, has_facing_orders(A<const MarketOrder&>()))
        .WillByDefault(Return(true));
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;
  NiceMock<RegularMatcherMock> matcher;
  LimitToMarketAmendment amendment{
      event_listener, order_book, matcher, std::nullopt};

  auto rest_limit_order(OrderQuantity quantity) -> void {
    order_book.take_page(Side::Option::Buy)
        .limit_orders()
        .emplace(OrderBuilder{}
                     .with_order_id(OrderId{42})
                     .with_side(Side::Option::Buy)
                     .with_client_order_id(ClientOrderId{"original"})
                     .with_order_price(OrderPrice{10})
                     .with_order_quantity(quantity)
                     .with_time_in_force(TimeInForce::Option::GoodTillCancel)
                     .build_limit_order());
  }

  auto resting_limit_orders() -> LimitOrdersContainer& {
    return order_book.buy_page().limit_orders();
  }

  static auto amend_to_market(OrderQuantity quantity) -> MarketUpdate {
    OrderAttributes attributes;
    attributes.set_client_order_id(ClientOrderId{"amended"});
    attributes.set_time_in_force(TimeInForce::Option::ImmediateOrCancel);
    MarketUpdate update{
        protocol::Session{protocol::generator::Session{}},
        Side::Option::Buy,
        MarketOrder::Update{.quantity = quantity,
                            .attributes = std::move(attributes)}};
    update.order_id = OrderId{42};
    update.orig_client_order_id = OrigClientOrderId{"original"};
    return update;
  }

  static auto amend_to_market_with_time_in_force(OrderQuantity quantity)
      -> MarketUpdate {
    auto update = amend_to_market(quantity);
    update.time_in_force_specified = true;
    return update;
  }
};

TEST_F(MatchingEngineLimitToMarketAmendment, DoesNotExecuteRejectedAmendment) {
  rest_limit_order(OrderQuantity{100});

  EXPECT_CALL(matcher, has_facing_orders(A<const MarketOrder&>())).Times(0);
  EXPECT_CALL(matcher, match(A<MarketOrder&>())).Times(0);

  amendment(amend_to_market_with_time_in_force(OrderQuantity{100}));
}

TEST_F(MatchingEngineLimitToMarketAmendment, RemovesLimitOrderFromBook) {
  rest_limit_order(OrderQuantity{100});

  amendment(amend_to_market(OrderQuantity{100}));

  EXPECT_THAT(resting_limit_orders(), IsEmpty());
}

TEST_F(MatchingEngineLimitToMarketAmendment,
       MatchesMarketOrderAfterConfirmingAmendment) {
  rest_limit_order(OrderQuantity{100});

  InSequence sequence;
  EXPECT_CALL(event_listener, on(AnyModificationConfirmation()));
  EXPECT_CALL(matcher,
              match(Matcher<MarketOrder&>(Property(&MarketOrder::total_quantity,
                                                   Eq(OrderQuantity{80})))));

  amendment(amend_to_market(OrderQuantity{80}));
}

struct MatchingEngineLimitToMarketAmendmentWithoutFacingOrders
    : public MatchingEngineLimitToMarketAmendment {
  MatchingEngineLimitToMarketAmendmentWithoutFacingOrders() {
    ON_CALL(matcher, has_facing_orders(A<const MarketOrder&>()))
        .WillByDefault(Return(false));
  }
};

TEST_F(MatchingEngineLimitToMarketAmendmentWithoutFacingOrders,
       CancelsMarketOrderAfterConfirmingAmendment) {
  rest_limit_order(OrderQuantity{100});

  InSequence sequence;
  EXPECT_CALL(event_listener, on(AnyModificationConfirmation()));
  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(AllOf(
          IsOrderCancellationConfirmation(VenueOrderId{"42"},
                                          OrderStatus::Option::Cancelled,
                                          LeavesQuantity{0},
                                          ClientOrderId{"amended"}),
          VariantWith<protocol::OrderCancellationConfirmation>(Field(
              &protocol::OrderCancellationConfirmation::cancellation_text,
              Optional(Eq(CancellationText{"no facing orders found"}))))))));

  amendment(amend_to_market(OrderQuantity{100}));
}

TEST_F(MatchingEngineLimitToMarketAmendmentWithoutFacingOrders,
       DoesNotMatchMarketOrder) {
  rest_limit_order(OrderQuantity{100});

  EXPECT_CALL(matcher, match(A<MarketOrder&>())).Times(0);

  amendment(amend_to_market(OrderQuantity{100}));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
