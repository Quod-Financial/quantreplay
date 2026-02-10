#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <type_traits>

#include "api/models/patch_field.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

MATCHER(HasValue, "") {
  return (arg.state() == std::remove_cvref_t<decltype(arg)>::State::Value ||
          arg.state() == std::remove_cvref_t<decltype(arg)>::State::Null) &&
         static_cast<bool>(arg);
}

MATCHER(StoresNull, "") {
  return ExplainMatchResult(Eq(std::remove_cvref_t<decltype(arg)>::State::Null),
                            arg.state(),
                            result_listener) &&
         ExplainMatchResult(std::nullopt, arg.value(), result_listener);
}

MATCHER_P(StoresValue, matcher, "") {
  return ExplainMatchResult(
             Eq(std::remove_cvref_t<decltype(arg)>::State::Value),
             arg.state(),
             result_listener) &&
         ExplainMatchResult(matcher, arg.value(), result_listener);
}

TEST(DataLayerModelsPatchField, StateIsUnsetByDefault) {
  const PatchField<std::string> field;
  ASSERT_EQ(field.state(), PatchField<std::string>::State::Unset);
}

TEST(DataLayerModelsPatchField, DefinedByLValueStoresValue) {
  const std::string value{"string"};
  const PatchField<std::string> field{value};
  ASSERT_THAT(field, StoresValue(Optional(StrEq(value))));
}

TEST(DataLayerModelsPatchField, DefinedByNullLOptionalStoresNull) {
  const std::optional<std::string> value;
  const PatchField<std::string> field{value};
  ASSERT_THAT(field, StoresNull());
}

TEST(DataLayerModelsPatchField, DefinedByLOptionalStoresValue) {
  constexpr std::optional<int> value{42};
  constexpr PatchField<int> field{value};
  ASSERT_THAT(field, StoresValue(value));
}

TEST(DataLayerModelsPatchField, DefinedByRValueStoresValue) {
  const PatchField<std::string> field{std::string{"string"}};
  ASSERT_THAT(field, StoresValue(Optional(StrEq("string"))));
}

TEST(DataLayerModelsPatchField, DefinedByNullROptionalStoresNull) {
  constexpr PatchField<std::int32_t> field{std::optional<std::int32_t>{}};
  ASSERT_THAT(field, StoresNull());
}

TEST(DataLayerModelsPatchField, DefinedByROptionalStoresValue) {
  constexpr PatchField<std::int32_t> field{std::optional<std::int32_t>{42}};
  ASSERT_THAT(field, StoresValue(Optional(42)));
}

TEST(DataLayerModelsPatchFieldOperatorAssign, AssignsValueByLValueStoresValue) {
  PatchField<std::string> field;
  const std::string value{"string"};
  field = value;
  ASSERT_THAT(field, StoresValue(Optional(value)));
}

TEST(DataLayerModelsPatchFieldOperatorAssign,
     AssignsValueByLNullOptionalStoresNull) {
  PatchField<std::string> field;
  const std::optional<std::string> value;
  field = value;
  ASSERT_THAT(field, StoresNull());
}

TEST(DataLayerModelsPatchFieldOperatorAssign,
     AssignsValueByLOptionalStoresValue) {
  PatchField<std::string> field;
  const std::optional<std::string> value{"string"};
  field = value;
  ASSERT_THAT(field, StoresValue(value));
}

TEST(DataLayerModelsPatchFieldOperatorAssign, AssignsValueByRValueStoresValue) {
  PatchField<std::string> field;
  field = std::string{"string"};
  ASSERT_THAT(field, StoresValue(Optional(StrEq("string"))));
}

TEST(DataLayerModelsPatchFieldOperatorAssign,
     AssignsValueByNullROptionalValueStoresNull) {
  PatchField<std::string> field;
  field = std::optional<std::string>{};
  ASSERT_THAT(field, StoresNull());
}

TEST(DataLayerModelsPatchFieldOperatorAssign,
     AssignsValueByROptionalValueStoresValue) {
  PatchField<std::string> field;
  field = std::optional<std::string>{"string"};
  ASSERT_THAT(field, StoresValue(Optional(StrEq("string"))));
}

TEST(DataLayerModelsPatchFieldHasValue, ReturnsFalseWhenNoValueIsSet) {
  constexpr PatchField<int> field;
  ASSERT_THAT(field, Not(HasValue()));
}

TEST(DataLayerModelsPatchFieldHasValue, ReturnsTrueWhenValueIsSet) {
  constexpr PatchField<int> field{42};
  ASSERT_THAT(field, HasValue());
}

TEST(DataLayerModelsPatchFieldHasValue, ReturnsTrueWhenValueIsNullopt) {
  constexpr PatchField<int> field{std::nullopt};
  ASSERT_THAT(field, HasValue());
}

TEST(DataLayerModelsPatchFieldInnerValue, ReturnsInnerValue) {
  constexpr PatchField<int> field{42};
  ASSERT_EQ(field.inner_value(), 42);
}

TEST(DataLayerModelsPatchFieldInnerValueOr, ReturnsDefaultWhenNoValueIsSet) {
  constexpr PatchField<int> field;
  ASSERT_EQ(field.inner_value_or(99), 99);
}

TEST(DataLayerModelsPatchFieldInnerValueOr, ReturnsDefaultWhenValueIsNullopt) {
  constexpr PatchField<int> field{std::nullopt};
  ASSERT_EQ(field.inner_value_or(99), 99);
}

TEST(DataLayerModelsPatchFieldInnerValueOr,
     ReturnsDefaultWhenValueIsEmptyOptional) {
  constexpr PatchField<int> field{std::optional<int>{}};
  ASSERT_EQ(field.inner_value_or(99), 99);
}

TEST(DataLayerModelsPatchFieldInnerValueOr, ReturnsInnerValueWhenSet) {
  constexpr PatchField<int> field{42};
  ASSERT_EQ(field.inner_value_or(99), 42);
}

TEST(DataLayerModelsPatchFieldInnerValueOr, ReturnsInnerStringValueWhenSet) {
  const PatchField<std::string> field{std::string{"test"}};
  ASSERT_EQ(field.inner_value_or("default"), "test");
}

}  // namespace
}  // namespace simulator::data_layer::test
