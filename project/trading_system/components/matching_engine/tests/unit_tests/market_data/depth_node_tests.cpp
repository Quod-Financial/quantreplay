#include <gtest/gtest.h>

#include "core/domain/attributes.hpp"
#include "ih/market_data/depth/depth_node.hpp"
#include "tools/order_book_notification_builder.hpp"

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-number*)

namespace simulator::trading_system::matching_engine::mdata {
namespace {

struct DepthNodeFixture : Test {
  MarketEntryId identifier{"node-id"};

  auto make_node(const NewOrderAdded& builder) const {
    return DepthNode(identifier, builder.create());
  }
};

/*----------------------------------------------------------------------------*/

struct DepthNodeTest : DepthNodeFixture {};

TEST_F(DepthNodeTest, CreatesWithSpecifiedMarketEntryId) {
  const auto node = make_node(NewOrderAdded::init());

  ASSERT_EQ(node.id(), identifier);
}

TEST_F(DepthNodeTest, CreatesWithSpecifiedPrice) {
  const auto node =
      make_node(NewOrderAdded::init().with_order_price(Price(100)));

  ASSERT_EQ(node.price(), Price(100));
}

TEST_F(DepthNodeTest, IsEmptyWhenCurrentQuantitiyIsZero) {
  const auto node =
      make_node(NewOrderAdded::init().with_order_quantity(Quantity(0)));

  ASSERT_TRUE(node.empty());
}

TEST_F(DepthNodeTest, IsNotEmptyWhenCurrentQuantitiyIsNonZero) {
  const auto node =
      make_node(NewOrderAdded::init().with_order_quantity(Quantity(1)));

  ASSERT_FALSE(node.empty());
}

/*----------------------------------------------------------------------------*/

struct FullDepthNodeLevelTest : DepthNodeFixture {};

TEST_F(FullDepthNodeLevelTest,
       ReturnsInvisibleLevelWhenInitiallyCreatedWithoutQuantity) {
  const auto node =
      make_node(NewOrderAdded::init().with_order_quantity(Quantity(0)));

  const auto level = node.full_level();

  ASSERT_TRUE(level.is_invisible());
}

TEST_F(FullDepthNodeLevelTest,
       ReturnsAddedLevelWhenInitiallyCreatedWithNonZeroQuantity) {
  const auto node =
      make_node(NewOrderAdded::init().with_order_quantity(Quantity(1)));

  const auto level = node.full_level();
  ASSERT_TRUE(level.is_added());
}

TEST_F(FullDepthNodeLevelTest, ReturnsUnchangedLevelWhenQuantityWasNotChanged) {
  auto node = make_node(NewOrderAdded::init().with_order_quantity(Quantity(1)));

  node.fold();

  const auto level = node.full_level();
  ASSERT_TRUE(level.is_unchanged());
}

TEST_F(FullDepthNodeLevelTest, ReturnsChangedLevelWhenQuantityWasChanged) {
  auto node = make_node(NewOrderAdded::init()
                            .with_order_id(OrderId(10))
                            .with_order_quantity(Quantity(100)));

  node.fold();
  node.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(11))
                 .with_order_quantity(Quantity(200))
                 .create());

  const auto level = node.full_level();
  ASSERT_TRUE(level.is_changed());
}

TEST_F(FullDepthNodeLevelTest,
       ReturnsRemovedLevelWhenLastOrderQuantityIsReducedToZero) {
  auto node = make_node(NewOrderAdded::init()
                            .with_order_id(OrderId(10))
                            .with_order_quantity(Quantity(100)));

  node.fold();
  node.apply(NewOrderReduced::init()
                 .with_order_id(OrderId(10))
                 .with_order_quantity(Quantity(0))
                 .create());

  const auto level = node.full_level();
  ASSERT_TRUE(level.is_removed());
}

TEST_F(FullDepthNodeLevelTest, ReturnsRemovedLevelWhenLastOrderIsRemoved) {
  auto node = make_node(NewOrderAdded::init()
                            .with_order_id(OrderId(10))
                            .with_order_quantity(Quantity(100)));

  node.fold();
  node.apply(NewOrderRemoved::init().with_order_id(OrderId(10)).create());

  const auto level = node.full_level();
  ASSERT_TRUE(level.is_removed());
}

/*----------------------------------------------------------------------------*/

struct PartialDepthNodeLevelTest : DepthNodeFixture {
  PartyId owner{"owner-id"};
};

TEST_F(PartialDepthNodeLevelTest,
       ReturnsInvisibleLevelWhenInitiallyCreatedWithoutVisibleQuantity) {
  const auto node = make_node(NewOrderAdded::init()
                                  .with_order_id(OrderId(1))
                                  .with_order_owner(owner)
                                  .with_order_quantity(Quantity(100)));

  const auto level = node.partial_level(owner);

  ASSERT_TRUE(level.is_invisible());
}

TEST_F(PartialDepthNodeLevelTest,
       ReturnsNewLevelWhenInitiallyCreatedWithVisibleQuantity) {
  const auto node = make_node(NewOrderAdded::init()
                                  .with_order_id(OrderId(1))
                                  .with_order_quantity(Quantity(100)));

  const auto level = node.partial_level(owner);

  ASSERT_TRUE(level.is_added());
}

TEST_F(PartialDepthNodeLevelTest,
       ReturnsNewLevelWhenInitiallyUpdatedWithVisibleQuantity) {
  auto node = make_node(NewOrderAdded::init()
                            .with_order_id(OrderId(1))
                            .with_order_owner(owner)
                            .with_order_quantity(Quantity(100)));

  node.fold();
  node.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(2))
                 .with_order_quantity(Quantity(100)));

  const auto level = node.partial_level(owner);
  ASSERT_TRUE(level.is_added());
}

TEST_F(PartialDepthNodeLevelTest,
       ReturnsUnchangedLevelWhenVisibleQuantityWasNotChanged) {
  auto node = make_node(NewOrderAdded::init()
                            .with_order_id(OrderId(1))
                            .with_order_quantity(Quantity(100)));

  node.fold();

  const auto level = node.partial_level(owner);
  ASSERT_TRUE(level.is_unchanged());
}

TEST_F(PartialDepthNodeLevelTest,
       ReturnsChangedLevelWhenVisibleQuantityWasChanged) {
  auto node = make_node(NewOrderAdded::init()
                            .with_order_id(OrderId(1))
                            .with_order_quantity(Quantity(100)));

  node.fold();
  node.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(2))
                 .with_order_quantity(Quantity(200)));

  const auto level = node.partial_level(owner);
  ASSERT_TRUE(level.is_changed());
}

TEST_F(PartialDepthNodeLevelTest,
       ReturnsRemovedLevelWhenVisibleQuantityWasReducedToZero) {
  auto node = make_node(NewOrderAdded::init()
                            .with_order_id(OrderId(1))
                            .with_order_owner(owner)
                            .with_order_quantity(Quantity(100)));
  node.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(2))
                 .with_order_quantity(Quantity(200)));

  node.fold();
  node.apply(NewOrderReduced::init()
                 .with_order_id(OrderId(2))
                 .with_order_quantity(Quantity(0))
                 .create());

  const auto level = node.partial_level(owner);
  ASSERT_TRUE(level.is_removed());
}

TEST_F(PartialDepthNodeLevelTest,
       ReturnsInvisibleLevelWhenChangedWithNoVisibleQuantity) {
  auto node = make_node(NewOrderAdded::init()
                            .with_order_id(OrderId(1))
                            .with_order_owner(owner)
                            .with_order_quantity(Quantity(100)));

  node.fold();
  node.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(2))
                 .with_order_owner(owner)
                 .with_order_quantity(Quantity(200)));

  const auto level = node.partial_level(owner);
  ASSERT_TRUE(level.is_invisible());
}

/*----------------------------------------------------------------------------*/

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata

// NOLINTEND(*magic-number*)