#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/historical/data/record.hpp"

namespace simulator::generator::historical::test {
namespace {

using namespace ::testing;

TEST(GeneratorHistoricalLevel, DoesNotStoreDefaultValues) {
  const Level level;

  EXPECT_FALSE(level.bid_price().has_value());
  EXPECT_FALSE(level.offer_price().has_value());

  EXPECT_FALSE(level.bid_quantity().has_value());
  EXPECT_FALSE(level.offer_quantity().has_value());

  EXPECT_FALSE(level.bid_counterparty().has_value());
  EXPECT_FALSE(level.offer_counterparty().has_value());
}

TEST(GeneratorHistoricalLevel, FormatsToString) {
  const auto builder = Level::Builder()
                           .with_bid_price(1.1)
                           .with_bid_quantity(2.2)
                           .with_bid_counterparty("BID-PARTY-1")
                           .with_offer_price(3.3)
                           .with_offer_quantity(4.4)
                           .with_offer_counterparty("OFFER-PARTY-1");
  const Level level = Level::Builder::construct(builder);

  ASSERT_EQ(fmt::to_string(level),
            "{ Bid={ Price=1.1 Qty=2.2 Counterparty=BID-PARTY-1 } "
            "Offer={ Price=3.3 Qty=4.4 Counterparty=OFFER-PARTY-1 } }");
}

struct GeneratorHistoricalLevelBuilder : public testing::Test {
  Level::Builder builder;
};

TEST_F(GeneratorHistoricalLevelBuilder, EmptyBuilderConstructsEmptyLevel) {
  EXPECT_TRUE(builder.empty());

  const auto level = Level::Builder::construct(std::move(builder));

  EXPECT_FALSE(level.bid_price().has_value());
  EXPECT_FALSE(level.offer_price().has_value());

  EXPECT_FALSE(level.bid_quantity().has_value());
  EXPECT_FALSE(level.offer_quantity().has_value());

  EXPECT_FALSE(level.bid_counterparty().has_value());
  EXPECT_FALSE(level.offer_counterparty().has_value());
}

TEST_F(GeneratorHistoricalLevelBuilder, ConstructsWithBidData) {
  constexpr double price = 0.124;
  constexpr double quantity = 153.556;
  const std::string party{"BID-PARTY-1"};

  builder.with_bid_price(price)
      .with_bid_quantity(quantity)
      .with_bid_counterparty(party);

  EXPECT_FALSE(builder.empty());

  const auto level = Level::Builder::construct(std::move(builder));
  EXPECT_THAT(level.bid_price(), Optional(DoubleEq(price)));
  EXPECT_THAT(level.bid_quantity(), Optional(DoubleEq(quantity)));
  EXPECT_THAT(level.bid_counterparty(), Optional(Eq(party)));

  EXPECT_FALSE(level.offer_price().has_value());
  EXPECT_FALSE(level.offer_quantity().has_value());
  EXPECT_FALSE(level.offer_counterparty().has_value());
}

TEST_F(GeneratorHistoricalLevelBuilder, ConstructsWithOfferData) {
  constexpr double price = 0.124;
  constexpr double quantity = 153.556;
  const std::string party{"OFFER-PARTY-1"};

  builder.with_offer_price(price)
      .with_offer_quantity(quantity)
      .with_offer_counterparty(party);

  const auto level = Level::Builder::construct(std::move(builder));
  EXPECT_THAT(level.offer_price(), Optional(DoubleEq(price)));
  EXPECT_THAT(level.offer_quantity(), Optional(DoubleEq(quantity)));
  EXPECT_THAT(level.offer_counterparty(), Optional(Eq(party)));

  EXPECT_FALSE(level.bid_price().has_value());
  EXPECT_FALSE(level.bid_quantity().has_value());
  EXPECT_FALSE(level.bid_counterparty().has_value());
}

}  // namespace
}  // namespace simulator::generator::historical::test