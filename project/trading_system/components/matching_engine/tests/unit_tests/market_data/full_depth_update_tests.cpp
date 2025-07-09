#include <gmock/gmock.h>

#include "ih/market_data/depth/full_depth_update.hpp"
#include "tools/fake_depth_node.hpp"

using namespace ::testing;  // NOLINT

namespace simulator::trading_system::matching_engine::mdata {
namespace {

struct BuilderTest : Test {
  static auto depth_view(const auto& depth) {
    return std::views::transform(depth,
                                 [](const auto& node) { return node.level(); });
  }

  static auto EntryWith(const std::optional<Price> price) {
    return Field(&MarketDataEntry::price, Eq(price));
  }

  std::vector<MarketDataEntry> destination;
  FullDepthUpdate update{destination};
  constexpr static std::optional<Price> NoPrice = std::nullopt;
};

/*----------------------------------------------------------------------------*/

struct FullDepthUpdateTest : Test {
  std::vector<MarketDataEntry> destination;
  FullDepthUpdate update{destination};
};

TEST_F(FullDepthUpdateTest, AddsUnchangedLevelToDestination) {
  const auto node = FakeBidNode::unchanged(Price(1), Quantity(1));

  update.add(node.level());

  ASSERT_THAT(destination, SizeIs(1));
}

TEST_F(FullDepthUpdateTest, AddsNewLevelToDestination) {
  const auto node = FakeBidNode::added(Price(1), Quantity(1));

  update.add(node.level());

  ASSERT_THAT(destination, SizeIs(1));
}

TEST_F(FullDepthUpdateTest, AddsChangedLevelToDestination) {
  const auto node = FakeBidNode::changed(Price(1), Quantity(1));

  update.add(node.level());

  ASSERT_THAT(destination, SizeIs(1));
}

TEST_F(FullDepthUpdateTest, DoesNotAddInvisibleLevelToDestination) {
  const auto node = FakeBidNode::invisible(Price(1));

  ASSERT_THROW(update.add(node.level()), std::logic_error);
}

TEST_F(FullDepthUpdateTest, DoesNotAddRemovedLevelToDestination) {
  const auto node = FakeBidNode::removed(Price(1));

  ASSERT_THROW(update.add(node.level()), std::logic_error);
}

TEST_F(FullDepthUpdateTest, SetsLevelPriceToMarketDataEntry) {
  const auto node = FakeBidNode::unchanged(Price(10), Quantity(100));

  update.add(node.level());

  ASSERT_THAT(destination.at(0).price, Eq(Price(10)));
}

TEST_F(FullDepthUpdateTest, SetsLevelQuantityToMarketDataEntry) {
  const auto node = FakeBidNode::unchanged(Price(10), Quantity(100));

  update.add(node.level());

  ASSERT_THAT(destination.at(0).quantity, Eq(Quantity(100)));
}

TEST_F(FullDepthUpdateTest, SetsLevelTypeToMarketDataEntry) {
  const auto node = FakeBidNode::unchanged(Price(10), Quantity(100));

  update.add(node.level());

  ASSERT_THAT(destination.at(0).type, Eq(MdEntryType::Option::Bid));
}

TEST_F(FullDepthUpdateTest, SetsLevelIdToMarketDataEntry) {
  const auto node = FakeBidNode::unchanged(Price(10), Quantity(100));

  update.add(node.level());

  ASSERT_THAT(destination.at(0).id, Eq(FakeBidNode::fake_id()));
}

TEST_F(FullDepthUpdateTest, DoesNotSpecifyLevelAction) {
  const auto node = FakeBidNode::unchanged(Price(10), Quantity(100));

  update.add(node.level());

  ASSERT_THAT(destination.at(0).action, Eq(std::nullopt));
}

/*----------------------------------------------------------------------------*/

struct FullDepthUpdateBuilderTest : BuilderTest {};

TEST_F(FullDepthUpdateBuilderTest, BuildsEmptyUpdateFromEmptyDepth) {
  constexpr std::vector<FakeBidNode> depth = {};

  FullDepthUpdateBuilder builder(update);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination, IsEmpty());
}

TEST_F(FullDepthUpdateBuilderTest, SkipsInvisibleLevels) {
  const std::vector depth = {
      FakeBidNode::invisible(Price(3)),
      FakeBidNode::invisible(Price(2)),
      FakeBidNode::invisible(Price(1)),
  };

  FullDepthUpdateBuilder builder(update);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination, SizeIs(0));
}

TEST_F(FullDepthUpdateBuilderTest, SkipsDeletedLevels) {
  const std::vector depth = {
      FakeOfferNode::removed(Price(1)),
      FakeOfferNode::removed(Price(2)),
      FakeOfferNode::removed(Price(3)),
  };

  FullDepthUpdateBuilder builder(update);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination, SizeIs(0));
}

TEST_F(FullDepthUpdateBuilderTest, CollectsAllVisibleLevels) {
  const std::vector depth = {
      FakeBidNode::unchanged(NoPrice, Quantity(3)),
      FakeBidNode::added(Price(2), Quantity(2)),
      FakeBidNode::changed(Price(1), Quantity(1)),
  };

  FullDepthUpdateBuilder builder(update);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(
          EntryWith(NoPrice), EntryWith(Price(2)), EntryWith(Price(1))));
}

/*----------------------------------------------------------------------------*/

struct LimitedFullDepthUpdateBuilderTest : BuilderTest {};

TEST_F(LimitedFullDepthUpdateBuilderTest, BuildsEmptyUpdateFromEmptyDepth) {
  constexpr std::uint32_t limit = 10;
  constexpr std::vector<FakeBidNode> depth = {};

  LimitedFullDepthUpdateBuilder builder(update, limit);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination, IsEmpty());
}

TEST_F(LimitedFullDepthUpdateBuilderTest, SkipsInvisibleLevels) {
  constexpr std::uint32_t limit = 10;
  const std::vector depth = {
      FakeBidNode::invisible(Price(3)),
      FakeBidNode::invisible(Price(2)),
      FakeBidNode::invisible(Price(1)),
  };

  LimitedFullDepthUpdateBuilder builder(update, limit);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination, SizeIs(0));
}

TEST_F(LimitedFullDepthUpdateBuilderTest, SkipsDeletedLevels) {
  constexpr std::uint32_t limit = 10;
  const std::vector depth = {
      FakeOfferNode::removed(Price(1)),
      FakeOfferNode::removed(Price(2)),
      FakeOfferNode::removed(Price(3)),
  };

  LimitedFullDepthUpdateBuilder builder(update, limit);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination, SizeIs(0));
}

TEST_F(LimitedFullDepthUpdateBuilderTest, CollectsAllVisibleLevelsInLimit) {
  constexpr std::uint32_t limit = 10;
  const std::vector depth = {
      FakeBidNode::unchanged(NoPrice, Quantity(3)),
      FakeBidNode::added(Price(2), Quantity(2)),
      FakeBidNode::changed(Price(1), Quantity(1)),
  };

  LimitedFullDepthUpdateBuilder builder(update, limit);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(
          EntryWith(NoPrice), EntryWith(Price(2)), EntryWith(Price(1))));
}

TEST_F(LimitedFullDepthUpdateBuilderTest,
       DoesNotCollectVisibleLevelsBeyondLimit) {
  constexpr std::uint32_t limit = 2;
  const std::vector depth = {
      FakeBidNode::unchanged(NoPrice, Quantity(3)),
      FakeBidNode::added(Price(2), Quantity(2)),
      FakeBidNode::changed(Price(1), Quantity(1)),
  };

  LimitedFullDepthUpdateBuilder builder(update, limit);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination,
              ElementsAre(EntryWith(NoPrice), EntryWith(Price(2))));
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata
