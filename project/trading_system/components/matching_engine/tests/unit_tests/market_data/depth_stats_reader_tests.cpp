#include <gmock/gmock.h>

#include <ranges>
#include <vector>

#include "ih/market_data/depth/depth_stats_reader.hpp"
#include "tools/fake_depth_node.hpp"

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-number*)

namespace simulator::trading_system::matching_engine::mdata {
namespace {

struct DepthStatsReaderTest : Test {
  static auto view(const auto& depth) {
    return std::views::transform(depth,
                                 [](const auto& node) { return node.level(); });
  }

  DepthStatsReader reader;
};

TEST_F(DepthStatsReaderTest, IsEmptyOnceCreated) {
  EXPECT_THAT(reader.levels_count(), Eq(0));
  EXPECT_THAT(reader.tob_quantity(), Eq(std::nullopt));
  EXPECT_THAT(reader.tob_price(), Eq(std::nullopt));
}

TEST_F(DepthStatsReaderTest, ReturnsNoLevelsOnEmptyView) {
  constexpr auto depth = std::vector<FakeBidNode>{};

  reader.read(view(depth));

  EXPECT_THAT(reader.levels_count(), Eq(0));
}

TEST_F(DepthStatsReaderTest, FiltersOutInvisibleAndRemovedLevels) {
  const auto depth = std::vector{
      FakeBidNode::invisible(Price(200)),
      FakeBidNode::removed(Price(100)),
  };

  reader.read(view(depth));

  EXPECT_THAT(reader.levels_count(), Eq(0));
}

TEST_F(DepthStatsReaderTest, CountsAvailableLevels) {
  const auto depth = std::vector{
      FakeOfferNode::added(Price(100), Quantity(100)),
      FakeOfferNode::changed(Price(200), Quantity(200)),
      FakeOfferNode::unchanged(Price(300), Quantity(300)),
  };

  reader.read(view(depth));

  EXPECT_THAT(reader.levels_count(), Eq(3));
}

TEST_F(DepthStatsReaderTest, ExaminesTopVisibleLevel) {
  const auto depth = std::vector{
      FakeOfferNode::added(Price(100), Quantity(100)),
      FakeOfferNode::added(Price(200), Quantity(200)),
      FakeOfferNode::added(Price(300), Quantity(300)),
  };

  reader.read(view(depth));

  EXPECT_THAT(reader.tob_quantity(), Eq(Quantity(100)));
  EXPECT_THAT(reader.tob_price(), Eq(Price(100)));
}

TEST_F(DepthStatsReaderTest, FiltersOutTopInvisibleLevel) {
  const auto depth = std::vector{
      FakeOfferNode::invisible(Price(100)),
      FakeOfferNode::added(Price(200), Quantity(200)),
      FakeOfferNode::added(Price(300), Quantity(300)),
  };

  reader.read(view(depth));

  EXPECT_THAT(reader.tob_quantity(), Eq(Quantity(200)));
  EXPECT_THAT(reader.tob_price(), Eq(Price(200)));
}

TEST_F(DepthStatsReaderTest, FiltersOutTopRemovedLevel) {
  const auto depth = std::vector{
      FakeOfferNode::removed(Price(100)),
      FakeOfferNode::added(Price(200), Quantity(200)),
      FakeOfferNode::added(Price(300), Quantity(300)),
  };

  reader.read(view(depth));

  EXPECT_THAT(reader.tob_quantity(), Eq(Quantity(200)));
  EXPECT_THAT(reader.tob_price(), Eq(Price(200)));
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata

// NOLINTEND(*magic-number*)