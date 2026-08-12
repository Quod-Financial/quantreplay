#include <gmock/gmock.h>

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "core/tools/time.hpp"
#include "ih/orders/actions/regular_amendment.hpp"
#include "ih/orders/actions/trade_at_last_actions.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "ih/orders/matchers/no_cross_matcher.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

[[nodiscard]]
auto RejectedWith(std::string_view reason) {
  return IsClientNotification(VariantWith<protocol::OrderModificationReject>(
      Field(&protocol::OrderModificationReject::reject_text,
            Optional(Eq(RejectText{std::string{reason}})))));
}

struct MatchingEngineTradeAtLastAmendment : public Test {
  MatchingEngineTradeAtLastAmendment() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  constexpr static Price ClosingPrice{42};

  static auto arrival(int seconds_since_epoch) -> OrderTime {
    return OrderTime{core::sys_us{std::chrono::seconds{seconds_since_epoch}}};
  }

  auto rest_trade_at_last_order(OrderId identifier,
                                OrderQuantity quantity = OrderQuantity{100},
                                OrderTime time = arrival(1)) -> void {
    trade_at_last_orders().emplace(
        OrderBuilder{}
            .with_order_id(identifier)
            .with_side(Side::Option::Buy)
            .with_order_price(OrderPrice{ClosingPrice.value()})
            .with_order_quantity(quantity)
            .with_order_time(time)
            .build_limit_order());
  }

  auto trade_at_last_orders() -> LimitOrdersContainer& {
    return order_book.take_page(Side::Option::Buy).trade_at_last_orders();
  }

  static auto amend_to(OrderId identifier,
                       OrderQuantity quantity,
                       OrderPrice price = OrderPrice{ClosingPrice.value()},
                       TimeInForce time_in_force = TimeInForce::Option::Day)
      -> LimitUpdate {
    OrderAttributes attributes;
    attributes.set_time_in_force(time_in_force);
    LimitUpdate update{protocol::Session{protocol::generator::Session{}},
                       Side::Option::Buy,
                       LimitOrder::Update{.price = price,
                                          .quantity = quantity,
                                          .attributes = std::move(attributes)}};
    update.order_id = identifier;
    return update;
  }

  static auto amend_priced_to(OrderId identifier,
                              OrderPrice price) -> LimitUpdate {
    return amend_to(identifier, OrderQuantity{100}, price);
  }

  static auto amend_with_time_in_force(
      OrderId identifier, TimeInForce time_in_force) -> LimitUpdate {
    return amend_to(identifier,
                    OrderQuantity{100},
                    OrderPrice{ClosingPrice.value()},
                    time_in_force);
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;
  NoCrossMatcher matcher;
  RegularAmendment regular_amendment{event_listener,
                                     order_book,
                                     matcher,
                                     std::nullopt,
                                     LimitOrderQueue::TradeAtLast};
};

struct MatchingEngineTradeAtLastAmendmentWithClosingPrice
    : public MatchingEngineTradeAtLastAmendment {
  TradeAtLastAmendment amendment{
      event_listener, regular_amendment, ClosingPrice};
};

TEST_F(MatchingEngineTradeAtLastAmendmentWithClosingPrice,
       RejectsAmendmentPricedAwayFromTheClosingPrice) {
  rest_trade_at_last_order(OrderId{1});

  EXPECT_CALL(event_listener,
              on(RejectedWith(trade_at_last::PriceNotAtClosingPriceReject)));

  amendment(amend_priced_to(OrderId{1}, OrderPrice{43}));
}

TEST_F(MatchingEngineTradeAtLastAmendmentWithClosingPrice,
       RejectsAmendmentThatChangesTheTimeInForce) {
  rest_trade_at_last_order(OrderId{1});

  EXPECT_CALL(event_listener,
              on(RejectedWith(trade_at_last::DayOrdersOnlyReject)));

  amendment(amend_with_time_in_force(OrderId{1},
                                     TimeInForce::Option::GoodTillCancel));
}

TEST_F(MatchingEngineTradeAtLastAmendmentWithClosingPrice,
       LeavesTheOrderUntouchedWhenTheAmendmentIsRejected) {
  rest_trade_at_last_order(OrderId{1}, OrderQuantity{100});

  amendment(amend_priced_to(OrderId{1}, OrderPrice{43}));

  EXPECT_THAT(
      trade_at_last_orders(),
      ElementsAre(AllOf(
          Property(&LimitOrder::price, Eq(OrderPrice{ClosingPrice.value()})),
          Property(&LimitOrder::total_quantity, Eq(OrderQuantity{100})))));
}

TEST_F(MatchingEngineTradeAtLastAmendmentWithClosingPrice,
       ConfirmsAmendmentOfARestingTradeAtLastOrder) {
  rest_trade_at_last_order(OrderId{1});

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderModificationConfirmation>(_))));

  amendment(amend_to(OrderId{1}, OrderQuantity{50}));
}

TEST_F(MatchingEngineTradeAtLastAmendmentWithClosingPrice,
       AppliesTheAmendedQuantityToTheRestingOrder) {
  rest_trade_at_last_order(OrderId{1}, OrderQuantity{100});

  amendment(amend_to(OrderId{1}, OrderQuantity{50}));

  EXPECT_THAT(trade_at_last_orders(),
              ElementsAre(Property(&LimitOrder::total_quantity,
                                   Eq(OrderQuantity{50}))));
}

TEST_F(MatchingEngineTradeAtLastAmendmentWithClosingPrice,
       KeepsTheQueuePositionWhenTheQuantityDecreases) {
  rest_trade_at_last_order(OrderId{1}, OrderQuantity{100}, arrival(1));
  rest_trade_at_last_order(OrderId{2}, OrderQuantity{100}, arrival(2));

  amendment(amend_to(OrderId{1}, OrderQuantity{50}));

  EXPECT_THAT(trade_at_last_orders(),
              ElementsAre(Property(&LimitOrder::id, Eq(OrderId{1})),
                          Property(&LimitOrder::id, Eq(OrderId{2}))));
}

TEST_F(MatchingEngineTradeAtLastAmendmentWithClosingPrice,
       LosesTheQueuePositionWhenTheQuantityIncreases) {
  rest_trade_at_last_order(OrderId{1}, OrderQuantity{100}, arrival(1));
  rest_trade_at_last_order(OrderId{2}, OrderQuantity{100}, arrival(2));

  amendment(amend_to(OrderId{1}, OrderQuantity{200}));

  EXPECT_THAT(trade_at_last_orders(),
              ElementsAre(Property(&LimitOrder::id, Eq(OrderId{2})),
                          Property(&LimitOrder::id, Eq(OrderId{1}))));
}

struct MatchingEngineTradeAtLastAmendmentWithoutClosingPrice
    : public MatchingEngineTradeAtLastAmendment {
  TradeAtLastAmendment amendment{
      event_listener, regular_amendment, std::nullopt};
};

TEST_F(MatchingEngineTradeAtLastAmendmentWithoutClosingPrice,
       RejectsAmendmentOfARestingTradeAtLastOrder) {
  rest_trade_at_last_order(OrderId{1});

  EXPECT_CALL(event_listener,
              on(RejectedWith(trade_at_last::ClosingPriceUnavailableReject)));

  amendment(amend_to(OrderId{1}, OrderQuantity{50}));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
