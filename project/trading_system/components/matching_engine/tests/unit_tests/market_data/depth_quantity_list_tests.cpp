#include <gtest/gtest.h>

#include "ih/market_data/depth/depth_quantity_list.hpp"
#include "tools/order_book_notification_builder.hpp"

using namespace testing;  // NOLINT

namespace simulator::trading_system::matching_engine::mdata {
namespace {

// NOLINTBEGIN(*magic-number*)

struct DepthQuantityListTest : Test {
  PartyId owner{"owner-id"};
  DepthQuantityList list;
};

TEST_F(DepthQuantityListTest, HasEmptyFullQuantityInitially) {
  ASSERT_EQ(list.full_quantity(), Quantity(0));
}

TEST_F(DepthQuantityListTest, HasEmptyPartialQuantityInitially) {
  ASSERT_EQ(list.partial_quantity(owner), Quantity(0));
}

TEST_F(DepthQuantityListTest, AddsNewOrder) {
  list.apply(NewOrderAdded::init().with_order_quantity(Quantity(100)).create());

  ASSERT_EQ(list.full_quantity(), Quantity(100));
}

TEST_F(DepthQuantityListTest, FailsToAddOrderWithDuplicatedOrderId) {
  const auto action = NewOrderAdded::init().with_order_id(OrderId(1)).create();
  list.apply(action);

  ASSERT_THROW(list.apply(action), std::logic_error);
}

TEST_F(DepthQuantityListTest, ReducesExistingOrder) {
  list.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(1))
                 .with_order_quantity(Quantity(100))
                 .create());

  list.apply(NewOrderReduced::init()
                 .with_order_id(OrderId(1))
                 .with_order_quantity(Quantity(50))
                 .create());

  ASSERT_EQ(list.full_quantity(), Quantity(50));
}

TEST_F(DepthQuantityListTest, RemovesOrderWhenQuantityIsReducedToZero) {
  list.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(1))
                 .with_order_quantity(Quantity(100))
                 .create());

  list.apply(NewOrderReduced::init()
                 .with_order_id(OrderId(1))
                 .with_order_quantity(Quantity(0))
                 .create());

  ASSERT_EQ(list.full_quantity(), Quantity(0));
}

TEST_F(DepthQuantityListTest, FailsToReduceNonExistingOrder) {
  const auto action =
      NewOrderReduced::init().with_order_id(OrderId(1)).create();

  ASSERT_THROW(list.apply(action), std::logic_error);
}

TEST_F(DepthQuantityListTest, RemovesOrder) {
  list.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(1))
                 .with_order_quantity(Quantity(100))
                 .create());

  list.apply(NewOrderRemoved::init().with_order_id(OrderId(1)).create());

  ASSERT_EQ(list.full_quantity(), Quantity(0));
}

TEST_F(DepthQuantityListTest, FailsToRemoveNonExistingOrder) {
  const auto action =
      NewOrderRemoved::init().with_order_id(OrderId(1)).create();

  ASSERT_THROW(list.apply(action), std::logic_error);
}

TEST_F(DepthQuantityListTest, AggregatesFullQuantityWithMultipleOrders) {
  list.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(1))
                 .with_order_quantity(Quantity(1005.125))
                 .create());
  list.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(2))
                 .with_order_quantity(Quantity(1005.125))
                 .create());
  list.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(3))
                 .with_order_quantity(Quantity(1005.500))
                 .create());

  ASSERT_EQ(list.partial_quantity(owner), Quantity(3015.75));
}

TEST_F(DepthQuantityListTest, ExcludesOwnerQuantityForPartialQuantity) {
  list.apply(NewOrderAdded::init()
                 .with_order_owner(owner)
                 .with_order_quantity(Quantity(1005.125))
                 .create());

  ASSERT_EQ(list.partial_quantity(owner), Quantity(0));
}

TEST_F(DepthQuantityListTest, CalculatesPartialQuantityWithMultipleOrders) {
  list.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(1))
                 .with_order_owner(owner)
                 .with_order_quantity(Quantity(1005.125))
                 .create());
  list.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(2))
                 .with_order_owner(owner)
                 .with_order_quantity(Quantity(1005.125))
                 .create());
  list.apply(NewOrderAdded::init()
                 .with_order_id(OrderId(3))
                 .with_order_quantity(Quantity(1005.500))
                 .create());

  ASSERT_EQ(list.partial_quantity(owner), Quantity(1005.500));
}

// NOLINTEND(*magic-number*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata