#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "api/predicate/lexeme.hpp"
#include "common/model.hpp"

namespace simulator::data_layer::predicate::test {
namespace {

using namespace ::testing;

namespace {

class FormatterMock {
 public:
  using Field = TestModel::Attribute;
  using CustomType = TestModel::CustomFieldType;
  using Value = std::variant<bool,
                             std::int64_t,
                             std::uint64_t,
                             std::double_t,
                             std::string,
                             CustomType>;

  auto operator()(Field field, BasicOperation operation, bool value) -> void {
    format_boolean(field, operation, value);
  }

  auto operator()(Field field, BasicOperation operation, std::int64_t value)
      -> void {
    format_int(field, operation, value);
  }

  auto operator()(Field field, BasicOperation operation, std::uint64_t value)
      -> void {
    format_uint(field, operation, value);
  }

  auto operator()(Field field, BasicOperation operation, std::double_t value)
      -> void {
    format_double(field, operation, value);
  }

  auto operator()(Field field,
                  BasicOperation operation,
                  const std::string& value) -> void {
    format_string(field, operation, value);
  }

  auto operator()(Field field, BasicOperation operation, CustomType value)
      -> void {
    format_custom(field, operation, value);
  }

  auto operator()(Field field, const std::vector<Value>& values) -> void {
    format_in(field, values);
  }

  auto operator()(CompositeOperation operation) -> void {
    format_composite(operation);
  }

  auto operator()([[maybe_unused]] SubExpressionBegin expression) -> void {
    format_sub_expression_begin();
  }

  auto operator()([[maybe_unused]] SubExpressionEnd expression) -> void {
    format_sub_expression_end();
  }

  MOCK_METHOD(void, format_boolean, (Field, BasicOperation, bool));
  MOCK_METHOD(void, format_int, (Field, BasicOperation, std::int64_t));
  MOCK_METHOD(void, format_uint, (Field, BasicOperation, std::uint64_t));
  MOCK_METHOD(void, format_double, (Field, BasicOperation, std::double_t));
  MOCK_METHOD(void, format_string, (Field, BasicOperation, const std::string&));
  MOCK_METHOD(void, format_custom, (Field, BasicOperation, CustomType));

  MOCK_METHOD(void, format_in, (Field, (const std::vector<Value>&)));

  MOCK_METHOD(void, format_composite, (CompositeOperation));

  MOCK_METHOD(void, format_sub_expression_begin, ());
  MOCK_METHOD(void, format_sub_expression_end, ());
};

}  // namespace

class DataLayer_Predicate_BasicPredicateLexeme : public ::testing::Test {
 public:
  using Field = TestModel::Attribute;
  using BasicPredicate = predicate::BasicPredicate<TestModel>;

  static_assert(std::is_copy_constructible_v<BasicPredicate>);
  static_assert(std::is_move_constructible_v<BasicPredicate>);

  template <typename... Args>
  static auto make_predicate(Args&&... args) -> BasicPredicate {
    return BasicPredicate{std::forward<Args>(args)...};
  }
};

TEST_F(DataLayer_Predicate_BasicPredicateLexeme,
       Accept_Attribute_BooleanField) {
  constexpr bool value = true;
  const BasicPredicate predicate =
      make_predicate(Field::BooleanField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_boolean(Eq(Field::BooleanField), _, _))
      .Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme,
       Accept_Attribute_IntegerField) {
  constexpr std::int64_t value = 1;
  const BasicPredicate predicate =
      make_predicate(Field::IntegerField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_int(Eq(Field::IntegerField), _, _)).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme,
       Accept_Attribute_UnsIntegerField) {
  constexpr std::uint64_t value = 1;
  const BasicPredicate predicate =
      make_predicate(Field::UnsignedIntegerField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_uint(Eq(Field::UnsignedIntegerField), _, _))
      .Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme,
       Accept_Attribute_DecimalField) {
  constexpr std::double_t value = 1.;
  const BasicPredicate predicate =
      make_predicate(Field::DecimalField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_double(Eq(Field::DecimalField), _, _)).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Attribute_StringField) {
  const std::string value = "ABC";
  const BasicPredicate predicate =
      make_predicate(Field::StringField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_string(Eq(Field::StringField), _, _)).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Attribute_CustomField) {
  constexpr auto value = TestModel::CustomFieldType::Value1;
  const BasicPredicate predicate =
      make_predicate(Field::CustomField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_custom(Eq(Field::CustomField), _, _)).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Operation_Eq) {
  constexpr bool value = true;
  const BasicPredicate predicate =
      make_predicate(Field::BooleanField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_boolean(_, Eq(BasicOperation::Eq), _)).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Operation_Neq) {
  constexpr bool value = true;
  const BasicPredicate predicate =
      make_predicate(Field::BooleanField, BasicOperation::Neq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_boolean(_, Eq(BasicOperation::Neq), _))
      .Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Operation_Less) {
  constexpr bool value = true;
  const BasicPredicate predicate =
      make_predicate(Field::BooleanField, BasicOperation::Less, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_boolean(_, Eq(BasicOperation::Less), _))
      .Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Operation_Greater) {
  constexpr bool value = true;
  const BasicPredicate predicate =
      make_predicate(Field::BooleanField, BasicOperation::Greater, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_boolean(_, Eq(BasicOperation::Greater), _))
      .Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Operation_LessEq) {
  constexpr bool value = true;
  const BasicPredicate predicate =
      make_predicate(Field::BooleanField, BasicOperation::LessEq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_boolean(_, Eq(BasicOperation::LessEq), _))
      .Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Operation_GreaterEq) {
  constexpr bool value = true;
  const BasicPredicate predicate =
      make_predicate(Field::BooleanField, BasicOperation::GreaterEq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_boolean(_, Eq(BasicOperation::GreaterEq), _))
      .Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Value_Boolean) {
  constexpr bool value = true;
  const BasicPredicate predicate =
      make_predicate(Field::BooleanField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_boolean(_, _, Eq(value))).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Value_Integer) {
  constexpr std::int64_t value = 42;
  const BasicPredicate predicate =
      make_predicate(Field::IntegerField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_int(_, _, Eq(value))).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Value_UnsignedInteger) {
  constexpr std::uint64_t value = 42;
  const BasicPredicate predicate =
      make_predicate(Field::UnsignedIntegerField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_uint(_, _, Eq(value))).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Value_Double) {
  constexpr std::double_t value = 42.42;
  const BasicPredicate predicate =
      make_predicate(Field::DecimalField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_double(_, _, DoubleEq(value))).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme, Accept_Value_String) {
  const std::string value = "ABC123";
  const BasicPredicate predicate =
      make_predicate(Field::StringField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_string(_, _, Eq(value))).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme,
       Accept_Value_CustomType_Value1) {
  constexpr auto value = TestModel::CustomFieldType::Value1;
  const BasicPredicate predicate =
      make_predicate(Field::CustomField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_custom(_, _, Eq(value))).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme,
       Accept_Value_CustomType_Value2) {
  constexpr auto value = TestModel::CustomFieldType::Value2;
  const BasicPredicate predicate =
      make_predicate(Field::CustomField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_custom(_, _, Eq(value))).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_BasicPredicateLexeme,
       Accept_Value_CustomType_Value3) {
  constexpr auto value = TestModel::CustomFieldType::Value3;
  const BasicPredicate predicate =
      make_predicate(Field::CustomField, BasicOperation::Eq, value);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_custom(_, _, Eq(value))).Times(1);

  predicate.accept(formatter);
}

class DataLayer_Predicate_InPredicateLexeme : public ::testing::Test {
 public:
  using Field = TestModel::Attribute;
  using CustomType = TestModel::CustomFieldType;
  using Value = FormatterMock::Value;
  using InPredicate = predicate::InPredicate<TestModel>;

  static_assert(std::is_copy_constructible_v<InPredicate>);
  static_assert(std::is_move_constructible_v<InPredicate>);

  template <typename... Args>
  static auto make_predicate(Args&&... args) -> InPredicate {
    return InPredicate{std::forward<Args>(args)...};
  }
};

TEST_F(DataLayer_Predicate_InPredicateLexeme, Accept_Attribute) {
  const InPredicate predicate =
      make_predicate(Field::StringField, std::vector<std::string>{"ABC"});

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_in(Eq(Field::StringField), _)).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_InPredicateLexeme, Accept_StringValues) {
  const InPredicate predicate = make_predicate(
      Field::StringField, std::vector<std::string>{"ABC", "DEF"});

  const std::vector<Value> expected{std::string{"ABC"}, std::string{"DEF"}};

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_in(_, Eq(expected))).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_InPredicateLexeme, Accept_IntegerValues) {
  const InPredicate predicate =
      make_predicate(Field::IntegerField, std::vector<std::int64_t>{1, 2, 3});

  const std::vector<Value> expected{
      std::int64_t{1}, std::int64_t{2}, std::int64_t{3}};

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_in(_, Eq(expected))).Times(1);

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_InPredicateLexeme, Accept_CustomTypeValues) {
  const InPredicate predicate = make_predicate(
      Field::CustomField,
      std::vector<CustomType>{CustomType::Value1, CustomType::Value3});

  const std::vector<Value> expected{CustomType::Value1, CustomType::Value3};

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_in(_, Eq(expected))).Times(1);

  predicate.accept(formatter);
}

class DataLayer_Predicate_CompositePredicateLexeme : public ::testing::Test {
 public:
  static_assert(std::is_copy_constructible_v<CompositePredicate>);
  static_assert(std::is_move_constructible_v<CompositePredicate>);

  static auto make_predicate(CompositeOperation operation)
      -> CompositePredicate {
    return CompositePredicate{operation};
  }
};

TEST_F(DataLayer_Predicate_CompositePredicateLexeme, Accept_Operation_AND) {
  const CompositePredicate predicate = make_predicate(CompositeOperation::And);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_composite(Eq(CompositeOperation::And)));

  predicate.accept(formatter);
}

TEST_F(DataLayer_Predicate_CompositePredicateLexeme, Accept_Operation_OR) {
  const CompositePredicate predicate = make_predicate(CompositeOperation::Or);

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_composite(Eq(CompositeOperation::Or)));

  predicate.accept(formatter);
}

class DataLayer_Predicate_SubExpressionLexeme : public ::testing::Test {
 public:
  static_assert(std::is_copy_constructible_v<SubExpressionBegin>);
  static_assert(std::is_move_constructible_v<SubExpressionBegin>);

  static_assert(std::is_copy_constructible_v<SubExpressionEnd>);
  static_assert(std::is_move_constructible_v<SubExpressionEnd>);

  static auto make_sub_expression_begin() -> SubExpressionBegin { return {}; }
  static auto make_sub_expression_end() -> SubExpressionEnd { return {}; }
};

TEST_F(DataLayer_Predicate_SubExpressionLexeme, Accept_SubExpressionBegin) {
  const SubExpressionBegin expression = make_sub_expression_begin();

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_sub_expression_begin).Times(1);

  expression.accept(formatter);
}

TEST_F(DataLayer_Predicate_SubExpressionLexeme, Accept_SubExpressionEnd) {
  const SubExpressionEnd expression = make_sub_expression_end();

  FormatterMock formatter;
  EXPECT_CALL(formatter, format_sub_expression_end).Times(1);

  expression.accept(formatter);
}

}  // namespace
}  // namespace simulator::data_layer::predicate::test