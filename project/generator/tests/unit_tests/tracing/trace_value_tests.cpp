#include <gtest/gtest.h>

#include "ih/tracing/trace_value.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::trace::test {
namespace {

TEST(Generator_Trace_TraceValue, ArithmeticType) {
  constexpr std::string_view key = "DoubleValue";
  constexpr double value = 23.43;

  const auto trace_value = make_value(key, value);
  static_assert(decltype(trace_value)::holds_arithmetic_type());

  EXPECT_EQ(trace_value.get_key(), key);
  EXPECT_EQ(trace_value.get_value(), value);
  EXPECT_FALSE(trace_value.holds_comment());
}

TEST(Generator_Trace_TraceValue, String) {
  constexpr std::string_view key = "StringValue";
  const std::string value = "An awesome string";

  const auto trace_value = make_value(key, value);
  static_assert(decltype(trace_value)::holds_string());

  EXPECT_EQ(trace_value.get_key(), key);
  EXPECT_EQ(trace_value.get_value(), value);
  EXPECT_FALSE(trace_value.holds_comment());
}

TEST(Generator_Trace_TraceValue, StringView) {
  constexpr std::string_view key = "StringViewValue";
  constexpr std::string_view value = "An awesome string view";

  const auto trace_value = make_value(key, value);
  static_assert(decltype(trace_value)::holds_string_view());

  EXPECT_EQ(trace_value.get_key(), key);
  EXPECT_EQ(trace_value.get_value(), value);
  EXPECT_FALSE(trace_value.holds_comment());
}

TEST(Generator_Trace_TraceValue, WithComment) {
  constexpr std::string_view key = "DoubleValue";
  constexpr std::string_view value = "An awesome string view";
  constexpr std::string_view comment = "My cool comment to the value";

  const auto trace_value = make_commented_value(key, value, comment);

  ASSERT_TRUE(trace_value.holds_comment());
  EXPECT_EQ(trace_value.get_comment(), comment);
}

}  // namespace
}  // namespace simulator::generator::trace::test