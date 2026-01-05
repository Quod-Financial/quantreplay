#ifndef MARKETSIMULATOR_MATCHERS_HPP
#define MARKETSIMULATOR_MATCHERS_HPP

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

#endif  // MARKETSIMULATOR_MATCHERS_HPP
