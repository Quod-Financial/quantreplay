#include <gtest/gtest.h>

#include <optional>
#include <tuple>

#include "ih/historical/data/record.hpp"
#include "ih/historical/record_applier.hpp"

namespace simulator::generator::historical::test {
namespace {

class GeneratorHistoricalRecordChecker : public testing::Test {
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

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidBidFalseIfBidPriceAndQuantityAreNotSet) {
  const auto level = make_level(LevelAttributes{}, LevelAttributes{});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_valid_bid(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidBidFalseIfBidQuantityIsNotSet) {
  const auto level =
      make_level(LevelAttributes{12.0, std::nullopt}, LevelAttributes{});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_valid_bid(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidBidFalseIfBidPriceIsNotSet) {
  const auto level =
      make_level(LevelAttributes{std::nullopt, 114.5}, LevelAttributes{});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_valid_bid(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidBidFalseIfBidQuantityIsZero) {
  const auto level =
      make_level(LevelAttributes{std::nullopt, 0}, LevelAttributes{});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_valid_bid(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidBidFalseIfBidQuantityIsLessThenZero) {
  const auto level =
      make_level(LevelAttributes{std::nullopt, -25}, LevelAttributes{});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_valid_bid(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidBidTrueIfBidPriceAndQuantityAreSet) {
  const auto level =
      make_level(LevelAttributes{12.0, 114.5}, LevelAttributes{});
  EXPECT_TRUE(RecordApplier::RecordChecker::has_valid_bid(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidOfferFalseIfOfferPriceAndQuantityAreNotSet) {
  const auto level = make_level(LevelAttributes{}, LevelAttributes{});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_valid_offer(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidOfferFalseIfOfferQuantityIsNotSet) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{12.0, std::nullopt});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_valid_offer(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidOfferFalseIfOfferPriceIsNotSet) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{std::nullopt, 114.5});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_valid_offer(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidOfferFalseIfOfferQuantityIsZero) {
  const auto level = make_level(LevelAttributes{}, LevelAttributes{14.0, 0});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_valid_offer(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidOfferFalseIfOfferQuantityIsLessThenZero) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{25.0, -114.5});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_valid_offer(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsHasValidOfferTrueIfOfferPriceAndQuantityAreSet) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{12.0, 114.5});
  EXPECT_TRUE(RecordApplier::RecordChecker::has_valid_offer(level));
}

}  // namespace
}  // namespace simulator::generator::historical::test