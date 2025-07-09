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
       ReturnsProcessableIfBidPriceAndQuantityAreNotSet) {
  const auto level = make_level(LevelAttributes{}, LevelAttributes{});
  EXPECT_TRUE(RecordApplier::RecordChecker::is_processable(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsNotProcessableIfBidQuantityIsNotSet) {
  const auto level =
      make_level(LevelAttributes{12.0, std::nullopt}, LevelAttributes{});
  EXPECT_FALSE(RecordApplier::RecordChecker::is_processable(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsNotProcessableIfBidPriceIsNotSet) {
  const auto level =
      make_level(LevelAttributes{std::nullopt, 114.5}, LevelAttributes{});
  EXPECT_FALSE(RecordApplier::RecordChecker::is_processable(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsProcessableIfBidPriceAndQuantityAreSet) {
  const auto level =
      make_level(LevelAttributes{12.0, 114.5}, LevelAttributes{});
  EXPECT_TRUE(RecordApplier::RecordChecker::is_processable(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsProcessableIfOfferPriceAndQuantityAreNotSet) {
  const auto level = make_level(LevelAttributes{}, LevelAttributes{});
  EXPECT_TRUE(RecordApplier::RecordChecker::is_processable(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsNotProcessableIfOfferQuantityIsNotSet) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{12.0, std::nullopt});
  EXPECT_FALSE(RecordApplier::RecordChecker::is_processable(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsNotProcessableIfOfferPriceIsNotSet) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{std::nullopt, 114.5});
  EXPECT_FALSE(RecordApplier::RecordChecker::is_processable(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsProcessableIfOfferPriceAndQuantityAreSet) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{12.0, 114.5});
  EXPECT_TRUE(RecordApplier::RecordChecker::is_processable(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsLevelNotHasBidPartIfBidPriceAndQuantityAreNotSet) {
  const auto level = make_level(LevelAttributes{}, LevelAttributes{});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_bid_part(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsLevelHasBidPartIfBidPriceAndQuantityAreSet) {
  const auto level =
      make_level(LevelAttributes{120.4, 43.3}, LevelAttributes{});
  EXPECT_TRUE(RecordApplier::RecordChecker::has_bid_part(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsLevelNotHasOfferPartIfOfferPriceAndQuantityAreNotSet) {
  const auto level = make_level(LevelAttributes{}, LevelAttributes{});
  EXPECT_FALSE(RecordApplier::RecordChecker::has_offer_part(level));
}

TEST_F(GeneratorHistoricalRecordChecker,
       ReturnsLevelHasOfferPartIfOfferPriceAndQuantityAreSet) {
  const auto level =
      make_level(LevelAttributes{}, LevelAttributes{453.5, 33.4});
  EXPECT_TRUE(RecordApplier::RecordChecker::has_offer_part(level));
}

}  // namespace
}  // namespace simulator::generator::historical::test