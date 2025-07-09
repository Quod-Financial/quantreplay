#include <gtest/gtest.h>

#include "ih/common/validation/conclusion.hpp"

namespace simulator::trading_system::matching_engine::test {

TEST(MatchingEngineValidationConclusion, ReturnsFalseFailedByDefault) {
  ASSERT_FALSE(Conclusion{}.failed());
}

TEST(MatchingEngineValidationConclusion, ReturnsEmptyErrorByDefault) {
  ASSERT_TRUE(Conclusion{}.error().empty());
}

TEST(MatchingEngineValidationConclusion, FailureCreatesFailedConclusion) {
  const auto conclusion = Conclusion::failure({});
  ASSERT_TRUE(conclusion.failed());
}

TEST(MatchingEngineValidationConclusion, FailureSetsError) {
  const auto conclusion = Conclusion::failure("error");
  ASSERT_EQ(conclusion.error(), "error");
}

TEST(MatchingEngineValidationConclusion, SuccessCreatesNotFailedConclusion) {
  const auto conclusion = Conclusion::success();
  ASSERT_FALSE(conclusion.failed());
}

TEST(MatchingEngineValidationConclusion, SuccessSetsEmptyError) {
  const auto conclusion = Conclusion::success();
  ASSERT_TRUE(conclusion.error().empty());
}

}  // namespace simulator::trading_system::matching_engine::test
