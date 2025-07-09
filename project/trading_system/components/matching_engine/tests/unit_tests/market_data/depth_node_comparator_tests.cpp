#include <gtest/gtest.h>

#include "core/domain/attributes.hpp"
#include "ih/market_data/depth/depth_node.hpp"
#include "ih/market_data/depth/depth_node_comparator.hpp"
#include "tools/order_book_notification_builder.hpp"

using namespace testing;  // NOLINT

namespace simulator::trading_system::matching_engine::mdata {
namespace {

struct DepthNodeComparatorTest : Test {
  constexpr static std::optional<Price> NoPrice = std::nullopt;

  static auto make_node(const std::optional<Price> price) {
    return DepthNode(MarketEntryId("node-id"),
                     NewOrderAdded::init().with_order_price(price).create());
  }
};

/*----------------------------------------------------------------------------*/

struct BidComparatorTest : DepthNodeComparatorTest {
  BidComparator cmp;
};

TEST_F(BidComparatorTest, ReturnsTrueWhenLeftNodeIsHigher) {
  const auto left = make_node(Price(100));
  const auto right = make_node(Price(50));

  ASSERT_TRUE(cmp(left, right));
}

TEST_F(BidComparatorTest, ReturnsFalseWhenLeftNodeIsLower) {
  const auto left = make_node(Price(50));
  const auto right = make_node(Price(100));

  ASSERT_FALSE(cmp(left, right));
}

TEST_F(BidComparatorTest, ReturnsTrueWhenNodesAreEqual) {
  const auto left = make_node(Price(50));
  const auto right = make_node(Price(50));

  ASSERT_TRUE(cmp(left, right));
}

TEST_F(BidComparatorTest, ReturnsTrueWhenLeftNodeHasNoPriceRightHas) {
  const auto left = make_node(NoPrice);
  const auto right = make_node(Price(50));

  ASSERT_TRUE(cmp(left, right));
}

TEST_F(BidComparatorTest, ReturnsFalseWhenRightNodeHasNoPriceLeftHas) {
  const auto left = make_node(Price(50));
  const auto right = make_node(NoPrice);

  ASSERT_TRUE(cmp(left, right));
}

TEST_F(BidComparatorTest, ReturnsFalseWhenBothNodesHaveNoPrice) {
  const auto left = make_node(NoPrice);
  const auto right = make_node(NoPrice);

  ASSERT_FALSE(cmp(left, right));
}

/*----------------------------------------------------------------------------*/

struct OfferComparatorTest : DepthNodeComparatorTest {
  OfferComparator cmp;
};

TEST_F(OfferComparatorTest, ReturnsTrueWhenLeftNodeIsLower) {
  const auto left = make_node(Price(50));
  const auto right = make_node(Price(100));

  ASSERT_TRUE(cmp(left, right));
}

TEST_F(OfferComparatorTest, ReturnsFalseWhenLeftNodeIsHigher) {
  const auto left = make_node(Price(100));
  const auto right = make_node(Price(50));

  ASSERT_FALSE(cmp(left, right));
}

TEST_F(OfferComparatorTest, ReturnsTrueWhenNodesAreEqual) {
  const auto left = make_node(Price(50));
  const auto right = make_node(Price(50));

  ASSERT_TRUE(cmp(left, right));
}

TEST_F(OfferComparatorTest, ReturnsTrueWhenLeftNodeHasNoPriceRightHas) {
  const auto left = make_node(NoPrice);
  const auto right = make_node(Price(50));

  ASSERT_TRUE(cmp(left, right));
}

TEST_F(OfferComparatorTest, ReturnsFalseWhenRightNodeHasNoPriceLeftHas) {
  const auto left = make_node(Price(50));
  const auto right = make_node(NoPrice);

  ASSERT_TRUE(cmp(left, right));
}

TEST_F(OfferComparatorTest, ReturnsFalseWhenBothNodesHaveNoPrice) {
  const auto left = make_node(NoPrice);
  const auto right = make_node(NoPrice);

  ASSERT_FALSE(cmp(left, right));
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata