#ifndef SIMULATOR_LOG_TESTS_TEST_UTILS_MATCHERS_HPP_
#define SIMULATOR_LOG_TESTS_TEST_UTILS_MATCHERS_HPP_

namespace simulator::log::test {

using namespace ::testing;

MATCHER_P2(HasString, key, expected, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsString(), result_listener) &&
         ExplainMatchResult(
             StrEq(expected), iter->value.GetString(), result_listener);
}

MATCHER_P2(HasStringMatches, key, matcher, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsString(), result_listener) &&
         ExplainMatchResult(matcher, iter->value.GetString(), result_listener);
}

}  // namespace simulator::log::test

#endif  // SIMULATOR_LOG_TESTS_TEST_UTILS_MATCHERS_HPP_
