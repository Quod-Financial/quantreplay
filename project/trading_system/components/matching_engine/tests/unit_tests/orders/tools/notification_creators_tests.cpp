#include <gmock/gmock.h>

#include <vector>

#include "core/domain/attributes.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "tools/order_test_tools.hpp"
#include "tools/protocol_test_tools.hpp"

namespace simulator::trading_system::matching_engine::order::tests {
namespace {

// NOLINTBEGIN(*non-private-members*,*magic-numbers*)

using namespace testing;  // NOLINT

struct OrderBookNotificationOrderAddedCreation : public Test {
  OrderBuilder builder;
};

TEST_F(OrderBookNotificationOrderAddedCreation,
       SetsOrderAddedToOrderBookNofitication) {
  const auto order = builder.build_limit_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  ASSERT_THAT(notification.value, VariantWith<OrderAdded>(_));
}

TEST_F(OrderBookNotificationOrderAddedCreation,
       SetsOrderOwnerEmptyIfPartyIdNotSetInLimitOrder) {
  const auto order = builder.build_limit_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_FALSE(order_added.order_owner.has_value());
}

TEST_F(OrderBookNotificationOrderAddedCreation,
       SetsOrderOwnerEmptyIfPartyIsNotOwnerInLimitOrder) {
  const auto party = Party{PartyId{"Taker"},
                           PartyIdSource::Option::BIC,
                           PartyRole::Option::ClaimingAccount};
  const auto order = builder.with_order_parties({party}).build_limit_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_FALSE(order_added.order_owner.has_value());
}

TEST_F(OrderBookNotificationOrderAddedCreation, SetsOrderOwnerFromPartyId) {
  const auto party = Party{PartyId{"Taker"},
                           PartyIdSource::Option::BIC,
                           PartyRole::Option::ExecutingFirm};
  const auto order = builder.with_order_parties({party}).build_limit_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_EQ(order_added.order_owner, PartyId{"Taker"});
}

TEST_F(OrderBookNotificationOrderAddedCreation, SetsOrderPriceFromLimitOrder) {
  const auto order =
      builder.with_order_price(OrderPrice{3.14}).build_limit_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_EQ(order_added.order_price, Price{3.14});
}

TEST_F(OrderBookNotificationOrderAddedCreation,
       SetsOrderQuantityFromLeavesQuanityOfLimitOrder) {
  auto order =
      builder.with_order_quantity(OrderQuantity{10.5}).build_limit_order();
  order.execute(ExecutedQuantity{7.4});

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_EQ(order_added.order_quantity, Quantity{3.1});
}

TEST_F(OrderBookNotificationOrderAddedCreation, SetsOrderIdFromLimitOrder) {
  const auto order = builder.with_order_id(OrderId{123}).build_limit_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_EQ(order_added.order_id, OrderId{123});
}

TEST_F(OrderBookNotificationOrderAddedCreation, SetsOrderSideFromLimitOrder) {
  const auto order = builder.with_side(Side::Option::Sell).build_limit_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_EQ(order_added.order_side, Side::Option::Sell);
}

struct OrderBookNotificationOrderRemovedCreation : public Test {
  OrderBuilder builder;
};

TEST_F(OrderBookNotificationOrderRemovedCreation,
       SetsOrderRemovedToOrderBookNotification) {
  const auto order = builder.build_limit_order();

  const auto notification =
      make_making_order_removed_from_book_notification(order);
  ASSERT_THAT(notification.value, VariantWith<OrderRemoved>(_));
}

TEST_F(OrderBookNotificationOrderRemovedCreation, SetsOrderIdFromLimitOrder) {
  const auto order = builder.with_order_id(OrderId{123}).build_limit_order();

  const auto notification =
      make_making_order_removed_from_book_notification(order);
  const auto& order_removed = std::get<OrderRemoved>(notification.value);

  ASSERT_EQ(order_removed.order_id, OrderId{123});
}

TEST_F(OrderBookNotificationOrderRemovedCreation, SetsOrderSideFromLimitOrder) {
  const auto order = builder.with_side(Side::Option::Sell).build_limit_order();

  const auto notification =
      make_making_order_removed_from_book_notification(order);
  const auto& order_removed = std::get<OrderRemoved>(notification.value);

  ASSERT_EQ(order_removed.order_side, Side::Option::Sell);
}

struct OrderBookNotificationOrderReducedCreation : public Test {
  OrderBuilder builder;
};

TEST_F(OrderBookNotificationOrderReducedCreation,
       SetsOrderReducedToOrderBookNotification) {
  const auto order = builder.build_limit_order();

  const auto notification = make_making_order_reduced_notification(order);
  ASSERT_THAT(notification.value, VariantWith<OrderReduced>(_));
}

TEST_F(OrderBookNotificationOrderReducedCreation,
       SetsOrderPriceFromLimitOrder) {
  const auto order =
      builder.with_order_price(OrderPrice{3.14}).build_limit_order();
  const auto notification = make_making_order_reduced_notification(order);
  const auto& order_reduced = std::get<OrderReduced>(notification.value);

  ASSERT_EQ(order_reduced.order_price, OrderPrice{3.14});
}

TEST_F(OrderBookNotificationOrderReducedCreation,
       SetsOrderQuantityFromLeavesQuantityOfLimitOrder) {
  auto order =
      builder.with_order_quantity(OrderQuantity{10.5}).build_limit_order();
  order.execute(ExecutedQuantity{7.4});

  const auto notification = make_making_order_reduced_notification(order);
  const auto& order_reduced = std::get<OrderReduced>(notification.value);

  ASSERT_EQ(order_reduced.order_quantity, OrderQuantity{3.1});
}

TEST_F(OrderBookNotificationOrderReducedCreation, SetsOrderIdFromLimitOrder) {
  const auto order = builder.with_order_id(OrderId{123}).build_limit_order();

  const auto notification = make_making_order_reduced_notification(order);
  const auto& order_reduced = std::get<OrderReduced>(notification.value);

  ASSERT_EQ(order_reduced.order_id, OrderId{123});
}

TEST_F(OrderBookNotificationOrderReducedCreation, SetsSideFromLimitOrder) {
  const auto order = builder.with_order_id(OrderId{123}).build_limit_order();

  const auto notification = make_making_order_reduced_notification(order);
  const auto& order_reduced = std::get<OrderReduced>(notification.value);

  ASSERT_EQ(order_reduced.order_id, OrderId{123});
}

struct OrderBookNotificationTradeFromLimitOrdersCreation : public Test {
  const Party taker_party{PartyId{"Taker"},
                          PartyIdSource::Option::BIC,
                          PartyRole::Option::ExecutingFirm};
  const Party maker_party{PartyId{"Maker"},
                          PartyIdSource::Option::FDID,
                          PartyRole::Option::ExecutingFirm};

  OrderBuilder builder;
};

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsTradeToOrderBookNofitication) {
  const auto taker = builder.build_limit_order();
  const auto maker = builder.build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  ASSERT_THAT(notification.value, VariantWith<Trade>(_));
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsBuyerFromPartyIdOfTakerBuySide) {
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.buyer->value(), taker_party.party_id().value());
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsBuyerFromPartyIdOfMakerBuySide) {
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.buyer->value(), maker_party.party_id().value());
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsSellerFromPartyIdOfMakerSellSide) {
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.seller->value(), maker_party.party_id().value());
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsSellerFromPartyIdOfTakerSellSide) {
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.seller->value(), taker_party.party_id().value());
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsNullBuyerIfTakerBuySideDoesNotHaveOwner) {
  const auto taker_party_contra_firm = Party{PartyId{"Taker"},
                                             PartyIdSource::Option::BIC,
                                             PartyRole::Option::ContraFirm};
  const auto taker = builder.with_order_parties({taker_party_contra_firm})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_FALSE(trade.buyer.has_value());
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsNullBuyerIfMakerBuySideDoesNotHaveOwner) {
  const auto maker_party_contra_firm = Party{PartyId{"Maker"},
                                             PartyIdSource::Option::BIC,
                                             PartyRole::Option::ContraFirm};
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();
  const auto maker = builder.with_order_parties({maker_party_contra_firm})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_FALSE(trade.buyer.has_value());
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsNullSellerIfTakerSellSideDoesNotHaveOwner) {
  const auto taker_party_contra_firm = Party{PartyId{"Taker"},
                                             PartyIdSource::Option::BIC,
                                             PartyRole::Option::ContraFirm};
  const auto taker = builder.with_order_parties({taker_party_contra_firm})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_FALSE(trade.seller.has_value());
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsNullSellerIfMakerSellSideDoesNotHaveOwner) {
  const auto maker_party_contra_firm = Party{PartyId{"Maker"},
                                             PartyIdSource::Option::BIC,
                                             PartyRole::Option::ContraFirm};
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();
  const auto maker = builder.with_order_parties({maker_party_contra_firm})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_FALSE(trade.seller.has_value());
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsTradePriceFromExecutionPrice) {
  const auto taker = builder.build_limit_order();
  const auto maker = builder.build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.trade_price, Price{100});
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsTradedQuantityFromExecutedQuantity) {
  const auto taker = builder.build_limit_order();
  const auto maker = builder.build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.traded_quantity, Quantity{50});
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation,
       SetsAggressorSideFromTakerSide) {
  const auto taker = builder.with_side(Side::Option::Buy).build_limit_order();
  const auto maker = builder.with_side(Side::Option::Sell).build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.aggressor_side, AggressorSide::Option::Buy);
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation, SetsTradeTime) {
  const auto taker = builder.build_limit_order();
  const auto maker = builder.build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_GT(trade.trade_time.time_since_epoch().count(), 0);
}

TEST_F(OrderBookNotificationTradeFromLimitOrdersCreation, SetsOpenMarketPhase) {
  const auto taker = builder.build_limit_order();
  const auto maker = builder.build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.market_phase, MarketPhase::open());
}

struct OrderBookNotificationTradeFromMarketLimitOrdersCreation : public Test {
  const Party taker_party{PartyId{"Taker"},
                          PartyIdSource::Option::BIC,
                          PartyRole::Option::ExecutingFirm};
  const Party maker_party{PartyId{"Maker"},
                          PartyIdSource::Option::FDID,
                          PartyRole::Option::ExecutingFirm};

  OrderBuilder builder;
};

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsTradeToOrderBookNofitication) {
  const auto taker = builder.build_market_order();
  const auto maker = builder.build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  ASSERT_THAT(notification.value, VariantWith<Trade>(_));
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsBuyerFromPartyIdOfMarketOrderTakerBuySide) {
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Buy)
                         .build_market_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.buyer->value(), taker_party.party_id().value());
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsBuyerFromPartyIdOfLimitOrderMakerBuySide) {
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Sell)
                         .build_market_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.buyer->value(), maker_party.party_id().value());
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsSellerFromPartyIdOfLimitOrderMakerSellSide) {
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Buy)
                         .build_market_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.seller->value(), maker_party.party_id().value());
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsSellerFromPartyIdOfMarketOrderTakerSellSide) {
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Sell)
                         .build_market_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.seller->value(), taker_party.party_id().value());
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsNullBuyerIfMarketOrderTakerBuySideDoesNotHaveOwner) {
  const auto taker_party_contra_firm = Party{PartyId{"Taker"},
                                             PartyIdSource::Option::BIC,
                                             PartyRole::Option::ContraFirm};
  const auto taker = builder.with_order_parties({taker_party_contra_firm})
                         .with_side(Side::Option::Buy)
                         .build_market_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_FALSE(trade.buyer.has_value());
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsNullBuyerIfLimitOrderMakerBuySideDoesNotHaveOwner) {
  const auto maker_party_contra_firm = Party{PartyId{"Maker"},
                                             PartyIdSource::Option::BIC,
                                             PartyRole::Option::ContraFirm};
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Sell)
                         .build_market_order();
  const auto maker = builder.with_order_parties({maker_party_contra_firm})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_FALSE(trade.buyer.has_value());
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsNullSellerIfMarketOrderTakerSellSideDoesNotHaveOwner) {
  const auto taker_party_contra_firm = Party{PartyId{"Taker"},
                                             PartyIdSource::Option::BIC,
                                             PartyRole::Option::ContraFirm};
  const auto taker = builder.with_order_parties({taker_party_contra_firm})
                         .with_side(Side::Option::Sell)
                         .build_market_order();
  const auto maker = builder.with_order_parties({maker_party})
                         .with_side(Side::Option::Buy)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_FALSE(trade.seller.has_value());
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsNullSellerIfLimitOrderMakerSellSideDoesNotHaveOwner) {
  const auto maker_party_contra_firm = Party{PartyId{"Maker"},
                                             PartyIdSource::Option::BIC,
                                             PartyRole::Option::ContraFirm};
  const auto taker = builder.with_order_parties({taker_party})
                         .with_side(Side::Option::Buy)
                         .build_market_order();
  const auto maker = builder.with_order_parties({maker_party_contra_firm})
                         .with_side(Side::Option::Sell)
                         .build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_FALSE(trade.seller.has_value());
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsTradePriceFromExecutionPrice) {
  const auto taker = builder.build_market_order();
  const auto maker = builder.build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.trade_price, Price{100});
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsTradedQuantityFromExecutedQuantity) {
  const auto taker = builder.build_market_order();
  const auto maker = builder.build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.traded_quantity, Quantity{50});
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsAggressorSideFromTakerSide) {
  const auto taker = builder.with_side(Side::Option::Buy).build_market_order();
  const auto maker = builder.with_side(Side::Option::Sell).build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.aggressor_side, AggressorSide::Option::Buy);
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation, SetsTradeTime) {
  const auto taker = builder.build_market_order();
  const auto maker = builder.build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_GT(trade.trade_time.time_since_epoch().count(), 0);
}

TEST_F(OrderBookNotificationTradeFromMarketLimitOrdersCreation,
       SetsOpenMarketPhase) {
  const auto taker = builder.build_limit_order();
  const auto maker = builder.build_limit_order();

  const auto notification = make_trade_notification(
      taker, maker, ExecutionPrice{100}, ExecutedQuantity{50});

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.market_phase, MarketPhase::open());
}

// NOLINTEND(*non-private-members*,*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::tests
