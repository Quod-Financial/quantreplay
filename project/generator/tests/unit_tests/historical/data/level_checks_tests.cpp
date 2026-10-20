#include <gtest/gtest.h>

#include <optional>

#include "ih/historical/data/level_checks.hpp"
#include "ih/historical/data/record.hpp"

namespace simulator::generator::historical::test {
namespace {

class GeneratorHistoricalLevelChecks : public testing::Test {
 public:
  struct LevelAttributes {
    std::optional<double> price;
    std::optional<double> quantity;
  };

  static auto make_level(const LevelAttributes& bid_attributes,
                         const LevelAttributes& offer_attributes) -> Level {
    Level::Builder builder;

    if (bid_attributes.price.has_value()) {
      builder.with_bid_price(*bid_attributes.price);
    }
    if (bid_attributes.quantity.has_value()) {
      builder.with_bid_quantity(*bid_attributes.quantity);
    }

    if (offer_attributes.price.has_value()) {
      builder.with_offer_price(*offer_attributes.price);
    }
    if (offer_attributes.quantity.has_value()) {
      builder.with_offer_quantity(*offer_attributes.quantity);
    }

    return Level::Builder::construct(std::move(builder));
  }
};

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidBidFalseIfBidPriceAndQuantityAreNotSet) {
  const auto level = make_level(LevelAttributes{}, LevelAttributes{});
  EXPECT_FALSE(has_valid_bid(level));
}

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidBidFalseIfBidQuantityIsNotSet) {
  const auto level =
      make_level(LevelAttributes{12.0, std::nullopt}, LevelAttributes{});
  EXPECT_FALSE(has_valid_bid(level));
}

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidBidFalseIfBidPriceIsNotSet) {
  const auto level =
      make_level(LevelAttributes{std::nullopt, 114.5}, LevelAttributes{});
  EXPECT_FALSE(has_valid_bid(level));
}

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidBidFalseIfBidQuantityIsZero) {
  const auto level = make_level(LevelAttributes{14.0, 0}, LevelAttributes{});
  EXPECT_FALSE(has_valid_bid(level));
}

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidBidFalseIfBidQuantityIsLessThenZero) {
  const auto level =
      make_level(LevelAttributes{25.0, -114.5}, LevelAttributes{});
  EXPECT_FALSE(has_valid_bid(level));
}

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidBidTrueIfBidPriceAndQuantityAreSet) {
  const auto level =
      make_level(LevelAttributes{12.0, 114.5}, LevelAttributes{});
  EXPECT_TRUE(has_valid_bid(level));
}

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidOfferFalseIfOfferPriceAndQuantityAreNotSet) {
  const auto level = make_level(LevelAttributes{}, LevelAttributes{});
  EXPECT_FALSE(has_valid_offer(level));
}

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidOfferFalseIfOfferQuantityIsNotSet) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{12.0, std::nullopt});
  EXPECT_FALSE(has_valid_offer(level));
}

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidOfferFalseIfOfferPriceIsNotSet) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{std::nullopt, 114.5});
  EXPECT_FALSE(has_valid_offer(level));
}

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidOfferFalseIfOfferQuantityIsZero) {
  const auto level = make_level(LevelAttributes{}, LevelAttributes{14.0, 0});
  EXPECT_FALSE(has_valid_offer(level));
}

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidOfferFalseIfOfferQuantityIsLessThenZero) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{25.0, -114.5});
  EXPECT_FALSE(has_valid_offer(level));
}

TEST_F(GeneratorHistoricalLevelChecks,
       ReturnsHasValidOfferTrueIfOfferPriceAndQuantityAreSet) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{12.0, 114.5});
  EXPECT_TRUE(has_valid_offer(level));
}

}  // namespace
}  // namespace simulator::generator::historical::test