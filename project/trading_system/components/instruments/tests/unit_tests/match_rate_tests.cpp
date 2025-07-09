#include <gtest/gtest.h>

#include "ih/lookup/match_rate.hpp"

namespace simulator::trading_system::instrument::lookup::test {
namespace {

TEST(InstrumentMatchRate, NoMatchIsTheLowestConstant) {
  EXPECT_LT(MatchRate::NoMatch, MatchRate::Match);
  EXPECT_LT(MatchRate::NoMatch, MatchRate::Unmatchable);
}

TEST(InstrumentMatchRate, MatchIsTheHighestConstant) {
  EXPECT_GT(MatchRate::Match, MatchRate::NoMatch);
  EXPECT_GT(MatchRate::Match, MatchRate::Unmatchable);
}

TEST(InstrumentMatchRate, MatchesSumUp) {
  EXPECT_EQ(MatchRate::Match + MatchRate::Match, MatchRate{2U});
}

TEST(InstrumentMatchRate, RateIsLessGreaterComparable) {
  EXPECT_LT(MatchRate::Match, MatchRate::Match + MatchRate::Match);
  EXPECT_GT(MatchRate::Match + MatchRate::Match, MatchRate::Match);
}

TEST(InstrumentMatchRate, NoMatchValueConvertsResultToNoMatch) {
  EXPECT_EQ(MatchRate::NoMatch + MatchRate::Match, MatchRate::NoMatch);
}

TEST(InstrumentMatchRate, UnmatchableDoesNotAffectTheResult) {
  EXPECT_EQ(MatchRate::Unmatchable + MatchRate::Match, MatchRate::Match);
}

}  // namespace
}  // namespace simulator::trading_system::instrument::lookup::test