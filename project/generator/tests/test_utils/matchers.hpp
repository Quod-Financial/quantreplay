#ifndef SIMULATOR_GENERATOR_TESTS_TEST_UTILS_MATCHERS_HPP_
#define SIMULATOR_GENERATOR_TESTS_TEST_UTILS_MATCHERS_HPP_

#include <gmock/gmock.h>

namespace simulator::generator {

using namespace ::testing;

MATCHER_P(IsExpected, matcher, "") {
  return ExplainMatchResult(IsTrue(), arg.has_value(), result_listener) &&
         ExplainMatchResult(matcher, arg.value(), result_listener);
}

MATCHER_P(IsUnexpected, error_message, "") {
  return ExplainMatchResult(IsFalse(), arg.has_value(), result_listener) &&
         ExplainMatchResult(StrEq(error_message), arg.error(), result_listener);
}

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_TESTS_TEST_UTILS_MATCHERS_HPP_
