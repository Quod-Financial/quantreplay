#include <gmock/gmock.h>

#include <vector>

#include "core/domain/attributes.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::order::test {
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
  order.execute(ExecutedQuantity{7.4}, ExecutionPrice{10.0});

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

TEST_F(OrderBookNotificationOrderAddedCreation,
       SetsLimitOrderTypeForLimitOrder) {
  const auto order = builder.build_limit_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_EQ(order_added.order_type, OrderType::Option::Limit);
}

TEST_F(OrderBookNotificationOrderAddedCreation,
       SetsMarketOrderTypeForMarketOrder) {
  const auto order = builder.build_market_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_EQ(order_added.order_type, OrderType::Option::Market);
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
  order.execute(ExecutedQuantity{7.4}, ExecutionPrice{10.0});

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

struct OrderBookNotificationMarketOrderAddedCreation : public Test {
  OrderBuilder builder;
};

TEST_F(OrderBookNotificationMarketOrderAddedCreation,
       SetsOrderAddedToOrderBookNotification) {
  const auto order = builder.build_market_order();

  const auto notification = make_making_order_added_to_book_notification(order);

  ASSERT_THAT(notification.value, VariantWith<OrderAdded>(_));
}

TEST_F(OrderBookNotificationMarketOrderAddedCreation,
       SetsOrderPriceEmptyForMarketOrder) {
  const auto order = builder.build_market_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_EQ(order_added.order_price, std::nullopt);
}

TEST_F(OrderBookNotificationMarketOrderAddedCreation,
       SetsOrderQuantityFromLeavesQuantityOfMarketOrder) {
  auto order =
      builder.with_order_quantity(OrderQuantity{10.5}).build_market_order();
  order.execute(ExecutedQuantity{7.4}, ExecutionPrice{10.0});

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_EQ(order_added.order_quantity, Quantity{3.1});
}

TEST_F(OrderBookNotificationMarketOrderAddedCreation,
       SetsOrderIdFromMarketOrder) {
  const auto order = builder.with_order_id(OrderId{123}).build_market_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_EQ(order_added.order_id, OrderId{123});
}

TEST_F(OrderBookNotificationMarketOrderAddedCreation,
       SetsOrderSideFromMarketOrder) {
  const auto order = builder.with_side(Side::Option::Sell).build_market_order();

  const auto notification = make_making_order_added_to_book_notification(order);
  const auto& order_added = std::get<OrderAdded>(notification.value);

  ASSERT_EQ(order_added.order_side, Side::Option::Sell);
}

struct OrderBookNotificationMarketOrderRemovedCreation : public Test {
  OrderBuilder builder;
};

TEST_F(OrderBookNotificationMarketOrderRemovedCreation,
       SetsOrderRemovedToOrderBookNotification) {
  const auto order = builder.build_market_order();

  const auto notification =
      make_making_order_removed_from_book_notification(order);

  ASSERT_THAT(notification.value, VariantWith<OrderRemoved>(_));
}

TEST_F(OrderBookNotificationMarketOrderRemovedCreation,
       SetsOrderPriceEmptyForMarketOrder) {
  const auto order = builder.build_market_order();

  const auto notification =
      make_making_order_removed_from_book_notification(order);
  const auto& order_removed = std::get<OrderRemoved>(notification.value);

  ASSERT_EQ(order_removed.order_price, std::nullopt);
}

TEST_F(OrderBookNotificationMarketOrderRemovedCreation,
       SetsOrderIdFromMarketOrder) {
  const auto order = builder.with_order_id(OrderId{123}).build_market_order();

  const auto notification =
      make_making_order_removed_from_book_notification(order);
  const auto& order_removed = std::get<OrderRemoved>(notification.value);

  ASSERT_EQ(order_removed.order_id, OrderId{123});
}

TEST_F(OrderBookNotificationMarketOrderRemovedCreation,
       SetsOrderSideFromMarketOrder) {
  const auto order = builder.with_side(Side::Option::Sell).build_market_order();

  const auto notification =
      make_making_order_removed_from_book_notification(order);
  const auto& order_removed = std::get<OrderRemoved>(notification.value);

  ASSERT_EQ(order_removed.order_side, Side::Option::Sell);
}

struct OrderBookNotificationAuctionTradeCreation : public Test {
  const Party buyer_party{PartyId{"Buyer"},
                          PartyIdSource::Option::BIC,
                          PartyRole::Option::ExecutingFirm};
  const Party seller_party{PartyId{"Seller"},
                           PartyIdSource::Option::FDID,
                           PartyRole::Option::ExecutingFirm};
  const MarketPhase auction_phase{TradingPhase::Option::OpeningAuction,
                                  TradingStatus::Option::Halt};

  OrderBuilder builder;
};

TEST_F(OrderBookNotificationAuctionTradeCreation,
       SetsTradeToOrderBookNotification) {
  const auto buy = builder.with_side(Side::Option::Buy).build_limit_order();
  const auto sell = builder.with_side(Side::Option::Sell).build_limit_order();

  const auto notification = make_auction_trade_notification(
      buy, sell, ExecutionPrice{100}, ExecutedQuantity{50}, auction_phase);

  ASSERT_THAT(notification.value, VariantWith<Trade>(_));
}

TEST_F(OrderBookNotificationAuctionTradeCreation, SetsBuyerFromBuyOrderOwner) {
  const auto buy = builder.with_order_parties({buyer_party})
                       .with_side(Side::Option::Buy)
                       .build_limit_order();
  const auto sell = builder.with_order_parties({seller_party})
                        .with_side(Side::Option::Sell)
                        .build_limit_order();

  const auto notification = make_auction_trade_notification(
      buy, sell, ExecutionPrice{100}, ExecutedQuantity{50}, auction_phase);

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.buyer->value(), buyer_party.party_id().value());
}

TEST_F(OrderBookNotificationAuctionTradeCreation,
       SetsSellerFromSellOrderOwner) {
  const auto buy = builder.with_order_parties({buyer_party})
                       .with_side(Side::Option::Buy)
                       .build_limit_order();
  const auto sell = builder.with_order_parties({seller_party})
                        .with_side(Side::Option::Sell)
                        .build_limit_order();

  const auto notification = make_auction_trade_notification(
      buy, sell, ExecutionPrice{100}, ExecutedQuantity{50}, auction_phase);

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.seller->value(), seller_party.party_id().value());
}

TEST_F(OrderBookNotificationAuctionTradeCreation, SetsNoAggressorSide) {
  const auto buy = builder.with_side(Side::Option::Buy).build_limit_order();
  const auto sell = builder.with_side(Side::Option::Sell).build_limit_order();

  const auto notification = make_auction_trade_notification(
      buy, sell, ExecutionPrice{100}, ExecutedQuantity{50}, auction_phase);

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_FALSE(trade.aggressor_side.has_value());
}

TEST_F(OrderBookNotificationAuctionTradeCreation, SetsMarketPhaseFromArgument) {
  const auto buy = builder.with_side(Side::Option::Buy).build_limit_order();
  const auto sell = builder.with_side(Side::Option::Sell).build_limit_order();
  const MarketPhase closing_auction{TradingPhase::Option::ClosingAuction,
                                    TradingStatus::Option::Halt};

  const auto notification = make_auction_trade_notification(
      buy, sell, ExecutionPrice{100}, ExecutedQuantity{50}, closing_auction);

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.market_phase, closing_auction);
}

TEST_F(OrderBookNotificationAuctionTradeCreation,
       SetsNoBuyerWhenBuyOrderHasNoOwner) {
  const auto buyer_contra_firm = Party{PartyId{"Buyer"},
                                       PartyIdSource::Option::BIC,
                                       PartyRole::Option::ContraFirm};
  const auto buy = builder.with_order_parties({buyer_contra_firm})
                       .with_side(Side::Option::Buy)
                       .build_limit_order();
  const auto sell = builder.with_order_parties({seller_party})
                        .with_side(Side::Option::Sell)
                        .build_limit_order();

  const auto notification = make_auction_trade_notification(
      buy, sell, ExecutionPrice{100}, ExecutedQuantity{50}, auction_phase);

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_FALSE(trade.buyer.has_value());
}

TEST_F(OrderBookNotificationAuctionTradeCreation,
       SupportsMarketAndLimitOrders) {
  const auto buy = builder.with_order_parties({buyer_party})
                       .with_side(Side::Option::Buy)
                       .build_market_order();
  const auto sell = builder.with_order_parties({seller_party})
                        .with_side(Side::Option::Sell)
                        .build_limit_order();

  const auto notification = make_auction_trade_notification(
      buy, sell, ExecutionPrice{100}, ExecutedQuantity{50}, auction_phase);

  const auto& trade = std::get<Trade>(notification.value);
  ASSERT_EQ(trade.buyer->value(), buyer_party.party_id().value());
}

// NOLINTEND(*non-private-members*,*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
