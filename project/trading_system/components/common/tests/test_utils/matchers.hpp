#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_TESTS_TEST_UTILS_MATCHERS_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_TESTS_TEST_UTILS_MATCHERS_HPP_

#include <gmock/gmock.h>

namespace simulator::trading_system::test {

using namespace ::testing;

MATCHER_P(HasNull, key, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsNull(), result_listener);
}

MATCHER_P2(HasString, key, expected, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsString(), result_listener) &&
         ExplainMatchResult(
             StrEq(expected), iter->value.GetString(), result_listener);
}

MATCHER_P2(HasInt, key, expected, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsInt(), result_listener) &&
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

MATCHER_P2(HasDouble, key, expected, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsDouble(), result_listener) &&
         ExplainMatchResult(
             Eq(expected), iter->value.GetDouble(), result_listener);
}

MATCHER_P2(HasArraySize, key, size, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsArray(), result_listener) &&
         ExplainMatchResult(Eq(static_cast<rapidjson::SizeType>(size)),
                               iter->value.GetArray().Size(),
                               result_listener);
}

MATCHER_P(HasObject, key, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsObject(), result_listener);
}

MATCHER_P2(HasInner, key, inner_matcher, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsObject(), result_listener) &&
         ExplainMatchResult(
             inner_matcher, iter->value.GetObject(), result_listener);
}

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_TESTS_TEST_UTILS_MATCHERS_HPP_
