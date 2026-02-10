#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "api/models/market_phase.hpp"
#include "ih/common/exceptions.hpp"
#include "tests/test_utils/matchers.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct DataLayerModelsMarketPhasePatch : public Test {
  MarketPhase::Patch patch;
};

TEST_F(DataLayerModelsMarketPhasePatch, SetsPhase) {
  ASSERT_EQ(patch.phase(), std::nullopt);
  patch.with_phase(MarketPhase::Phase::Open);
  EXPECT_THAT(patch.phase(), Optional(Eq(MarketPhase::Phase::Open)));
}

TEST_F(DataLayerModelsMarketPhasePatch, SetsStartTime) {
  ASSERT_EQ(patch.start_time(), std::nullopt);
  patch.with_start_time("12:01:50");
  EXPECT_THAT(patch.start_time(), Optional(Eq("12:01:50")));
}

TEST_F(DataLayerModelsMarketPhasePatch, SetsEndTime) {
  ASSERT_EQ(patch.end_time(), std::nullopt);
  patch.with_end_time("14:56:42");
  EXPECT_THAT(patch.end_time(), Optional(Eq("14:56:42")));
}

TEST_F(DataLayerModelsMarketPhasePatch, SetsEndTimeRangeNull) {
  ASSERT_FALSE(patch.end_time_range().has_value());
  patch.with_end_time_range(std::nullopt);
  EXPECT_THAT(patch.end_time_range(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelsMarketPhasePatch, SetsEndTimeRange) {
  ASSERT_FALSE(patch.end_time_range().has_value());
  patch.with_end_time_range(-1);
  EXPECT_THAT(patch.end_time_range(), IsPatchFieldWithValue(Optional(Eq(-1))));
}

TEST_F(DataLayerModelsMarketPhasePatch, SetsAllowCancelsNull) {
  ASSERT_FALSE(patch.allow_cancels().has_value());
  patch.with_allow_cancels(std::nullopt);
  EXPECT_THAT(patch.allow_cancels(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerModelsMarketPhasePatch, SetsAllowCancels) {
  ASSERT_FALSE(patch.allow_cancels().has_value());
  patch.with_allow_cancels(true);
  EXPECT_THAT(patch.allow_cancels(), IsPatchFieldWithValue(Optional(Eq(true))));
}

struct DataLayerModelsMarketPhase : public Test {
  static auto make_default_patch() -> MarketPhase::Patch {
    return MarketPhase::Patch{}
        .with_phase(MarketPhase::Phase::Open)
        .with_start_time("12:00:00")
        .with_end_time("18:00:00");
  }
};

TEST_F(DataLayerModelsMarketPhase,
       ThrowsRequiredAttributeMissingOnCreationWhenPhaseIsMissed) {
  MarketPhase::Patch patch;
  patch.with_start_time("12:00:00").with_end_time("18:00:00");

  ASSERT_EQ(patch.phase(), std::nullopt);
  EXPECT_THROW((void)MarketPhase::create(patch, "LSE"),
               RequiredAttributeMissing);
}

TEST_F(DataLayerModelsMarketPhase, CreatesWhenPhaseIsSet) {
  MarketPhase::Patch patch;
  patch.with_phase(MarketPhase::Phase::Open)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00");

  const auto market_phase = MarketPhase::create(patch, "LSE");
  EXPECT_EQ(market_phase.phase(), MarketPhase::Phase::Open);
}

TEST_F(DataLayerModelsMarketPhase,
       ThrowsRequiredAttributeMissingOnCreationWhenStartTimeIsMissed) {
  MarketPhase::Patch patch;
  patch.with_phase(MarketPhase::Phase::Open).with_end_time("18:00:00");

  ASSERT_EQ(patch.start_time(), std::nullopt);
  EXPECT_THROW((void)MarketPhase::create(patch, "LSE"),
               RequiredAttributeMissing);
}

TEST_F(DataLayerModelsMarketPhase, CreatesWhenStartTimeIsSet) {
  const auto patch = make_default_patch();

  const auto market_phase = MarketPhase::create(patch, "LSE");
  EXPECT_EQ(market_phase.start_time(), "12:00:00");
}

TEST_F(DataLayerModelsMarketPhase,
       ThrowsRequiredAttributeMissingOnCreationWhenEndTimeIsMissed) {
  MarketPhase::Patch patch;
  patch.with_phase(MarketPhase::Phase::Open).with_start_time("12:00:00");

  ASSERT_EQ(patch.end_time(), std::nullopt);
  EXPECT_THROW((void)MarketPhase::create(patch, "LSE"),
               RequiredAttributeMissing);
}

TEST_F(DataLayerModelsMarketPhase, CreatesWhenEndTimeIsSet) {
  MarketPhase::Patch patch;
  patch.with_phase(MarketPhase::Phase::Open)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00");

  const auto market_phase = MarketPhase::create(patch, "LSE");
  EXPECT_EQ(market_phase.end_time(), "18:00:00");
}

TEST_F(DataLayerModelsMarketPhase, CreatesWhenAllowCancelsIsSet) {
  const auto patch = make_default_patch().with_allow_cancels(true);

  const auto market_phase = MarketPhase::create(patch, "LSE");
  EXPECT_EQ(market_phase.allow_cancels(), true);
}

TEST_F(DataLayerModelsMarketPhase, CreatesWhenEndTimeRangeIsMissed) {
  const auto patch = make_default_patch();
  ASSERT_FALSE(patch.end_time_range().has_value());

  const auto market_phase = MarketPhase::create(patch, "LSE");
  EXPECT_EQ(market_phase.end_time_range(), std::nullopt);
}

TEST_F(DataLayerModelsMarketPhase, CreatesWhenEndTimeRangeIsSet) {
  const auto patch = make_default_patch().with_end_time_range(42);

  const auto market_phase = MarketPhase::create(patch, "LSE");
  EXPECT_THAT(market_phase.end_time_range(), Optional(Eq(42)));
}

TEST_F(DataLayerModelsMarketPhase, SetsVenueIDOnCreation) {
  const auto patch = make_default_patch();

  const auto market_phase = MarketPhase::create(patch, "LSE");
  EXPECT_EQ(market_phase.venue_id(), "LSE");
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::test