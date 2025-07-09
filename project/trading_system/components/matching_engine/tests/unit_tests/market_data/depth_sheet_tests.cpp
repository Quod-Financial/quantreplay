#include <gmock/gmock.h>

#include "ih/market_data/depth/depth_node_comparator.hpp"
#include "ih/market_data/depth/depth_sheet.hpp"
#include "mocks/mock_market_entry_id_generator.hpp"
#include "tools/order_book_notification_builder.hpp"

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-number*)

namespace simulator::trading_system::matching_engine::mdata {

struct DepthSheetTest : Test {
  constexpr static std::optional<Price> NoPrice = std::nullopt;

  static auto wrap(const std::ranges::range auto& range) {
    return std::vector(range.begin(), range.end());
  }

  NiceMock<MockMarketEntryIdGenerator> idgen;
  DepthSheet sheet = DepthSheet::create_bid_sheet(idgen);
  PartyId owner{"owner"};

 private:
  auto SetUp() -> void override {
    ON_CALL(idgen, generate()).WillByDefault(Return(MarketEntryId("node-id")));
  }
};

TEST_F(DepthSheetTest, HasEmptyViewWhenCreated) {
  ASSERT_THAT(wrap(sheet.view()), IsEmpty());
}

TEST_F(DepthSheetTest, HasEmptyPartialViewWhenCreated) {
  ASSERT_THAT(wrap(sheet.partial_view(owner)), IsEmpty());
}

TEST_F(DepthSheetTest, AddsNewNodeWhenEmptyAndHandlingOrderAdded) {
  sheet.apply(NewOrderAdded::init().with_order_price(Price(100)).create());

  ASSERT_THAT(wrap(sheet.view()), SizeIs(1));
}

TEST_F(DepthSheetTest, AddsNewNodeWhenHandlingOrderAddedWithUniquePrice) {
  sheet.apply(NewOrderAdded::init().with_order_price(Price(100)).create());

  sheet.apply(NewOrderAdded::init().with_order_price(Price(101)).create());

  ASSERT_THAT(wrap(sheet.view()), SizeIs(2));
}

TEST_F(DepthSheetTest, AggregatesNewOrderWithNodeWhenPricesEqual) {
  sheet.apply(NewOrderAdded::init()
                  .with_order_price(Price(100))
                  .with_order_id(OrderId(1))
                  .create());

  sheet.apply(NewOrderAdded::init()
                  .with_order_price(Price(100))
                  .with_order_id(OrderId(2))
                  .create());

  ASSERT_THAT(wrap(sheet.view()), SizeIs(1));
}

TEST_F(DepthSheetTest, ThrowsExceptionOnAttemptToReduceNonExistingOrder) {
  const auto action = NewOrderReduced::init()
                          .with_order_id(OrderId(1))
                          .with_order_price(Price(100))
                          .with_order_quantity(Quantity(100))
                          .create();

  ASSERT_THROW(sheet.apply(action), std::logic_error);
}

TEST_F(DepthSheetTest, UpdatesNodeWhenHandlingOrderReduced) {
  constexpr auto price = Price(100);
  constexpr auto identifier = OrderId(1);
  sheet.apply(NewOrderAdded::init()
                  .with_order_id(identifier)
                  .with_order_price(price)
                  .with_order_quantity(Quantity(200))
                  .create());

  sheet.apply(NewOrderReduced::init()
                  .with_order_id(identifier)
                  .with_order_price(price)
                  .with_order_quantity(Quantity(100))
                  .create());

  ASSERT_THAT(wrap(sheet.view()),
              ElementsAre(Property(&DepthLevel::quantity, Eq(Quantity(100)))));
}

TEST_F(DepthSheetTest, ThrowsExceptionOnAttemptToRemoveNonExistingOrder) {
  const auto action = NewOrderRemoved::init()
                          .with_order_id(OrderId(1))
                          .with_order_price(Price(100))
                          .create();

  ASSERT_THROW(sheet.apply(action), std::logic_error);
}

TEST_F(DepthSheetTest, UpdatesNodeWhenHandlingOrderRemoved) {
  constexpr auto price = Price(100);
  constexpr auto identifier = OrderId(1);
  sheet.apply(NewOrderAdded::init()
                  .with_order_id(identifier)
                  .with_order_price(price)
                  .with_order_quantity(Quantity(200))
                  .create());

  sheet.apply(NewOrderRemoved::init()
                  .with_order_id(identifier)
                  .with_order_price(price)
                  .create());

  ASSERT_THAT(wrap(sheet.view()),
              ElementsAre(Property(&DepthLevel::quantity, Eq(Quantity(0)))));
}

TEST_F(DepthSheetTest, TransformsAddedNodeToVisibleLevel) {
  sheet.apply(NewOrderAdded::init()
                  .with_order_price(Price(100))
                  .with_order_quantity(Quantity(200))
                  .create());

  ASSERT_THAT(wrap(sheet.view()),
              ElementsAre(Property(&DepthLevel::quantity, Eq(Quantity(200)))));
}

TEST_F(DepthSheetTest, FoldsNonEmptyNodesWhileFolding) {
  sheet.apply(NewOrderAdded::init().with_order_price(Price(100)).create());

  sheet.fold();

  ASSERT_THAT(wrap(sheet.view()),
              ElementsAre(Property(&DepthLevel::is_unchanged, IsTrue())));
}

TEST_F(DepthSheetTest, RemovesEmptyNodesWhileFolding) {
  sheet.apply(NewOrderAdded::init().with_order_price(Price(100)).create());
  sheet.apply(NewOrderReduced::init()
                  .with_order_price(Price(100))
                  .with_order_quantity(Quantity(0))
                  .create());

  sheet.fold();

  ASSERT_THAT(wrap(sheet.view()), IsEmpty());
}

TEST_F(DepthSheetTest, TransformsNodeToPartialLevelInPartialView) {
  sheet.apply(NewOrderAdded::init()
                  .with_order_price(Price(100))
                  .with_order_owner(owner)
                  .create());

  ASSERT_THAT(wrap(sheet.partial_view(owner)),
              ElementsAre(Property(&DepthLevel::is_invisible, IsTrue())));
}

TEST_F(DepthSheetTest, BidSheetKeepsLevelsSotedByBidPriceCriteria) {
  sheet = DepthSheet::create_bid_sheet(idgen);
  sheet.apply(NewOrderAdded::init().with_order_price(NoPrice).create());
  sheet.apply(NewOrderAdded::init().with_order_price(Price(100)).create());
  sheet.apply(NewOrderAdded::init().with_order_price(Price(100.1)).create());
  sheet.apply(NewOrderAdded::init().with_order_price(Price(200)).create());

  ASSERT_THAT(wrap(sheet.view()),
              ElementsAre(Property(&DepthLevel::price, Eq(NoPrice)),
                          Property(&DepthLevel::price, Eq(Price(200))),
                          Property(&DepthLevel::price, Eq(Price(100.1))),
                          Property(&DepthLevel::price, Eq(Price(100)))));
}

TEST_F(DepthSheetTest, OfferSheetKeepsLevelsSotedByOfferPriceCriteria) {
  sheet = DepthSheet::create_offer_sheet(idgen);
  sheet.apply(NewOrderAdded::init().with_order_price(NoPrice).create());
  sheet.apply(NewOrderAdded::init().with_order_price(Price(100)).create());
  sheet.apply(NewOrderAdded::init().with_order_price(Price(100.1)).create());
  sheet.apply(NewOrderAdded::init().with_order_price(Price(200)).create());

  ASSERT_THAT(wrap(sheet.view()),
              ElementsAre(Property(&DepthLevel::price, Eq(NoPrice)),
                          Property(&DepthLevel::price, Eq(Price(100))),
                          Property(&DepthLevel::price, Eq(Price(100.1))),
                          Property(&DepthLevel::price, Eq(Price(200)))));
}

// NOLINTEND(*magic-number*)

}  // namespace simulator::trading_system::matching_engine::mdata