#ifndef SIMULATOR_HTTP_TESTS_TEST_UTILS_MATCHERS_HPP_
#define SIMULATOR_HTTP_TESTS_TEST_UTILS_MATCHERS_HPP_

#include <gmock/gmock.h>

namespace simulator::http::json::test {

using namespace ::testing;

MATCHER_P2(HasString, key, expected, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsString(), result_listener) &&
         ExplainMatchResult(
             StrEq(expected), iter->value.GetString(), result_listener);
}

MATCHER_P2(HasBool, key, expected, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsBool(), result_listener) &&
         ExplainMatchResult(
             Eq(expected), iter->value.GetBool(), result_listener);
}

MATCHER_P2(HasDouble, key, expected, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsDouble(), result_listener) &&
         ExplainMatchResult(
             Eq(expected), iter->value.GetDouble(), result_listener);
}

MATCHER_P2(HasInt64, key, expected, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsInt64(), result_listener) &&
         ExplainMatchResult(
             Eq(expected), iter->value.GetInt(), result_listener);
}

MATCHER_P2(HasUInt64, key, expected, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsUint64(), result_listener) &&
         ExplainMatchResult(
             Eq(expected), iter->value.GetUint64(), result_listener);
}

}  // namespace simulator::http::json::test

#endif  // SIMULATOR_HTTP_TESTS_TEST_UTILS_MATCHERS_HPP_
