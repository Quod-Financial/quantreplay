#include "ih/market_data/depth/incremental_depth_update.hpp"

#include <gmock/gmock.h>

#include "tools/fake_depth_node.hpp"

using namespace ::testing;  // NOLINT

namespace simulator::trading_system::matching_engine::mdata {
namespace {

struct BuilderTest : Test {
  static auto depth_view(const auto& depth) {
    return std::views::transform(depth,
                                 [](const auto& node) { return node.level(); });
  }

  static auto EntryWith(const std::optional<Price> price,
                        const MarketEntryAction action) {
    return AllOf(Field(&MarketDataEntry::price, Eq(price)),
                 Field(&MarketDataEntry::action, Eq(action)));
  }

  std::vector<MarketDataEntry> destination;
  IncrementalDepthUpdate update{destination};
  constexpr static std::optional<Price> NoPrice = std::nullopt;
};

/*----------------------------------------------------------------------------*/

struct IncrementalDepthUpdateTest : Test {
  std::vector<MarketDataEntry> destination;
  IncrementalDepthUpdate update{destination};
};

TEST_F(IncrementalDepthUpdateTest, AddsNewLevelToDestination) {
  const auto node = FakeBidNode::added(Price(10), Quantity(100));

  update.add_new_level(node.level());

  EXPECT_THAT(destination.at(0).price, Optional(Eq(Price(10))));
  EXPECT_THAT(destination.at(0).quantity, Optional(Eq(Quantity(100))));
  EXPECT_THAT(destination.at(0).action, Eq(MarketEntryAction::Option::New));
  EXPECT_THAT(destination.at(0).type, Eq(MdEntryType::Option::Bid));
  EXPECT_THAT(destination.at(0).id, Eq(FakeBidNode::fake_id()));
}

TEST_F(IncrementalDepthUpdateTest, AddsChangedLevelToDestination) {
  const auto node = FakeOfferNode::changed(Price(10), Quantity(100));

  update.add_changed_level(node.level());

  EXPECT_THAT(destination.at(0).price, Optional(Eq(Price(10))));
  EXPECT_THAT(destination.at(0).quantity, Optional(Eq(Quantity(100))));
  EXPECT_THAT(destination.at(0).action, Eq(MarketEntryAction::Option::Change));
  EXPECT_THAT(destination.at(0).type, Eq(MdEntryType::Option::Offer));
  EXPECT_THAT(destination.at(0).id, Eq(FakeOfferNode::fake_id()));
}

TEST_F(IncrementalDepthUpdateTest, AddsRemovedLevelToDestination) {
  const auto node = FakeBidNode::removed(Price(10));

  update.add_removed_level(node.level());

  EXPECT_THAT(destination.at(0).price, Optional(Eq(Price(10))));
  EXPECT_THAT(destination.at(0).quantity, Eq(std::nullopt));
  EXPECT_THAT(destination.at(0).action, Eq(MarketEntryAction::Option::Delete));
  EXPECT_THAT(destination.at(0).type, Eq(MdEntryType::Option::Bid));
  EXPECT_THAT(destination.at(0).id, Eq(FakeBidNode::fake_id()));
}

TEST_F(IncrementalDepthUpdateTest, DoesNotAddNewLevelAsRemoved) {
  const auto node = FakeOfferNode::added(Price(10), Quantity(100));

  ASSERT_THROW(update.add_removed_level(node.level()), std::logic_error);
}

TEST_F(IncrementalDepthUpdateTest, DoesNotAddNewLevelAsChanged) {
  const auto node = FakeOfferNode::added(Price(10), Quantity(100));

  ASSERT_THROW(update.add_changed_level(node.level()), std::logic_error);
}

TEST_F(IncrementalDepthUpdateTest, DoesNotAddDeletedLevelAsAdded) {
  const auto node = FakeBidNode::removed(Price(10));

  ASSERT_THROW(update.add_new_level(node.level()), std::logic_error);
}

TEST_F(IncrementalDepthUpdateTest, DoesNotAddDeletedLevelAsChanged) {
  const auto node = FakeBidNode::removed(Price(10));

  ASSERT_THROW(update.add_changed_level(node.level()), std::logic_error);
}

TEST_F(IncrementalDepthUpdateTest, DoesNotAddUnchangedLevelAsChanged) {
  const auto node = FakeBidNode::unchanged(Price(10), Quantity(100));

  ASSERT_THROW(update.add_changed_level(node.level()), std::logic_error);
}

TEST_F(IncrementalDepthUpdateTest, DoesNotAddInvisibleLevelAsAdded) {
  const auto node = FakeBidNode::invisible(Price(10));

  ASSERT_THROW(update.add_new_level(node.level()), std::logic_error);
}

TEST_F(IncrementalDepthUpdateTest, DoesNotAddInvisibleLevelAsChanged) {
  const auto node = FakeBidNode::invisible(Price(10));

  ASSERT_THROW(update.add_changed_level(node.level()), std::logic_error);
}

TEST_F(IncrementalDepthUpdateTest, DoesNotAddInvisibleLevelAsRemoved) {
  const auto node = FakeBidNode::invisible(Price(10));

  ASSERT_THROW(update.add_removed_level(node.level()), std::logic_error);
}

/*----------------------------------------------------------------------------*/

struct IncrementalDepthBuilderTest : BuilderTest {};

TEST_F(IncrementalDepthBuilderTest, BuildsEmptyUpdateFromEmptyDepth) {
  constexpr std::vector<FakeBidNode> depth = {};

  IncrementalDepthUpdateBuilder builder(update);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination, IsEmpty());
}

TEST_F(IncrementalDepthBuilderTest, SkipsUnchangedLevels) {
  const std::vector depth = {
      FakeBidNode::unchanged(Price(2), Quantity(2)),
      FakeBidNode::unchanged(Price(1), Quantity(1)),
  };

  IncrementalDepthUpdateBuilder builder(update);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination, IsEmpty());
}

TEST_F(IncrementalDepthBuilderTest, SkipsInvisibleLevels) {
  const std::vector depth = {
      FakeOfferNode::invisible(Price(1)),
      FakeOfferNode::invisible(Price(2)),
  };

  IncrementalDepthUpdateBuilder builder(update);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination, IsEmpty());
}

TEST_F(IncrementalDepthBuilderTest, CollectsAddedLevel) {
  const auto depth = std::vector{FakeBidNode::added(Price(2), Quantity(2))};

  IncrementalDepthUpdateBuilder builder(update);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination,
              ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::New)));
}

TEST_F(IncrementalDepthBuilderTest, CollectsChangedLevel) {
  const auto depth = std::vector{FakeBidNode::changed(Price(2), Quantity(2))};

  IncrementalDepthUpdateBuilder builder(update);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::Change)));
}

TEST_F(IncrementalDepthBuilderTest, CollectsRemovedLevel) {
  const auto depth = std::vector{FakeBidNode::removed(Price(2))};

  IncrementalDepthUpdateBuilder builder(update);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::Delete)));
}

TEST_F(IncrementalDepthBuilderTest, CollectsAllUpdatedLevels) {
  const auto depth = std::vector{
      FakeBidNode::added(NoPrice, Quantity(2)),
      FakeBidNode::changed(Price(1), Quantity(1)),
      FakeBidNode::removed(Price(3)),
  };

  IncrementalDepthUpdateBuilder builder(update);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(NoPrice, MarketEntryAction::Option::New),
                  EntryWith(Price(1), MarketEntryAction::Option::Change),
                  EntryWith(Price(3), MarketEntryAction::Option::Delete)));
}

}  // namespace

/*----------------------------------------------------------------------------*/

struct LimitedIncrementalDepthBuilderTest : BuilderTest {
  constexpr static std::size_t TopOfBookLimit = 1;
};

TEST_F(LimitedIncrementalDepthBuilderTest, BuildsEmptyUpdateFromEmptyDepth) {
  constexpr std::vector<FakeBidNode> depth = {};

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination, IsEmpty());
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       IgnoresUnchangedLevelsInVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::unchanged(Price(2), Quantity(2)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination, IsEmpty());
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       AddsNewLevelToTheUpdateInVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::added(Price(2), Quantity(2)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination,
              ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::New)));
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       AddsChangedLevelToTheUpdateInVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::changed(Price(2), Quantity(2)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::Change)));
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       AddsRemovedLevelToTheUpdateInVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::removed(Price(2)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::Delete)));
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       DoesNotRemoveAddedLevelOutsideVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::added(Price(2), Quantity(2)),
      FakeBidNode::added(Price(3), Quantity(3)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(destination,
              ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::New)));
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       RemovesChangedLevelOutsideVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::added(Price(2), Quantity(2)),
      FakeBidNode::changed(Price(3), Quantity(3)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::New),
                  EntryWith(Price(3), MarketEntryAction::Option::Delete)));
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       RemovesUnchangedLevelOutsideVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::added(Price(2), Quantity(2)),
      FakeBidNode::unchanged(Price(3), Quantity(3)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::New),
                  EntryWith(Price(3), MarketEntryAction::Option::Delete)));
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       RemovesRemovedLevelOutsideVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::added(Price(2), Quantity(2)),
      FakeBidNode::removed(Price(3)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::New),
                  EntryWith(Price(3), MarketEntryAction::Option::Delete)));
}

TEST_F(LimitedIncrementalDepthBuilderTest, AddsAddedLevelOutsideVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::removed(Price(2)),
      FakeBidNode::added(Price(3), Quantity(3)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::Delete),
                  EntryWith(Price(3), MarketEntryAction::Option::New)));
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       AddsChangedLevelOutsideVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::removed(Price(2)),
      FakeBidNode::changed(Price(3), Quantity(3)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::Delete),
                  EntryWith(Price(3), MarketEntryAction::Option::New)));
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       AddsUnchangedLevelOutsideVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::removed(Price(2)),
      FakeBidNode::unchanged(Price(3), Quantity(3)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::Delete),
                  EntryWith(Price(3), MarketEntryAction::Option::New)));
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       DoesNotAddRemovedLevelOutsideVisibleWindow) {
  const std::vector depth = {
      FakeBidNode::removed(Price(2)),
      FakeBidNode::removed(Price(3)),
  };

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::Delete)));
}

TEST_F(LimitedIncrementalDepthBuilderTest,
       DoesNotCorrectVisibleWindowIfNoLevelsWereAddedOrRemoved) {
  const std::vector depth = {FakeBidNode::changed(Price(2), Quantity(2))};

  LimitedIncrementalDepthUpdateBuilder builder(update, TopOfBookLimit);
  builder.build(depth_view(depth));

  ASSERT_THAT(
      destination,
      ElementsAre(EntryWith(Price(2), MarketEntryAction::Option::Change)));
}

/*----------------------------------------------------------------------------*/

}  // namespace simulator::trading_system::matching_engine::mdata